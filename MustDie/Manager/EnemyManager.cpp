#include "EnemyManager.h"
#include "GameManager.h"
#include "DxLib.h"
#include "EnemyBase.h"
#include "Enemy.h"
#include "EnemyBig.h"
#include "Input.h"
#include "CsvLoad.h"
#include "MapDataLoad.h"

EnemyManager::EnemyManager() :
	m_deathEnemyNum(0)
{
	temp_enemyHandle = MV1LoadModel("data/model/Skeleton_Minion.mv1");
	temp_weaponHandle = MV1LoadModel("data/model/SkeletonBlade.mv1");
}

EnemyManager::~EnemyManager()
{
	MV1DeleteModel(temp_enemyHandle);
	MV1DeleteModel(temp_weaponHandle);
}

void EnemyManager::Init()
{
	CsvLoad::GetInstance().StageEnenyDataLoad("stage", m_pGenerateInfo);

	for (auto& generate : m_pGenerateInfo)
	{
		if (generate->appearPhase == 1)
		{
			int num = m_enemyGenerationCountPerOnePhase[generate->appearPhase];
			num++;
			m_enemyGenerationCountPerOnePhase[generate->appearPhase] = num;
		}
		else if (generate->appearPhase == 2)
		{
			generate->appearPhase = 3;

			int num = m_enemyGenerationCountPerOnePhase[generate->appearPhase];
			num++;
			m_enemyGenerationCountPerOnePhase[generate->appearPhase] = num;
		}
		else if (generate->appearPhase == 3)
		{
			generate->appearPhase = 5;

			int num = m_enemyGenerationCountPerOnePhase[generate->appearPhase];
			num++;
			m_enemyGenerationCountPerOnePhase[generate->appearPhase] = num;
		}
	}
}

void EnemyManager::Update(std::shared_ptr<Input> input, std::shared_ptr<MyLib::Physics> physics, GameManager* gameManager, bool isPlayerChase)
{
	//今のフェーズがどのフェーズか取得する
	auto thisPhaseName = gameManager->GetThisPhaseName();
	//今のフェーズのカウントを取得する
	auto thisPhaseCount = gameManager->GetThisPhaseCount() / 60;

	//もし今のフェーズが戦闘フェーズなら
	if (thisPhaseName == 1 || thisPhaseName == 3 || thisPhaseName == 5)
	{
		//敵生成情報をまわして
		for (auto& generate : m_pGenerateInfo)
		{
			//今のフェーズとフェーズカウントが一致している生成情報があればその敵を生成する
			if (generate->appearPhase == thisPhaseName && generate->appearFrame == thisPhaseCount)
			{
				if (!generate->isCreated)
				{
					generate->isCreated = true;


					CreateEnemy(generate->enemyName, physics);
				}

			}
		}
	}


	for (auto& e : m_pEnemys)
	{
		e->Update(gameManager->GetPlayerPos(), isPlayerChase);

		//死んでいる(ダウンアニメーションが始まっている)敵の判定をする
		if (e->GetIsDead())
		{
			if (!e->GetIsDropedPoint())
			{
				gameManager->KilledEnemy(e->GetDropPoint());
			}

		}

		//使用済みの敵キャラクタを削除する必要がある
		if (!e->GetIsExist())
		{
			//敵が目的地に到達しているか
			if (e->GetIsReach())
			{
				//到達していたらオブジェクトのHPを減らす
				gameManager->SubObjectHP();
			}

			m_deathEnemyNum++;

			//メモリを解放する
			//e->Finalize(physics);
			e.reset();
			e = nullptr;
		}
	}

	if (thisPhaseName < 6 && thisPhaseName % 2 != 0)
	{
		if (m_enemyGenerationCountPerOnePhase[thisPhaseName] == m_deathEnemyNum)
		{
			gameManager->ChangeNextPhase();
			m_deathEnemyNum = 0;
		}
	}


	//不要になった敵をここで削除処理する
	auto lIt = remove_if(m_pEnemys.begin(), m_pEnemys.end(), [](auto& v) {
		return v == nullptr;
		});
	m_pEnemys.erase(lIt, m_pEnemys.end());
}

void EnemyManager::Draw()
{
	for (auto& e : m_pEnemys)
	{
		e->Draw();
	}
}

std::vector<MyLib::Vec3> EnemyManager::CreateEnemyRoute(const char* stageName)
{
	std::vector<MyLib::Vec3> ret;

	auto info = MapDataLoad::GetInstance().GetEnemyRoute();

	ret.push_back(info.start);

	int add = 0;

	int rand = GetRand(info.pos1.size() - 1);
	add = rand;
	ret.push_back(info.pos1[add]);

	int rand2 = GetRand(2) - 1;
	add += rand2;
	if (add < 0)
	{
		add = 0;
	}
	if (add > info.pos2.size() - 1)
	{
		add = info.pos2.size() - 1;
	}

	ret.push_back(info.pos2[add]);


	if (stageName == "stage1")
	{
		ret.push_back(info.end);
		return ret;
	}

	return ret;
}

void EnemyManager::CreateEnemy(std::string name, std::shared_ptr<MyLib::Physics> physics)
{
	if (name == "NormalSkelton")
	{
		std::shared_ptr<Enemy>add = std::make_shared<Enemy>(MV1DuplicateModel(temp_enemyHandle), MV1DuplicateModel(temp_weaponHandle));
		add->Init(physics, CreateEnemyRoute("stage1"));
		m_pEnemys.emplace_back(add);
	}
	else if (name == "BigSkelton")
	{
		std::shared_ptr<EnemyBig>add = std::make_shared<EnemyBig>(MV1DuplicateModel(temp_enemyHandle), MV1DuplicateModel(temp_weaponHandle));
		add->Init(physics, CreateEnemyRoute("stage1"));
		m_pEnemys.emplace_back(add);
	}

}
