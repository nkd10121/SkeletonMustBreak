#pragma once
#include <List>
#include <string>
#include <map>
#include <memory>
#include "MyLib.h"

class EnemyBase;
class GameManager;

/// <summary>
/// 敵関係を管理するクラス
/// </summary>
class EnemyManager
{
public:
	/// <summary>
	/// 敵生成情報
	/// </summary>
	struct EnemyGenerateInfo
	{
		std::string enemyName;		//敵の名前
		int appearPhase;			//出現フェーズ
		int appearFrame;			//出現フレーム
		int appearPos;				//生成場所(現在は未使用)

		bool isCreated = false;		//生成済みかどうか
	};

public:
	//コンストラクタ
	EnemyManager();
	//デストラクタ
	virtual ~EnemyManager();

	//初期化
	void Init(int* playerKillCount, int* trapKillCount,int* comboMax,const char* stageName);
	//更新
	void Update(std::shared_ptr<MyLib::Physics> physics,GameManager* gameManager,MyLib::Vec3 playerPos, MyLib::Vec3 playerDir, bool isPlayerChase);
	//描画
	void Draw();

	//正面の敵の現在のHPを返す
	const int GetFrontEnemyHp()const { return m_frontEnemyHp; }
	//正面の敵の最大HPを返す
	const int GetFrontEnemyMaxHp()const { return m_frontEnemyMaxHp; }
	//敵の座標を返す
	const std::list<MyLib::Vec3> GetEnemyPos()const { return m_enemyPos; }

private:
	//敵の移動ルートの作成
	std::vector<MyLib::Vec3> CreateEnemyRoute();
	//敵の生成
	void CreateEnemy(std::string name,std::shared_ptr<MyLib::Physics> physics);

private:
	//敵の管理
	std::list<std::shared_ptr<EnemyBase>> m_pEnemys;
	//敵の生成情報
	std::list<std::shared_ptr<EnemyGenerateInfo>> m_pGenerateInfo;

	//ステージ名
	const char* m_stageName;

	//1フェーズ当たりの敵の生成数
	std::unordered_map<int, int> m_enemyGenerationCountPerOnePhase;

	//死んだ敵の数
	int m_deletedEnemyNum;

	/*スコア関係*/
	int* m_playerKillCount;	//プレイヤーキル数
	int* m_trapKillCount;	//トラップキル数
	int* m_comboMax;		//最大コンボ数

	//コンボ数
	int m_comboCount;
	//コンボの時間制限
	int m_combTimeLimit;

	//フォントの倍率
	float m_fontMag;

	//正面の敵のHP
	int m_frontEnemyHp;
	//正面の敵の最大HP
	int m_frontEnemyMaxHp;

	//敵の座標
	std::list<MyLib::Vec3> m_enemyPos;
};

