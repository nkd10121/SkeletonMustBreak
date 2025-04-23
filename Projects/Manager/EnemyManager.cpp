#include "EnemyManager.h"
#include "GameManager.h"
#include "DxLib.h"
#include "EnemyBase.h"
#include "Enemy.h"
#include "EnemyBig.h"
#include "EnemyFast.h"
#include "CsvLoad.h"
#include "MapDataLoad.h"

#include "SoundManager.h"
#include "EffectManager.h"
#include "Font.h"
#include "Game.h"

namespace
{
	//コンボ継続のためのタイムリミット
	constexpr int kComboTimeLimitMax = 6 * 60;
	//コンボの時間制限の減少量
	constexpr int kDelTimeLimit = 10;

	/*コンボバーの描画座標*/
	constexpr int kComboBarStartX = 160;	//左端
	constexpr int kComboBarEndX = 320;		//右端
	constexpr int kComboBarStartY = 400;	//上
	constexpr int kComboBarEndY = 420;		//下

	//テキストサイズ
	constexpr int kTextSize = 40;

	//敵の移動ルートの差分の最大
	constexpr int kRandMax = 8;

	//エフェクトのフレーム数
	constexpr int kEffectFrame = 120;

	constexpr float kComboTextSize = 0.05f;
}

/// <summary>
/// コンストラクタ
/// </summary>
EnemyManager::EnemyManager() :
	m_stageName(""),
	m_deletedEnemyNum(0),
	m_playerKillCount(nullptr),
	m_trapKillCount(nullptr),
	m_comboMax(nullptr),
	m_comboCount(0),
	m_combTimeLimit(kComboTimeLimitMax),
	m_fontMag(0.0f),
	m_frontEnemyHp(0),
	m_frontEnemyMaxHp(0)
{
	//サウンドのロード
	SoundManager::GetInstance().Load("EnemyHit", "data/sound/se/enemyHit.mp3", false);
	SoundManager::GetInstance().Load("EnemyCriticalHit", "data/sound/se/enemyHit2.mp3", false);

	//エフェクトのロード
	EffectManager::GetInstance().Load("EnemyHit", "data/effect/enemy_hit.efk", kEffectFrame);
	EffectManager::GetInstance().Load("EnemyCriticalHit", "data/effect/enemy_CriticalHit.efk", kEffectFrame);
	EffectManager::GetInstance().Load("EnemyDisappear", "data/effect/enemy_disappear.efk", kEffectFrame);
}

/// <summary>
/// デストラクタ
/// </summary>
EnemyManager::~EnemyManager()
{
	//メモリ開放
	m_playerKillCount = nullptr;
	delete m_playerKillCount;
	m_trapKillCount = nullptr;
	delete m_trapKillCount;

	//配列の全要素削除
	m_pEnemys.clear();
	m_pGenerateInfo.clear();
	m_enemyPos.clear();
}

/// <summary>
/// 初期化
/// </summary>
/// <param name="playerKillCount"></param>
/// <param name="trapKillCount"></param>
/// <param name="comboMax"></param>
/// <param name="stageName"></param>
void EnemyManager::Init(int* playerKillCount, int* trapKillCount, int* comboMax, const char* stageName)
{
	//スコア計算に必要な情報のアドレスを格納する
	m_stageName = stageName;
	m_playerKillCount = playerKillCount;
	m_trapKillCount = trapKillCount;
	m_comboMax = comboMax;

	//敵生成情報を取得する
	CsvLoad::GetInstance().StageEnenyDataLoad(m_stageName, m_pGenerateInfo);

	//csv側で1,2,3ウェーブと出現ウェーブを指定しているため、ウェーブ数の補正をする
	//ついでに各ウェーブの敵の生成数もカウントしておく
	for (auto& generate : m_pGenerateInfo)
	{
		//出現ウェーブが1なら
		if (generate->appearPhase == 1)
		{
			//補正はしなくていい

			//そのウェーブの出現する敵の数を更新する
			int num = m_enemyGenerationCountPerOnePhase[generate->appearPhase];
			num++;
			m_enemyGenerationCountPerOnePhase[generate->appearPhase] = num;
		}
		else if (generate->appearPhase == 2)
		{
			//補正する
			generate->appearPhase = 3;

			//そのウェーブの出現する敵の数を更新する
			int num = m_enemyGenerationCountPerOnePhase[generate->appearPhase];
			num++;
			m_enemyGenerationCountPerOnePhase[generate->appearPhase] = num;
		}
		else if (generate->appearPhase == 3)
		{
			//補正する
			generate->appearPhase = 5;

			//そのウェーブの出現する敵の数を更新する
			int num = m_enemyGenerationCountPerOnePhase[generate->appearPhase];
			num++;
			m_enemyGenerationCountPerOnePhase[generate->appearPhase] = num;
		}
	}

}

/// <summary>
/// 更新
/// </summary>
/// <param name="input"></param>
/// <param name="physics"></param>
/// <param name="gameManager"></param>
/// <param name="isPlayerChase"></param>
void EnemyManager::Update(std::shared_ptr<MyLib::Physics> physics, GameManager* gameManager, MyLib::Vec3 playerPos, MyLib::Vec3 playerDir, bool isPlayerChase)
{
	m_enemyPos.clear();

	//今のフェーズがどのフェーズか取得する
	auto thisPhaseName = gameManager->GetThisPhaseName();
	//今のフェーズのカウントを取得する
	auto thisPhaseCount = gameManager->GetThisPhaseCount() / Game::kFps;

	//もし今のフェーズが戦闘フェーズなら
	if (thisPhaseName == 1 || thisPhaseName == 3 || thisPhaseName == 5)
	{
		//敵生成情報をまわして
		for (auto& generate : m_pGenerateInfo)
		{
			//今のフェーズとフェーズカウントが一致しているとき
			if (generate->appearPhase == thisPhaseName && generate->appearFrame == thisPhaseCount)
			{
				//生成済みでなければ
				if (!generate->isCreated)
				{
					//生成済みにして敵を生成する
					generate->isCreated = true;
					CreateEnemy(generate->enemyName, physics);
				}

			}
		}
	}

	//レイに当たった敵を入れる配列
	std::list<std::shared_ptr<EnemyBase>> rayHitEnemys;

	//レイキャストのレイの線分を計算する
	VECTOR start = playerPos.ConvertToVECTOR();
	VECTOR end = VAdd(start,VScale(playerDir.ConvertToVECTOR(),100.0f));

#ifdef _DEBUG
	DrawLine3D(start, end, 0x00ffff);
#endif

	//敵の更新をする
	for (auto& e : m_pEnemys)
	{
		//更新
		e->Update(gameManager->GetPlayerPos(), isPlayerChase);

		m_enemyPos.emplace_back(e->GetCenterPos());

		//プレイヤーからプレイヤーの向いている方向にレイを飛ばして敵がそのレイに当たったかどうかを取得する
		auto colData = HitCheck_Line_Sphere(start, end,e->GetCenterPos().ConvertToVECTOR(),e->GetRadius());
		//当たっていたら
		if (colData)
		{
			//その敵を一旦RayHitEnemy的な配列につっこんで保留しておく
			//一通りの敵の更新が終わったらそのRayHitEnemyの中で最もプレイヤーと近い敵のHPを画面上部に表示する
			rayHitEnemys.emplace_back(e);
		}

		//死んでいる(ダウンアニメーションが始まっている)敵の判定をする
		if (e->GetIsDead())
		{
			//ポイントをまだドロップしていなかったら
			if (!e->GetIsDropedPoint())
			{
				//コンボ数を更新して
				m_comboCount++;
				//コンボの時間制限をリセットする
				m_combTimeLimit = kComboTimeLimitMax - kDelTimeLimit * m_comboCount;

				int point = e->GetDropPoint();
				gameManager->AddTrapPoint(static_cast<int>(point * (1.0f + static_cast<float>(m_comboCount) / 20)));


				//コンボ数のフォントの大きさを変える
				if (m_comboCount >= 1)
				{
					m_fontMag = 1.0f + (m_comboCount - 1) * kComboTextSize;
				}
			}
		}

		//使用済みの敵キャラクタを削除する必要がある
		if (!e->GetIsExist())
		{
			//敵が消えるエフェクトを表示する
			auto pos = e->GetCenterPos();
			EffectManager::GetInstance().CreateEffect("EnemyDisappear",pos);

			//死んでいる=何者かによってダメージを与えられている
			if (e->GetIsDead())
			{
				//ラストアタックをとったのがプレイヤーかどうか
				if (e->GetLastHitObjectTag() == GameObjectTag::Shot)
				{
					//ラストアタック手段がShotならプレイヤーがキルした数を増やす
					*m_playerKillCount += 1;
				}
				else	//プレイヤー以外の原因ならトラップで死亡したとみなす
				{
					//ラストアタック手段がShot以外ならトラップがキルした数を増やす
					*m_trapKillCount += 1;
				}

				//確率でポーションがドロップするように
				int rand = GetRand(10);
				if (rand == 0)
				{
					gameManager->CreatePortion(e->GetPos());
				}
			}

			//敵が目的地に到達しているか
			if (e->GetIsReach())
			{
				//到達していたらオブジェクトのHPを減らす
				gameManager->SubCrystalHP();
			}

			//削除済みの敵の数を更新する
			m_deletedEnemyNum++;

			//メモリを解放する
			//e->Finalize(physics);
			e.reset();
			e = nullptr;
		}
	}

	//不要になった敵をここで削除処理する
	auto lIt = remove_if(m_pEnemys.begin(), m_pEnemys.end(), [](auto& v) {
		return v == nullptr;
		});
	m_pEnemys.erase(lIt, m_pEnemys.end());


	float length = 300.0f;
	m_frontEnemyHp = 0;
	m_frontEnemyMaxHp = 0;
	//レイに当たった敵の中からプレイヤーに最も近い敵を計算する
	for (auto& rayHitEnemy : rayHitEnemys)
	{
		auto distance = (rayHitEnemy->GetCenterPos() - playerPos).Size();
		if (distance < length)
		{
			length = distance;
			m_frontEnemyHp = rayHitEnemy->GetHp();
			m_frontEnemyMaxHp = rayHitEnemy->GetMaxHp();
		}
	}


	//戦闘フェーズの時
	if (thisPhaseName < 6 && thisPhaseName % 2 != 0)
	{
		//あらかじめ設定されていた敵の生成数と削除数が同じなら
		if (m_enemyGenerationCountPerOnePhase[thisPhaseName] == m_deletedEnemyNum)
		{
			//フェーズを次のフェーズにする
			gameManager->ChangeNextPhase();
			m_deletedEnemyNum = 0;
		}
	}

	//コンボカウントが1以上の時、タイムリミットを減らす
	if (m_comboCount >= 1)
	{
		m_combTimeLimit--;

		//タイムリミットが0になったときコンボをリセットする
		if (m_combTimeLimit <= 0)
		{
			if (*m_comboMax < m_comboCount)
			{
				*m_comboMax = m_comboCount;
			}
			m_comboCount = 0;
		}
	}
}

/// <summary>
/// 描画
/// </summary>
void EnemyManager::Draw()
{
	//敵を描画する
	for (auto& e : m_pEnemys)
	{
		e->Draw();
	}

	//コンボが続いていたらコンボ関係を描画する
	if (m_comboCount >= 1)
	{
		float per = static_cast<float>(m_combTimeLimit) / static_cast<float>(kComboTimeLimitMax - kDelTimeLimit * m_comboCount);
		int x = kComboBarStartX + static_cast<int>((kComboBarEndX - kComboBarStartX) * per);

		DrawBox(kComboBarStartX, kComboBarStartY, kComboBarEndX, kComboBarEndY, 0xcccccc, true);
		DrawBox(kComboBarStartX, kComboBarStartY, x, kComboBarEndY, 0xff0000, true);
		DrawBox(kComboBarStartX, kComboBarStartY, kComboBarEndX, kComboBarEndY, 0xffffff, false);

		int offset = 0;
		if (m_comboCount / 10 != 0)
		{
			offset = kTextSize;
		}
		DrawExtendFormatStringToHandle(kComboBarStartX + (kComboBarEndX - kComboBarStartX) / 2 - kTextSize - offset, 340, m_fontMag, m_fontMag, 0xffffff, Font::GetInstance().GetFontHandle("data/font/Dela-Gothic-One.ttf", "Dela Gothic One", kTextSize), "%d", m_comboCount);
		DrawStringToHandle(kComboBarStartX + (kComboBarEndX - kComboBarStartX) / 2 + 20, 370, "コンボ", 0xffffff, Font::GetInstance().GetFontHandle("data/font/Dela-Gothic-One.ttf", "Dela Gothic One", kTextSize / 2), 0xff0000);
	}
#ifdef _DEBUG
	//DrawFormatString(1100, 400, 0xff0000, "%d", m_frontEnemyHp);
#endif
}

/// <summary>
/// 敵の移動ルートの作成
/// </summary>
/// <returns>移動ルート</returns>
std::vector<MyLib::Vec3> EnemyManager::CreateEnemyRoute()
{
	//最終的に返す値
	std::vector<MyLib::Vec3> ret;

	//すべての移動ルートの候補座標を取得する
	auto info = MapDataLoad::GetInstance().GetEnemyRoute();

	//生成位置を設定する
	//TODO:複数の生成位置に対応できるようにする
	auto startPos = info.pos[0];
	startPos[0].x += GetRand(kRandMax) - kRandMax/2;
	startPos[0].z += GetRand(kRandMax) - kRandMax / 2;
	ret.push_back(startPos[0]);

	//ステージ名をstring型で取得する
	std::string name = m_stageName;

	/*ステージごとにルートが分岐するためそれぞれのルートを生成する*/

	if (name == "stage1")
	{
		int add = 0;
		auto pos = info.pos;
		//座標の候補のどれにするかを乱数で決定
		int rand = GetRand(static_cast<int>(pos[1].size()) - 1);
		add = rand;

		auto addPos = pos[1][add] + MyLib::Vec3(static_cast<float>(GetRand(kRandMax) - kRandMax / 2), 0.0f, static_cast<float>(GetRand(kRandMax) - kRandMax / 2));
		ret.push_back(addPos);


		//ルートを決定する
		for (int i = 2; i < pos.size(); i++)
		{
			if (static_cast<int>(pos[i].size()) == 0)
			{
				continue;
			}

			rand = GetRand(static_cast<int>(pos[i].size()) - 1);
			add = rand;

			if (add < 0)
			{
				add = 0;
			}
			else if (add > static_cast<int>(pos[i].size()) - 1)
			{
				add = static_cast<int>(pos[i].size()) - 1;
			}

			addPos = pos[i][add] + MyLib::Vec3(static_cast<float>(GetRand(kRandMax) - kRandMax / 2), 0.0f, static_cast<float>(GetRand(kRandMax) - kRandMax / 2));
			ret.push_back(addPos);
		}
	}
	else if (name == "stage2")
	{
		int add = 0;
		auto pos = info.pos;
		//座標の候補のどれにするかを乱数で決定
		int rand = GetRand(static_cast<int>(pos[1].size()) - 1);
		add = rand;

		auto addPos = pos[1][add] + MyLib::Vec3(static_cast<float>(GetRand(kRandMax) - kRandMax / 2), 0.0f, static_cast<float>(GetRand(kRandMax) - kRandMax / 2));
		ret.push_back(addPos);
		addPos = pos[2][add] + MyLib::Vec3(static_cast<float>(GetRand(kRandMax) - kRandMax / 2), 0.0f, static_cast<float>(GetRand(kRandMax) - kRandMax / 2));
		ret.push_back(addPos);

		//最初の左右をどっちに行ったかでルートが分岐する
		if (add == 0)
		{
			//これからのルートは0か1
			//ルートを決定する
			for (int i = 3; i < pos.size(); i++)
			{
				if (static_cast<int>(pos[i].size()) == 0)
				{
					continue;
				}

				rand = GetRand(1);

				addPos = pos[i][rand] + MyLib::Vec3(static_cast<float>(GetRand(4) - 2), 0.0f, static_cast<float>(GetRand(4) - 2));
				ret.push_back(addPos);
			}
		}
		else if (add == 1)
		{
			//これからのルートは2か3
			//ルートを決定する
			for (int i = 3; i < pos.size(); i++)
			{
				if (static_cast<int>(pos[i].size()) == 0)
				{
					continue;
				}

				rand = GetRand(1) + 2;

				addPos = pos[i][rand] + MyLib::Vec3(static_cast<float>(GetRand(4) - 2), 0.0f, static_cast<float>(GetRand(4) - 2));
				ret.push_back(addPos);
			}
		}
	}
	else if (name == "stage3")
	{
		auto pos = info.pos;
		ret.push_back(pos[1][0]);
		ret.push_back(pos[2][0]);

		int sep = 0;

		for (int i = 0; i < 3; i++)
		{
			int rand = GetRand(static_cast<int>(pos[i + 3].size()) - 1);
			ret.push_back(pos[i + 3][rand]);

			if (i == 2)
			{
				sep = rand;
			}
		}

		//道が広い側
		if (sep == 0)
		{
			int rand = GetRand(1);
			ret.push_back(pos[6][rand]);

			rand = GetRand(1);
			ret.push_back(pos[7][rand]);
		}
		else if (sep == 1)
		{
			ret.push_back(pos[6][2]);

			int rand = GetRand(1);
			ret.push_back(pos[7][rand + 1]);
		}
	}


	//終点座標を入れる
	ret.push_back(info.end);

	return ret;
}

/// <summary>
/// 敵の生成
/// </summary>
/// <param name="name"></param>
/// <param name="physics"></param>
void EnemyManager::CreateEnemy(std::string name, std::shared_ptr<MyLib::Physics> physics)
{
	if (name == "NormalSkelton")
	{
		std::shared_ptr<Enemy>add = std::make_shared<Enemy>();
		add->Init(physics, CreateEnemyRoute());
		m_pEnemys.emplace_back(add);
	}
	else if (name == "BigSkelton")
	{
		std::shared_ptr<EnemyBig>add = std::make_shared<EnemyBig>();
		add->Init(physics, CreateEnemyRoute());
		m_pEnemys.emplace_back(add);
	}
	else if (name == "FastSkelton")
	{
		std::shared_ptr<EnemyFast>add = std::make_shared<EnemyFast>();
		add->Init(physics, CreateEnemyRoute());
		m_pEnemys.emplace_back(add);
	}
}