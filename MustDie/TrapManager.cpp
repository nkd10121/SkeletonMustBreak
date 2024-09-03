#include "TrapManager.h"
#include "Input.h"
#include "TrapBase.h"
#include "SpikeTrap.h"

namespace
{
	enum modelIdx :int
	{
		SpikeFrame,
		Spike,
		Spring,

		MAX
	};

	//トラップファイル名
	const char* const kFileName[modelIdx::MAX] =
	{
		"data/model/spike_frame.mv1",
		"data/model/spike.mv1",
		"data/model/SpringTrap.mv1",
	};
}

TrapManager::TrapManager(int trapNum, std::list<MyLib::Vec3>& trapPoss):
	m_previewPos(),
	m_slotNum(0)
{
	for (int i = 0; i < trapNum; i++)
	{
		int handle = MV1LoadModel(kFileName[i]);
		m_modelHandles.emplace_back(handle);
	}

	m_trapPoss = trapPoss;
}

TrapManager::~TrapManager()
{
}

void TrapManager::Init()
{
}

void TrapManager::Update(std::shared_ptr<Input>& input, int slotNum,MyLib::Vec3 playerPos, MyLib::Vec3 playerVec, int* trapPoint, int nowPhase, std::shared_ptr<MyLib::Physics> physics)
{
	//現在のプレイヤーのスロットを取得
	m_slotNum = slotNum;

	float defaultLength = 100.0f;

	//0のときはクロスボウを持っているため何も処理しない
	if (m_slotNum != 0)
	{
		//線分の始点と終点を設定
		auto start = playerPos;
		auto end = playerPos + playerVec * 20;

		//設置可能なトラップの座標分回す
		for (auto pos : m_trapPoss)
		{
			////プレイヤーの中心座標から一定距離離れていたら計算しない
			//if ((pos - start).Size() > 50.0f)
			//{
			//	return;
			//}

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
		if (input->GetIsPushedZR())
		{
			//すでに設置された罠の座標を見て
			for (auto trap : m_traps)
			{
				//すでにその座標に罠が設置されていたら終わる
				if (trap->GetPos() == m_previewPos)
				{
					return;
				}
			}

			//ここまで来たら罠が設置されていない座標に罠を設置しようとしている

			//設置コスト以上の罠ポイントがあるか確認する
			if (slotNum == 1)
			{
				//足りていなかったら終わる
				if (*trapPoint < 600)
				{
					return;
				}

				*trapPoint -= 600;
			}


			std::shared_ptr<TrapBase> add;

			if (slotNum == 1)
			{
				add = std::make_shared<SpikeTrap>(physics);
				add->SetDrawPos(m_previewPos);
				add->Init(MV1DuplicateModel(m_modelHandles[0]), MV1DuplicateModel(m_modelHandles[1]));
			}

			m_traps.emplace_back(add);
		}
	}

	//すでに設置された罠の座標を見て
	for (auto trap : m_traps)
	{
		trap->Update();
	}
}

void TrapManager::Draw()
{
	//すでに設置された罠の座標を見て
	for (auto trap : m_traps)
	{
		trap->Draw();
	}
}

void TrapManager::PreviewDraw()
{
	switch (m_slotNum)
	{
	case 0:
		//0の時はクロスボウを持っているため何も描画しない
		break;
	case 1:
		MV1SetPosition(m_modelHandles[modelIdx::SpikeFrame], m_previewPos.ConvertToVECTOR());
		MV1SetScale(m_modelHandles[modelIdx::SpikeFrame], VECTOR(0.08f, 0.08f, 0.08f));
		MV1SetOpacityRate(m_modelHandles[modelIdx::SpikeFrame], 0.5f);

		MV1DrawModel(m_modelHandles[modelIdx::SpikeFrame]);
		break;

	case 2:

		break;

	case 3:

		break;
	}
}
