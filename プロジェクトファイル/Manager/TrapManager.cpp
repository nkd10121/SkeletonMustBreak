#include "TrapManager.h"
#include "Input.h"
#include "TrapBase.h"
#include "SpikeTrap.h"
#include "CutterTrap.h"
#include "HammerTrap.h"

#include "EffekseerForDXLib.h"
#include "SoundManager.h"
#include "EffectManager.h"
#include "ModelManager.h"
#include "CsvLoad.h"

namespace
{
	//罠モデル
	enum modelIdx :int
	{
		SpikeFrame,
		Spike,
		Cutter,
		Hammer,
		MAX
	};

	//トラップファイル名
	const char* const kFileName[modelIdx::MAX] =
	{
		"data/model/spike_frame.mv1",
		"data/model/spike.mv1",
		"data/model/cutter.mv1",
		"data/model/hammer.mv1",
	};

	//それぞれの罠のコスト
	//TODO:外部データ化
	constexpr int kSpikeCost = 400;
	constexpr int kCutterCost = 250;
}

/// <summary>
/// コンストラクタ
/// </summary>
/// <param name="trapNum">トラップの総数</param>
/// <param name="trapPositions">罠設置可能座標</param>
TrapManager::TrapManager(int trapNum, std::list<MyLib::Vec3>& trapPositions) :
	m_modelHandles(),
	m_traps(),
	m_trapPositions(),
	m_previewPos(),
	m_nowSlotIdx(0),
	m_isAlreadyPlaced(false),
	m_angle(0.0f),
	m_transparency(0.0f),
	m_pushZLCount(0)
{
	//モデルハンドルを取得する
	//TODO:そのステージで使用する罠のモデルのみをロードするようにしたい
	for (int i = 0; i < trapNum; i++)
	{
		int handle = ModelManager::GetInstance().GetModelHandle(kFileName[i]);
		m_modelHandles.emplace_back(handle);
	}

	//罠設置可能座標の取得
	m_trapPositions = trapPositions;

	//罠設置時のエフェクトをロード
	EffectManager::GetInstance().Load("CreateTrap", "data/effect/create_trap.efk",120);
}

/// <summary>
/// デストラクタ
/// </summary>
TrapManager::~TrapManager()
{
	for (auto& trap : m_modelHandles)
	{
		MV1DeleteModel(trap);
	}
	m_modelHandles.clear();

	m_traps.clear();
	m_trapPositions.clear();
}

/// <summary>
/// 初期化
/// </summary>
void TrapManager::Init()
{
}

/// <summary>
/// 更新
/// </summary>
/// <param name="input">入力情報</param>
/// <param name="slotNum">現在選択しているカーソルのインデックス</param>
/// <param name="playerPos">プレイヤー座標</param>
/// <param name="playerVec">プレイヤーの向き</param>
/// <param name="isPlayerSurvival">プレイヤーが生存中かどうか</param>
/// <param name="trapPoint">トラップ設置ポイント</param>
/// <param name="nowPhase">現在のフェーズ</param>
/// <param name="physics">物理クラス</param>
void TrapManager::Update(std::shared_ptr<Input>& input, int slotNum, MyLib::Vec3 playerPos, MyLib::Vec3 playerVec, bool isPlayerSurvival, int* trapPoint, int nowPhase, std::shared_ptr<MyLib::Physics> physics)
{
	//現在のプレイヤーのスロットを取得
	m_nowSlotIdx = slotNum;

	float defaultLength = 100.0f;

	//0のときはクロスボウを持っているため何も処理しない
	if (m_nowSlotIdx != 0)
	{
		m_angle += 0.04f;
		m_transparency = abs(sinf(m_angle) / 2.5f) + 0.1f;

		//線分の始点と終点を設定
		auto start = playerPos;
		auto end = playerPos + playerVec * 30;

		//設置可能なトラップの座標分回す
		for (auto pos : m_trapPositions)
		{
			//線分とトラップ設置可能座標の距離を計算する
			//float length = Segment_Point_MinLength(start.ConvertToVECTOR(), end.ConvertToVECTOR(), pos.ConvertToVECTOR());
			float length = (pos - end).Size();

			if (defaultLength > length)
			{
				defaultLength = length;
				m_previewPos = pos;
			}

		}

		//攻撃ボタンを押したとき
		if (input->GetIsPushedTriggerButton(true))
		{
			if (m_pushZLCount == 0 && !isPlayerSurvival)
			{

				m_isAlreadyPlaced = false;

				//すでに設置された罠の座標を見て
				for (auto trap : m_traps)
				{
					//すでにその座標に罠が設置されていたら
					if (trap->GetPos() == m_previewPos)
					{
						//すでに置かれているフラグをtrueにする
						m_isAlreadyPlaced = true;

						//プレイしている人に文字で
						// 「すでに罠が置かれています」的なのを表示してあげたい
					}
				}

				//置こうとしている座標に罠がすでに置かれていなかったら
				if (!m_isAlreadyPlaced)
				{
					//trueが帰ってきたら成功、falseが帰ってきたら失敗(ポイント不足)
					if (!CreateTrap(slotNum, physics, trapPoint))
					{
						//プレイしている人に文字で
						// 「ポイントが足りません」的なのを表示してあげたい

					}
					else	//成功していたらここでエフェクトを出す
					{
						//設置エフェクトの生成
						EffectManager::GetInstance().CreateEffect("CreateTrap", m_previewPos);
						//設置音を鳴らす
						SoundManager::GetInstance().PlaySE("trap");
					}
				}
			}

			m_pushZLCount++;
		}
		else
		{
			m_pushZLCount = 0;
		}
	}
	else
	{
		m_angle = 0.0f;
	}

	//すでに設置された罠の座標を見て
	for (auto trap : m_traps)
	{
		trap->Update();
	}
}

/// <summary>
/// 描画
/// </summary>
void TrapManager::Draw()
{
	//すでに設置された罠の座標を見て
	for (auto trap : m_traps)
	{
		trap->Draw();
	}

#ifdef _DEBUG
	//DrawFormatString(1100,600,0xff0000,"%f",m_transparency);
#endif
}

/// <summary>
/// トラップのどこに設置するか見るための描画
/// </summary>
void TrapManager::PreviewDraw()
{
	switch (m_nowSlotIdx)
	{
	case 0:
		//0の時はクロスボウを持っているため何も描画しない
		break;
	case 1:
		MV1SetPosition(m_modelHandles[modelIdx::SpikeFrame], m_previewPos.ConvertToVECTOR());
		MV1SetScale(m_modelHandles[modelIdx::SpikeFrame], VECTOR(0.08f, 0.08f, 0.08f));
		MV1SetOpacityRate(m_modelHandles[modelIdx::SpikeFrame], m_transparency);

		MV1DrawModel(m_modelHandles[modelIdx::SpikeFrame]);
		break;

	case 2:
		MV1SetPosition(m_modelHandles[modelIdx::Cutter], m_previewPos.ConvertToVECTOR());
		MV1SetScale(m_modelHandles[modelIdx::Cutter], VECTOR(0.08f, 0.08f, 0.08f));
		MV1SetOpacityRate(m_modelHandles[modelIdx::Cutter], m_transparency);

		MV1DrawModel(m_modelHandles[modelIdx::Cutter]);
		break;

	case 3:
		MV1SetPosition(m_modelHandles[modelIdx::Hammer], m_previewPos.ConvertToVECTOR());
		MV1SetScale(m_modelHandles[modelIdx::Hammer], VECTOR(10.0f, 10.0f, 10.0f));
		MV1SetOpacityRate(m_modelHandles[modelIdx::Hammer], m_transparency);

		MV1DrawModel(m_modelHandles[modelIdx::Hammer]);
		break;
	}
}

/// <summary>
/// トラップの生成
/// </summary>
/// <param name="slotNum">現在選択しているスロット</param>
/// <param name="physics">物理クラス</param>
/// <param name="trapPoint">罠ポイント</param>
/// <returns></returns>
bool TrapManager::CreateTrap(int slotNum, std::shared_ptr<MyLib::Physics> physics, int* trapPoint)
{

	if (slotNum == 1)
	{
		//設置コスト以上の罠ポイントがあるか確認する
		//足りていたら消費して罠を設置する
		if (*trapPoint >= CsvLoad::GetInstance().TrapStatusLoad("Spike").cost)
		{
			*trapPoint -= CsvLoad::GetInstance().TrapStatusLoad("Spike").cost;

			//罠を設置する
			std::shared_ptr<TrapBase> add;
			add = std::make_shared<SpikeTrap>(physics);
			add->SetPos(m_previewPos);
			add->Init();
			m_traps.emplace_back(add);

			//設置成功
			return true;
		}
		else
		{
			//ポイント不足で設置失敗
			return false;
		}

	}
	else if (slotNum == 2)
	{
		//設置コスト以上の罠ポイントがあるか確認する
		//足りていなかったら終わる
		if (*trapPoint >= CsvLoad::GetInstance().TrapStatusLoad("Cutter").cost)
		{
			//足りていたら消費する
			*trapPoint -= CsvLoad::GetInstance().TrapStatusLoad("Cutter").cost;

			//罠を設置する
			std::shared_ptr<TrapBase> add;
			add = std::make_shared<CutterTrap>(physics);
			add->SetPos(m_previewPos);
			add->Init();
			m_traps.emplace_back(add);

			//設置成功
			return true;
		}
		else
		{
			//ポイント不足で設置失敗
			return false;
		}
	}
	else if (slotNum == 3)
	{
		////設置コスト以上の罠ポイントがあるか確認する
		////足りていなかったら終わる
		//if (*trapPoint >= 200)
		//{
		//	//足りていたら消費する
		//	*trapPoint -= 200;

		//	//罠を設置する
		//	std::shared_ptr<TrapBase> add;
		//	add = std::make_shared<HammerTrap>(physics);
		//	add->SetPos(m_previewPos);
		//	add->Init(MV1DuplicateModel(m_modelHandles[Hammer]));
		//	m_traps.emplace_back(add);

		//	//設置成功
		//	return true;
		//}
		//else
		//{
		//	//ポイント不足で設置失敗
		//	return false;
		//}
	}

	return false;

}