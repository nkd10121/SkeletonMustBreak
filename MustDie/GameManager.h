#pragma once
#include <memory>
#include "MyLib.h"

class Input;
class EnemyManager;
class Camera;
class TrapGrid;
class Player;
class TrapManager;

class SceneGame;

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
	GameManager();
	virtual ~GameManager();

	void Init();
	void Update(std::shared_ptr<Input>& input, SceneGame* pSceneGame);
	void Draw();
	void DebugDraw();

	void ChangeNextPhase();

	const void SubObjectHP(){ m_objectHP--; }
	const int GetThisPhaseCount()const { return m_phaseCount; }
	const int GetThisPhaseName()const { return m_nowPhaseName; }

	const int GetThisSceneEnd()const { return m_isThisSceneEnd; }

	const MyLib::Vec3 GetPlayerPos()const;

	void KilledEnemy(int point);

private:
	std::shared_ptr<Player> m_pPlayer;
	std::shared_ptr<EnemyManager> m_pEnemyManager;
	std::shared_ptr<Camera> m_pCamera;
	std::shared_ptr<TrapGrid> m_pTrapGrid;
	std::shared_ptr<TrapManager> m_pTrapManager;

	int m_arrowHandle;

	// ����
	std::shared_ptr<MyLib::Physics> m_pPhysics;

	int m_objectHP;		//�I�u�W�F�N�gHP
	int m_objectUIBgHandle;	//�I�u�W�F�N�gHP�̕`��̔w�i

	ePhaseName m_nowPhaseName;
	int m_phaseCount;
	int m_allCount;

	int m_playerDownCount;

	bool m_isThisSceneEnd;

	//㩃|�C���g
	int m_trapPoint;

	//�A�C�e���̃n���h��
	std::vector<int> m_slotHandle;
	int m_playerSlotNum;

	//HP�o�[�֘A
	std::vector<int> m_hpBarHandle;
	int m_hpBarWidth;
	int m_hpBarHeight;
	int m_playerHp;
	int m_playerMaxHp;

	//�t�H���g�n���h��
	int m_fontHandle;
};

