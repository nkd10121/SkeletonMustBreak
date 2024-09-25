#pragma once
#include <List>
#include <string>
#include <map>
#include <memory>
#include "MyLib.h"

class EnemyBase;
class GameManager;

class EnemyManager
{
public:
	/// <summary>
	/// �G�������
	/// </summary>
	struct EnemyGenerateInfo
	{
		std::string enemyName;		//�G�̖��O
		int appearPhase;			//�o���t�F�[�Y
		int appearFrame;			//�o���t���[��
		int appearPos;				//�����ꏊ(���݂͖��g�p)

		bool isCreated = false;		//�����ς݂��ǂ���
	};

public:
	//�R���X�g���N�^
	EnemyManager();
	//�f�X�g���N�^
	virtual ~EnemyManager();

	//������
	void Init(int* playerKillCount, int* trapKillCount,int* comboMax,const char* stageName);
	//�X�V
	void Update(std::shared_ptr<MyLib::Physics> physics,GameManager* gameManager,MyLib::Vec3 playerPos, MyLib::Vec3 playerDir, bool isPlayerChase);
	//�`��
	void Draw();

	//���ʂ̓G�̌��݂�HP��Ԃ�
	const int GetFrontEnemyHp()const { return m_frontEnemyHp; }
	//���ʂ̓G�̍ő�HP��Ԃ�
	const int GetFrontEnemyMaxHp()const { return m_frontEnemyMaxHp; }

	const std::list<MyLib::Vec3> GetEnemyPos()const { return m_enemyPos; }

private:
	//�G�̈ړ����[�g�̍쐬
	std::vector<MyLib::Vec3> CreateEnemyRoute();
	//�G�̐���
	void CreateEnemy(std::string name,std::shared_ptr<MyLib::Physics> physics);

private:
	//�G�̊Ǘ�
	std::list<std::shared_ptr<EnemyBase>> m_pEnemys;
	//�G�̐������
	std::list<std::shared_ptr<EnemyGenerateInfo>> m_pGenerateInfo;

	//�X�e�[�W��
	const char* m_stageName;

	//1�t�F�[�Y������̓G�̐�����
	std::unordered_map<int, int> m_enemyGenerationCountPerOnePhase;

	//�G�̃��f���n���h�����i�[����
	std::map<std::string,int> m_enemyHandle;
	//����̃n���h��
	int m_swordHandle;

	//���񂾓G�̐�
	int m_deletedEnemyNum;

	/*�X�R�A�֌W*/
	int* m_playerKillCount;	//�v���C���[�L����
	int* m_trapKillCount;	//�g���b�v�L����
	int* m_comboMax;		//�ő�R���{��

	//�R���{��
	int m_comboCount;
	//�R���{�̎��Ԑ���
	int m_combTimeLimit;

	//�t�H���g�̔{��
	float m_fontMag;

	//���ʂ̓G��HP
	int m_frontEnemyHp;
	//���ʂ̓G�̍ő�HP
	int m_frontEnemyMaxHp;

	std::list<MyLib::Vec3> m_enemyPos;
};

