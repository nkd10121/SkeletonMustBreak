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
	//���̃t�F�[�Y���ǂ̃t�F�[�Y���擾����
	auto thisPhaseName = gameManager->GetThisPhaseName();
	//���̃t�F�[�Y�̃J�E���g���擾����
	auto thisPhaseCount = gameManager->GetThisPhaseCount() / 60;

	//�������̃t�F�[�Y���퓬�t�F�[�Y�Ȃ�
	if (thisPhaseName == 1 || thisPhaseName == 3 || thisPhaseName == 5)
	{
		//�G���������܂킵��
		for (auto& generate : m_pGenerateInfo)
		{
			//���̃t�F�[�Y�ƃt�F�[�Y�J�E���g����v���Ă��鐶����񂪂���΂��̓G�𐶐�����
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

		//����ł���(�_�E���A�j���[�V�������n�܂��Ă���)�G�̔��������
		if (e->GetIsDead())
		{
			if (!e->GetIsDropedPoint())
			{
				gameManager->KilledEnemy(e->GetDropPoint());
			}

		}

		//�g�p�ς݂̓G�L�����N�^���폜����K�v������
		if (!e->GetIsExist())
		{
			//�G���ړI�n�ɓ��B���Ă��邩
			if (e->GetIsReach())
			{
				//���B���Ă�����I�u�W�F�N�g��HP�����炷
				gameManager->SubObjectHP();
			}

			m_deathEnemyNum++;

			//���������������
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


	//�s�v�ɂȂ����G�������ō폜��������
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
