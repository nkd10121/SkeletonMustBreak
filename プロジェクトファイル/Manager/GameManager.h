#pragma once
#include <memory>
#include <string>
#include "MyLib.h"

class Input;
class EnemyManager;
class Camera;
class TrapGrid;
class Player;
class TrapManager;
class HealPortion;

class SceneGame;

struct ScoreInfo;

/// <summary>
/// �Q�[�����[�����Ǘ�����N���X
//	�E�F�[�u�̊Ǘ���������Q�[���I�[�o�[�Ƃ�
/// </summary>
class GameManager
{
public:
	enum ePhaseName : int
	{
		FirstPrePhase,		//��1�����t�F�[�Y
		FirstBattlePhase,	//��1�퓬�t�F�[�Y
		SecondPrePhase,		//��2�����t�F�[�Y
		SecondBattlePhase,	//��2�퓬�t�F�[�Y
		ThirdPrePhase,		//��3�����t�F�[�Y
		ThirdBattlePhase,	//��3�퓬�t�F�[�Y

		GameClear,
		Gameover
	};

public:
	//�R���X�g���N�^
	GameManager(std::string stageName);
	//�f�X�g���N�^
	virtual ~GameManager();

	//������
	void Init(std::shared_ptr<ScoreInfo> pScore);
	//�X�V
	void Update(std::shared_ptr<Input>& input, SceneGame* pSceneGame);
	//�`��
	void Draw();
	//�f�o�b�O�`��
	void DebugDraw();

	//�t�F�[�Y��i�߂�
	void ChangeNextPhase();

	//�N���X�^��HP�����炷
	const void SubCrystalHP();

	//���݂̃t�F�[�Y�����擾
	const int GetThisPhaseCount()const { return m_phaseFrame; }
	//���݂̃t�F�[�Y�����擾
	const int GetThisPhaseName()const { return m_nowPhase; }
	//�Q�[���V�[�����I�����������擾
	const int GetThisSceneEnd()const { return m_isThisSceneEnd; }

	//�v���C���[���W���擾
	const MyLib::Vec3 GetPlayerPos()const;

	//�G���L�������Ƃ���㩃|�C���g���擾
	void AddTrapPoint(int point);

	//�񕜃|�[�V�����𐶐�
	void CreatePortion(MyLib::Vec3 pos);

private:
	//�X�e�[�W��
	std::string m_stageName;

	//�v���C���[�N���X�|�C���^
	std::shared_ptr<Player> m_pPlayer;
	//�G�Ǘ��N���X�|�C���^
	std::shared_ptr<EnemyManager> m_pEnemyManager;
	//�J�����N���X�|�C���^
	std::shared_ptr<Camera> m_pCamera;
	//㩊Ǘ��N���X�|�C���^
	std::shared_ptr<TrapManager> m_pTrapManager;

	//�|�[�V�����N���X�|�C���^
	std::list<std::shared_ptr<HealPortion>> m_pPortion;
	//�|�[�V�����̃h���b�v��
	int m_potionNum;

	//�X�R�A�v�Z�ɕK�v�ȏ��
	std::shared_ptr<ScoreInfo> m_score;
	int* m_playerKillCount;
	int* m_trapKillCount;

	//��̃��f���n���h��
	int m_arrowHandle;
	//�|�[�V�������f���n���h��
	int m_portionHandle;

	//UI�̃O���t�n���h��
	int m_uiHandle;

	// ����
	std::shared_ptr<MyLib::Physics> m_pPhysics;

	//�I�u�W�F�N�gHP
	int m_objectHP;
	//�I�u�W�F�N�gHP�̕`��̔w�i
	int m_objectUIBgHandle;	

	//���݂̃t�F�[�Y
	ePhaseName m_nowPhase;
	//�t�F�[�Y���̃t���[����
	int m_phaseFrame;
	//�o�߃t���[����
	int m_allFrame;

	//�v���C���[�����S���Ă���̃J�E���g
	int m_playerDownCount;

	//�Q�[���V�[�����I���������ǂ���
	bool m_isThisSceneEnd;

	//㩃|�C���g
	int m_trapPoint;

	//�A�C�e���̃n���h��
	std::vector<int> m_slotHandle;
	//���ݑI�����Ă���A�C�e���X���b�g�̔ԍ�
	int m_playerSlotIdx;

	//HP�o�[�֘A
	std::vector<int> m_hpBarHandle;
	//HP�o�[�̉���
	int m_hpBarWidth;
	//HP�o�[�̍���
	int m_hpBarHeight;
	//�v���C���[HP
	int m_playerHp;
	//�v���C���[�̍ő�HP
	int m_playerMaxHp;

	//�t�H���g�n���h��
	int m_fontHandle;
	int m_trapPointHandle;

	//�N���A�������ǂ���
	bool m_isClear;

	//�G�t�F�N�g�̐����t���[��
	int m_effectFrame;

	//////////////////////
	int m_miniMapHandle;
	MyLib::Vec3 m_crystalPos;

	float m_tempPlayerPosX;
	float m_tempPlayerPosZ;

	int m_miniMapPlayerPosX;
	int m_miniMapPlayerPosY;

	int m_offsetY;

	std::list<MyLib::Vec3> m_drawPos;

	int m_frame;
};

