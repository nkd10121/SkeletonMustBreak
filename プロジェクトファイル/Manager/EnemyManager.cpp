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
	//�R���{�p���̂��߂̃^�C�����~�b�g
	constexpr int kComboTimeLimitMax = 6 * 60;
	//�R���{�̎��Ԑ����̌�����
	constexpr int kDelTimeLimit = 10;

	/*�R���{�o�[�̕`����W*/
	constexpr int kComboBarStartX = 160;	//���[
	constexpr int kComboBarEndX = 320;		//�E�[
	constexpr int kComboBarStartY = 400;	//��
	constexpr int kComboBarEndY = 420;		//��

	//�e�L�X�g�T�C�Y
	constexpr int kTextSize = 40;

	//�G�̈ړ����[�g�̍����̍ő�
	constexpr int kRandMax = 8;

	//�G�t�F�N�g�̃t���[����
	constexpr int kEffectFrame = 120;
	//�q�b�g�G�t�F�N�g�̃I�t�Z�b�g
	constexpr int kEffectOffsetRand = 6;

	constexpr float kComboTextSize = 0.05f;
}

/// <summary>
/// �R���X�g���N�^
/// </summary>
EnemyManager::EnemyManager() :
	m_stageName(""),
	m_swordHandle(-1),
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
	//�G�l�~�[�̃��f�������炩���ߐ������Ă���
	//TODO:�X�e�[�W�̐�����񂩂�A�����\��̂��郂�f���݂̂����[�h����`�ɂ�����
	m_enemyHandle["Skeleton_Minion"] = MV1LoadModel("data/model/Skeleton_Minion.mv1");
	m_enemyHandle["SkeletonWarrior"] = MV1LoadModel("data/model/SkeletonWarrior.mv1");
	m_swordHandle = MV1LoadModel("data/model/SkeletonBlade.mv1");

	//�T�E���h�̃��[�h
	SoundManager::GetInstance().Load("EnemyHit", "data/sound/se/enemyHit.mp3", false);
	SoundManager::GetInstance().Load("EnemyCriticalHit", "data/sound/se/enemyHit2.mp3", false);

	//�G�t�F�N�g�̃��[�h
	EffectManager::GetInstance().Load("EnemyHit", "data/effect/enemy_hit.efk", kEffectFrame);
	EffectManager::GetInstance().Load("EnemyDisappear", "data/effect/enemy_disappear.efk", kEffectFrame);
}

/// <summary>
/// �f�X�g���N�^
/// </summary>
EnemyManager::~EnemyManager()
{
	for (auto& handle : m_enemyHandle)
	{
		MV1DeleteModel(handle.second);
	}
	m_enemyHandle.clear();

	MV1DeleteModel(m_swordHandle);

	m_playerKillCount = nullptr;
	delete m_playerKillCount;
	m_trapKillCount = nullptr;
	delete m_trapKillCount;

	m_pEnemys.clear();
	m_pGenerateInfo.clear();
	m_enemyPos.clear();

}

/// <summary>
/// ������
/// </summary>
/// <param name="playerKillCount"></param>
/// <param name="trapKillCount"></param>
/// <param name="comboMax"></param>
/// <param name="stageName"></param>
void EnemyManager::Init(int* playerKillCount, int* trapKillCount, int* comboMax, const char* stageName)
{
	//�X�R�A�v�Z�ɕK�v�ȏ��̃A�h���X���i�[����
	m_stageName = stageName;
	m_playerKillCount = playerKillCount;
	m_trapKillCount = trapKillCount;
	m_comboMax = comboMax;

	//�G���������擾����
	CsvLoad::GetInstance().StageEnenyDataLoad(m_stageName, m_pGenerateInfo);

	//csv����1,2,3�E�F�[�u�Əo���E�F�[�u���w�肵�Ă��邽�߁A�E�F�[�u���̕␳������
	for (auto& generate : m_pGenerateInfo)
	{
		//�o���E�F�[�u��1�Ȃ�
		if (generate->appearPhase == 1)
		{
			//�␳�͂��Ȃ��Ă���

			//���̃E�F�[�u�̏o������G�̐����X�V����
			int num = m_enemyGenerationCountPerOnePhase[generate->appearPhase];
			num++;
			m_enemyGenerationCountPerOnePhase[generate->appearPhase] = num;
		}
		else if (generate->appearPhase == 2)
		{
			//�␳����
			generate->appearPhase = 3;

			//���̃E�F�[�u�̏o������G�̐����X�V����
			int num = m_enemyGenerationCountPerOnePhase[generate->appearPhase];
			num++;
			m_enemyGenerationCountPerOnePhase[generate->appearPhase] = num;
		}
		else if (generate->appearPhase == 3)
		{
			//�␳����
			generate->appearPhase = 5;

			//���̃E�F�[�u�̏o������G�̐����X�V����
			int num = m_enemyGenerationCountPerOnePhase[generate->appearPhase];
			num++;
			m_enemyGenerationCountPerOnePhase[generate->appearPhase] = num;
		}
	}

}

/// <summary>
/// �X�V
/// </summary>
/// <param name="input"></param>
/// <param name="physics"></param>
/// <param name="gameManager"></param>
/// <param name="isPlayerChase"></param>
void EnemyManager::Update(std::shared_ptr<MyLib::Physics> physics, GameManager* gameManager, MyLib::Vec3 playerPos, MyLib::Vec3 playerDir, bool isPlayerChase)
{
	m_enemyPos.clear();

	//���̃t�F�[�Y���ǂ̃t�F�[�Y���擾����
	auto thisPhaseName = gameManager->GetThisPhaseName();
	//���̃t�F�[�Y�̃J�E���g���擾����
	auto thisPhaseCount = gameManager->GetThisPhaseCount() / Game::kFps;

	//�������̃t�F�[�Y���퓬�t�F�[�Y�Ȃ�
	if (thisPhaseName == 1 || thisPhaseName == 3 || thisPhaseName == 5)
	{
		//�G���������܂킵��
		for (auto& generate : m_pGenerateInfo)
		{
			//���̃t�F�[�Y�ƃt�F�[�Y�J�E���g����v���Ă���Ƃ�
			if (generate->appearPhase == thisPhaseName && generate->appearFrame == thisPhaseCount)
			{
				//�����ς݂łȂ����
				if (!generate->isCreated)
				{
					//�����ς݂ɂ��ēG�𐶐�����
					generate->isCreated = true;
					CreateEnemy(generate->enemyName, physics);
				}

			}
		}
	}

	//���C�ɓ��������G������z��
	std::list<std::shared_ptr<EnemyBase>> rayHitEnemys;

	//���C�L���X�g�̃��C�̐������v�Z����
	VECTOR start = playerPos.ConvertToVECTOR();
	VECTOR end = VAdd(start,VScale(playerDir.ConvertToVECTOR(),100.0f));

#ifdef _DEBUG
	DrawLine3D(start, end, 0x00ffff);
#endif

	//�G�̍X�V������
	for (auto& e : m_pEnemys)
	{
		//�X�V
		e->Update(gameManager->GetPlayerPos(), isPlayerChase);

		m_enemyPos.emplace_back(e->GetCenterPos());

		//�R���W���������X�V����
		MV1RefreshCollInfo(e->GetModelHandle());

		//�v���C���[����v���C���[�̌����Ă�������Ƀ��C���΂��ēG�����̃��C�ɓ����������ǂ������擾����
		auto colData = MV1CollCheck_Line(e->GetModelHandle(), -1, start, end);
		//�������Ă�����
		if (colData.HitFlag)
		{
			//���̓G����URayHitEnemy�I�Ȕz��ɂ�����ŕۗ����Ă���
			//��ʂ�̓G�̍X�V���I������炻��RayHitEnemy�̒��ōł��v���C���[�Ƌ߂��G��HP����ʏ㕔�ɕ\������
			rayHitEnemys.emplace_back(e);
		}

		//�G���U���ɓ���������
		if (e->GetIsHit())
		{
			//�G�̋߂��Ƀq�b�g�G�t�F�N�g��\��
			auto pos = e->GetCenterPos();
			MyLib::Vec3 offset = MyLib::Vec3(static_cast<float>(GetRand(kEffectOffsetRand) - kEffectOffsetRand/2), static_cast<float>(GetRand(kEffectOffsetRand) - kEffectOffsetRand/2), static_cast<float>(GetRand(kEffectOffsetRand) - kEffectOffsetRand/2));
			pos += offset;

			EffectManager::GetInstance().CreateEffect("EnemyHit", pos,pos);
		}

		//����ł���(�_�E���A�j���[�V�������n�܂��Ă���)�G�̔��������
		if (e->GetIsDead())
		{
			//�|�C���g���܂��h���b�v���Ă��Ȃ�������
			if (!e->GetIsDropedPoint())
			{
				//�R���{�����X�V����
				m_comboCount++;
				//�R���{�̎��Ԑ��������Z�b�g����
				m_combTimeLimit = kComboTimeLimitMax - kDelTimeLimit * m_comboCount;

				int point = e->GetDropPoint();
				gameManager->AddTrapPoint(static_cast<int>(point * (1.0f + static_cast<float>(m_comboCount) / 20)));


				//�R���{���̃t�H���g�̑傫����ς���
				if (m_comboCount >= 1)
				{
					m_fontMag = 1.0f + (m_comboCount - 1) * kComboTextSize;
				}
			}
		}

		//�g�p�ς݂̓G�L�����N�^���폜����K�v������
		if (!e->GetIsExist())
		{
			//�G��������G�t�F�N�g��\������
			auto pos = e->GetCenterPos();
			EffectManager::GetInstance().CreateEffect("EnemyDisappear",pos);

			//����ł���=���҂��ɂ���ă_���[�W��^�����Ă���
			if (e->GetIsDead())
			{
				//���X�g�A�^�b�N���Ƃ����̂��v���C���[���ǂ���
				if (e->GetLastHitObjectTag() == GameObjectTag::Shot)
				{
					//���X�g�A�^�b�N��i��Shot�Ȃ�v���C���[���L���������𑝂₷
					*m_playerKillCount += 1;
				}
				else	//�v���C���[�ȊO�̌����Ȃ�g���b�v�Ŏ��S�����Ƃ݂Ȃ�
				{
					//���X�g�A�^�b�N��i��Shot�ȊO�Ȃ�g���b�v���L���������𑝂₷
					*m_trapKillCount += 1;
				}

				//�m���Ń|�[�V�������h���b�v����悤��
				int rand = GetRand(10);
				if (rand == 0)
				{
					gameManager->CreatePortion(e->GetPos());
				}
			}

			//�G���ړI�n�ɓ��B���Ă��邩
			if (e->GetIsReach())
			{
				//���B���Ă�����I�u�W�F�N�g��HP�����炷
				gameManager->SubCrystalHP();
			}

			//�폜�ς݂̓G�̐����X�V����
			m_deletedEnemyNum++;

			//���������������
			//e->Finalize(physics);
			e.reset();
			e = nullptr;
		}
	}

	//�s�v�ɂȂ����G�������ō폜��������
	auto lIt = remove_if(m_pEnemys.begin(), m_pEnemys.end(), [](auto& v) {
		return v == nullptr;
		});
	m_pEnemys.erase(lIt, m_pEnemys.end());


	float length = 300.0f;
	m_frontEnemyHp = 0;
	m_frontEnemyMaxHp = 0;
	//���C�ɓ��������G�̒�����v���C���[�ɍł��߂��G���v�Z����
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


	//�퓬�t�F�[�Y�̎�
	if (thisPhaseName < 6 && thisPhaseName % 2 != 0)
	{
		//���炩���ߐݒ肳��Ă����G�̐������ƍ폜���������Ȃ�
		if (m_enemyGenerationCountPerOnePhase[thisPhaseName] == m_deletedEnemyNum)
		{
			//�t�F�[�Y�����̃t�F�[�Y�ɂ���
			gameManager->ChangeNextPhase();
			m_deletedEnemyNum = 0;
		}
	}

	//�R���{�J�E���g��1�ȏ�̎��A�^�C�����~�b�g�����炷
	if (m_comboCount >= 1)
	{
		m_combTimeLimit--;

		//�^�C�����~�b�g��0�ɂȂ����Ƃ��R���{�����Z�b�g����
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
/// �`��
/// </summary>
void EnemyManager::Draw()
{
	//�G��`�悷��
	for (auto& e : m_pEnemys)
	{
		e->Draw();
	}

	//�R���{�������Ă�����R���{�֌W��`�悷��
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
		DrawStringToHandle(kComboBarStartX + (kComboBarEndX - kComboBarStartX) / 2 + 20, 370, "�R���{", 0xffffff, Font::GetInstance().GetFontHandle("data/font/Dela-Gothic-One.ttf", "Dela Gothic One", kTextSize / 2), 0xff0000);
	}
#ifdef _DEBUG
	//DrawFormatString(1100, 400, 0xff0000, "%d", m_frontEnemyHp);
#endif
}

/// <summary>
/// �G�̈ړ����[�g�̍쐬
/// </summary>
/// <returns>�ړ����[�g</returns>
std::vector<MyLib::Vec3> EnemyManager::CreateEnemyRoute()
{
	//�ŏI�I�ɕԂ��l
	std::vector<MyLib::Vec3> ret;

	//���ׂĂ̈ړ����[�g�̌����W���擾����
	auto info = MapDataLoad::GetInstance().GetEnemyRoute();

	//�����ʒu��ݒ肷��
	//TODO:�����̐����ʒu�ɑΉ��ł���悤�ɂ���
	auto startPos = info.pos[0];
	startPos[0].x += GetRand(kRandMax) - kRandMax/2;
	startPos[0].z += GetRand(kRandMax) - kRandMax / 2;
	ret.push_back(startPos[0]);

	//�X�e�[�W����string�^�Ŏ擾����
	std::string name = m_stageName;

	/*�X�e�[�W���ƂɃ��[�g�����򂷂邽�߂��ꂼ��̃��[�g�𐶐�����*/

	if (name == "stage1")
	{
		int add = 0;
		auto pos = info.pos;
		//���W�̌��̂ǂ�ɂ��邩�𗐐��Ō���
		int rand = GetRand(static_cast<int>(pos[1].size()) - 1);
		add = rand;

		auto addPos = pos[1][add] + MyLib::Vec3(static_cast<float>(GetRand(kRandMax) - kRandMax / 2), 0.0f, static_cast<float>(GetRand(kRandMax) - kRandMax / 2));
		ret.push_back(addPos);


		//���[�g�����肷��
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
		//���W�̌��̂ǂ�ɂ��邩�𗐐��Ō���
		int rand = GetRand(static_cast<int>(pos[1].size()) - 1);
		add = rand;

		auto addPos = pos[1][add] + MyLib::Vec3(static_cast<float>(GetRand(kRandMax) - kRandMax / 2), 0.0f, static_cast<float>(GetRand(kRandMax) - kRandMax / 2));
		ret.push_back(addPos);
		addPos = pos[2][add] + MyLib::Vec3(static_cast<float>(GetRand(kRandMax) - kRandMax / 2), 0.0f, static_cast<float>(GetRand(kRandMax) - kRandMax / 2));
		ret.push_back(addPos);

		//�ŏ��̍��E���ǂ����ɍs�������Ń��[�g�����򂷂�
		if (add == 0)
		{
			//���ꂩ��̃��[�g��0��1
			//���[�g�����肷��
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
			//���ꂩ��̃��[�g��2��3
			//���[�g�����肷��
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

		//�����L����
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


	//�I�_���W������
	ret.push_back(info.end);

	return ret;
}

/// <summary>
/// �G�̐���
/// </summary>
/// <param name="name"></param>
/// <param name="physics"></param>
void EnemyManager::CreateEnemy(std::string name, std::shared_ptr<MyLib::Physics> physics)
{
	if (name == "NormalSkelton")
	{
		int handle = MV1DuplicateModel(m_enemyHandle["Skeleton_Minion"]);
		MV1SetupCollInfo(handle);
		std::shared_ptr<Enemy>add = std::make_shared<Enemy>(handle, MV1DuplicateModel(m_swordHandle));
		add->Init(physics, CreateEnemyRoute());
		m_pEnemys.emplace_back(add);
	}
	else if (name == "BigSkelton")
	{
		int handle = MV1DuplicateModel(m_enemyHandle["SkeletonWarrior"]);
		MV1SetupCollInfo(handle);
		std::shared_ptr<EnemyBig>add = std::make_shared<EnemyBig>(handle, MV1DuplicateModel(m_swordHandle));
		add->Init(physics, CreateEnemyRoute());
		m_pEnemys.emplace_back(add);
	}
	else if (name == "FastSkelton")
	{
		int handle = MV1DuplicateModel(m_enemyHandle["Skeleton_Minion"]);
		MV1SetupCollInfo(handle);
		std::shared_ptr<EnemyFast>add = std::make_shared<EnemyFast>(handle);
		add->Init(physics, CreateEnemyRoute());
		m_pEnemys.emplace_back(add);
	}
}