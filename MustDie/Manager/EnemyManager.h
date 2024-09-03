#pragma once
#include <List>
#include <string>
#include <memory>

#include "MyLib.h"

class EnemyBase;
class Input;
class GameManager;

class EnemyManager
{
public:
	struct EnemyGenerateInfo
	{
		std::string enemyName;
		int appearPhase;
		int appearFrame;
		int appearPos;

		bool isCreated = false;
	};

public:
	EnemyManager();
	virtual ~EnemyManager();

	void Init();
	void Update(std::shared_ptr<Input> input, std::shared_ptr<MyLib::Physics> physics,GameManager* gameManager,bool isPlayerChase);
	void Draw();

	//TODO:Update��Input�̓f�o�b�O�p�̂��ߕK�v�Ȃ��Ȃ��������

private:
	std::vector<MyLib::Vec3> CreateEnemyRoute(const char* stageName);

	void CreateEnemy(std::string name,std::shared_ptr<MyLib::Physics> physics);
private:
	std::list<std::shared_ptr<EnemyBase>> m_pEnemys;
	std::list<std::shared_ptr<EnemyGenerateInfo>> m_pGenerateInfo;

	//1�t�F�[�Y������̓G�̐�����
	std::unordered_map<int, int> m_enemyGenerationCountPerOnePhase;

	//�ꎞ�I�ȕϐ�
	int temp_enemyHandle;
	int temp_weaponHandle;

	int m_deathEnemyNum;
};

