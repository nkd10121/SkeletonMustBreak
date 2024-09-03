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

	//TODO:UpdateのInputはデバッグ用のため必要なくなったら消す

private:
	std::vector<MyLib::Vec3> CreateEnemyRoute(const char* stageName);

	void CreateEnemy(std::string name,std::shared_ptr<MyLib::Physics> physics);
private:
	std::list<std::shared_ptr<EnemyBase>> m_pEnemys;
	std::list<std::shared_ptr<EnemyGenerateInfo>> m_pGenerateInfo;

	//1フェーズ当たりの敵の生成数
	std::unordered_map<int, int> m_enemyGenerationCountPerOnePhase;

	//一時的な変数
	int temp_enemyHandle;
	int temp_weaponHandle;

	int m_deathEnemyNum;
};

