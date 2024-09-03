#include "GameManager.h"
#include "EnemyManager.h"
#include "Camera.h"
#include "Player.h"
#include "TrapGrid.h"
#include "TrapManager.h"

#include "Input.h"
#include "SceneGame.h"
#include "MapDataLoad.h"
#include "Font.h"

namespace
{
	//�I�u�W�F�N�g�̍ő�HP
	constexpr int kObjectHPMax = 10;
	
	constexpr float kHpBarScale = 0.225f;

	//�t�H���g�̃p�X
	const char* kFontPath = "data/font/Dela-Gothic-One.ttf";
}

GameManager::GameManager():
	m_objectHP(kObjectHPMax),
	m_nowPhaseName(ePhaseName::FirstPrePhase),
	m_phaseCount(0),
	m_allCount(0),
	m_playerDownCount(0),
	m_isThisSceneEnd(false),
	m_trapPoint(1000),
	m_playerSlotNum(0),
	m_hpBarWidth(0),
	m_hpBarHeight(0),
	m_playerHp(0),
	m_playerMaxHp(80)
{
	MapDataLoad::GetInstance().Load("stage1");

	m_pPhysics = std::make_shared<MyLib::Physics>(MapDataLoad::GetInstance().GetStageCollisionHandle());

	m_arrowHandle = MV1LoadModel("data/model/arrow.mv1");

	m_pPlayer = std::make_shared<Player>();
	m_pPlayer->Init(m_pPhysics, &m_arrowHandle);
	m_pPlayer->SetSlotNumMax(1);

	m_pEnemyManager = std::make_shared<EnemyManager>();
	m_pEnemyManager->Init();

	m_pCamera = std::make_shared<Camera>();
	m_pCamera->Init();

	m_pTrapGrid = std::make_shared<TrapGrid>();
	m_pTrapGrid->Init();

	std::list<MyLib::Vec3> pos = MapDataLoad::GetInstance().GetTrapPos();
	//pos.emplace_back(MyLib::Vec3(12.0f,0.0f,12.0f));
	//pos.emplace_back(MyLib::Vec3(-12.0f,0.0f,12.0f));
	//pos.emplace_back(MyLib::Vec3(12.0f,0.0f,-12.0f));
	//pos.emplace_back(MyLib::Vec3(-12.0f,0.0f,-12.0f));

	m_pTrapManager = std::make_shared<TrapManager>(2, pos);
	m_pTrapManager->Init();

	m_objectUIBgHandle = LoadGraph("data/img/ui_button.png");

	//�A�C�e���X���b�g�̉摜�����[�h
	m_slotHandle.push_back(LoadGraph("data/img/ui_slot_crossbow.png"));
	m_slotHandle.push_back(LoadGraph("data/img/ui_slot_spikeTrap.png"));

	//HP�o�[�̉摜�����[�h
	m_hpBarHandle.push_back(LoadGraph("data/img/ui_hpBar_base.png"));
	m_hpBarHandle.push_back(LoadGraph("data/img/ui_hpBar_bar.png"));
	m_hpBarHandle.push_back(LoadGraph("data/img/ui_hpBar_cover.png"));

	//HP�o�[�̉摜�T�C�Y���擾����
	GetGraphSize(m_hpBarHandle[1],&m_hpBarWidth, &m_hpBarHeight);

	m_fontHandle = Font::GetInstance().GetFontHandle(kFontPath, "Dela Gothic One", 24);;
}

GameManager::~GameManager()
{
	MV1DeleteModel(m_arrowHandle);
}

void GameManager::Init()
{

}

void GameManager::Update(std::shared_ptr<Input>& input, SceneGame* pSceneGame)
{

	if (m_nowPhaseName == ePhaseName::GameClear)
	{
		pSceneGame->ToResultScene(true);
		return;
	}
	else if (m_nowPhaseName == ePhaseName::Gameover)
	{
		pSceneGame->ToResultScene(false);
		return;
	}
	
	m_pEnemyManager->Update(input, m_pPhysics,this,m_pPlayer->IsGetPlayerDown());


	m_pCamera->SetPlayerPos(m_pPlayer->GetPos());
	m_pCamera->Update(input, MapDataLoad::GetInstance().GetStageCollisionHandle());

	m_pPlayer->SetCameraAngle(m_pCamera->GetDirection());
	m_pPlayer->Update(input);

	m_pTrapManager->Update(input,m_pPlayer->GetNowSlotNum(), m_pPlayer->GetPos(), m_pCamera->GetDirection(), &m_trapPoint, m_nowPhaseName, m_pPhysics);

	// �����X�V
	m_pPhysics->Update();

	//�v���C���[�̏����擾
	m_playerSlotNum = m_pPlayer->GetNowSlotNum();	//���ݑI�����Ă���X���b�g�̔ԍ�
	m_playerHp = m_pPlayer->GetHp();				//���݂̃v���C���[��HP

	//�v���C���[�̍ő�Hp�̒l���擾
	if (m_playerMaxHp == 0)
	{
		m_playerMaxHp = m_playerHp;
	}

	if (m_pPlayer->IsGetPlayerDown())
	{
		m_playerDownCount++;

		//if (m_objectHP > 5)
		{
			if (m_playerDownCount >= 100)
			{
				pSceneGame->StartFadeOut();
			}
		}

		//�v���C���[�����񂾂Ƃ�
		if (m_pPlayer->IsGetPlayerDead())
		{
			if (m_objectHP <= 5)
			{
				//�����ł��Ȃ��󋵂�������Q�[���I�[�o�[��
				m_nowPhaseName = ePhaseName::Gameover;
			}

			//�I�u�W�F�N�gHP��5�ȏ゠���5����ăv���C���[�𕜊�������
			if (m_objectHP > 5)
			{
				m_objectHP -= 5;


				m_pPlayer = nullptr;

				m_pPlayer = std::make_shared<Player>();
				m_pPlayer->Init(m_pPhysics, &m_arrowHandle);
			}


		}
	}
	else
	{
		m_playerDownCount = 0;
	}

	//�I�u�W�F�N�gHP��0�ȉ��ɂȂ�����Q�[���I�[�o�[�ɂ���
	if (m_objectHP <= 0)
	{
		m_nowPhaseName = ePhaseName::Gameover;
	}



	//���݂̃t�F�[�Y�̃J�E���g��i�߂�
	m_phaseCount++;

	//�ŏ���OK�{�^������������ŏ��̏����t�F�[�Y���I������
	if (input->IsTriggered("Y"))
	{
		//�{���͍ŏ��̃t�F�[�Y�̂݃v���C���[�̔C�ӂ̃^�C�~���O�Ŏ��̃t�F�[�Y�ɐi�s������
		//TODO:�t�F�[�Y���ƂɓG�𐶐����āA���̃t�F�[�Y�̓G��S���|�����玟�̃t�F�[�Y�ɐi�s���鏈�������
		//		�������炱�̕����̏�����߂�
		//if (m_nowPhaseName == ePhaseName::FirstBattlePhase)
		//if (m_nowPhaseName != ePhaseName::SecondPrePhase && m_nowPhaseName != ePhaseName::ThirdPrePhase)
		{
			ChangeNextPhase();
		}
	}

	//�����t�F�[�Y�ɓ�������10�b��ɐ퓬�t�F�[�Y�ɓ���悤�ɂ���
	if (m_nowPhaseName == ePhaseName::SecondPrePhase || m_nowPhaseName == ePhaseName::ThirdPrePhase)
	{
		if (m_phaseCount >= 600)
		{
			ChangeNextPhase();
		}
	}


}

void GameManager::Draw()
{
	MapDataLoad::GetInstance().Draw();

	m_pTrapManager->PreviewDraw();
	m_pTrapManager->Draw();

	m_pPlayer->Draw();
	m_pEnemyManager->Draw();

	DrawCircle(640, 390, m_pPlayer->GetDifAngle(), 0xffffff, false);

	//�X���b�g�A�C�e���̃J�[�\���`��
	int center = 320 + m_playerSlotNum * 80;
	DrawBox(center + 34, 660 + 34, center - 34, 660 - 34, 0x0000aa, false);

	//�X���b�g�A�C�e���̕`��
	for (int i = 0; i < m_slotHandle.size(); i++)
	{
		DrawRotaGraph(320 + i*80, 660, 1.0f, 0.0f, m_slotHandle[i], true);
	}

	//HP�o�[�̌v�Z
	//�v���C���[��HP���v���C���[�̍ő�HP�Ō��݂�HP�̊������o��
	float widthScale = static_cast<float>(m_playerHp) / static_cast<float>(m_playerMaxHp);
	//HP�̊������̉摜��X�����o��
	int width = m_hpBarWidth * widthScale;
	//�������Ȃ��Ɖ摜�����S�ɂ���Ă������߉摜�̒��S���W��HP�ɉ����Ă��炷
	int posX = 240 - (m_hpBarWidth - width) * kHpBarScale / 2;

	//Hp�o�[�̕`��
	DrawRotaGraph(220, 30, kHpBarScale, 0.0f, m_hpBarHandle[0], true);
	DrawRectRotaGraph(posX, 30, 0,0, width,m_hpBarHeight, kHpBarScale,0.0f, m_hpBarHandle[1], true);
	DrawRotaGraph(236, 25, kHpBarScale, 0.0f, m_hpBarHandle[2], true);

	//�I�u�W�F�N�g�̎c��HP�̕`��
	DrawRotaGraph(1200, 40, 0.28f, 0.0f, m_objectUIBgHandle, true);
	//������̌������ς��ƒ��S������邽�ߕ␳
	auto offset = ((1 - (m_objectHP / 10)) * 8);
	DrawFormatStringToHandle(1182 + offset, 27, 0xffffff, m_fontHandle, "%d", m_objectHP);
	//㩃|�C���g�̕`��
	DrawFormatStringToHandle(80, 680,0xffffff, m_fontHandle,"%d",m_trapPoint);

#ifdef _DEBUG
	DebugDraw();

	//���W���j�ɂ��邽�߂���
	m_pCamera->DebugDraw();
	DrawFormatString(1000, 0, 0xff0000, "�I�u�W�F�N�gHP:%d", m_objectHP);
	DrawFormatString(1000, 16, 0xff0000, "㩃|�C���g:%d", m_trapPoint);
	//DrawFormatString(1000, 16, 0xff0000, "�v���C���[�_�E���J�E���g:%d", m_playerDownCount);

#endif
}

void GameManager::DebugDraw()
{
	if(m_nowPhaseName == ePhaseName::FirstPrePhase)
	{
		DrawFormatString(560, 10, 0xff0000, "��1�����t�F�[�Y");
	} 
	else if(m_nowPhaseName == ePhaseName::FirstBattlePhase)
	{
		DrawFormatString(560, 10, 0xff0000, "��1�퓬�t�F�[�Y");
	}
	else if(m_nowPhaseName == ePhaseName::SecondPrePhase)
	{
		DrawFormatString(560, 10, 0xff0000, "��2�����t�F�[�Y");
	}
	else if(m_nowPhaseName == ePhaseName::SecondBattlePhase)
	{
		DrawFormatString(560, 10, 0xff0000, "��2�퓬�t�F�[�Y");
	}
	else if(m_nowPhaseName == ePhaseName::ThirdPrePhase)
	{
		DrawFormatString(560, 10, 0xff0000, "��3�����t�F�[�Y");
	}
	else if(m_nowPhaseName == ePhaseName::ThirdBattlePhase)
	{
		DrawFormatString(560, 10, 0xff0000, "��3�퓬�t�F�[�Y");
	}
	else if(m_nowPhaseName == ePhaseName::GameClear)
	{
		DrawFormatString(560, 10, 0xff0000, "�Q�[���N���A");
	}
	else if(m_nowPhaseName == ePhaseName::Gameover)
	{
		DrawFormatString(560, 10, 0xff0000, "�Q�[���I�[�o�[");
	}

	DrawFormatString(560, 30, 0xff0000, "���݂̃t�F�[�Y�̃^�C�� : %d",m_phaseCount);
	DrawFormatString(560, 50, 0xff0000, "�J�n����̃^�C�� : %d",m_allCount);

}

void GameManager::ChangeNextPhase()
{
	m_nowPhaseName = static_cast<ePhaseName>(m_nowPhaseName + 1);
	m_allCount += m_phaseCount;
	m_phaseCount = 0;
}

const MyLib::Vec3 GameManager::GetPlayerPos()const
{
	return m_pPlayer->GetPos();
}

void GameManager::KilledEnemy(int point)
{
	m_trapPoint += point;

	//�R���{�N�[���^�C���Ƃ�����肽��
}
