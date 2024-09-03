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
/// ゲームルールを管理するクラス
//	ウェーブの管理だったりゲームオーバーとか
/// </summary>
class GameManager
{
public:
	enum ePhaseName : int
	{
		FirstPrePhase,		//第1準備フェーズ
		FirstBattlePhase,	//第1戦闘フェーズ
		SecondPrePhase,		//第2準備フェーズ
		SecondBattlePhase,	//第2戦闘フェーズ
		ThirdPrePhase,		//第3準備フェーズ
		ThirdBattlePhase,	//第3戦闘フェーズ

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

	// 物理
	std::shared_ptr<MyLib::Physics> m_pPhysics;

	int m_objectHP;		//オブジェクトHP
	int m_objectUIBgHandle;	//オブジェクトHPの描画の背景

	ePhaseName m_nowPhaseName;
	int m_phaseCount;
	int m_allCount;

	int m_playerDownCount;

	bool m_isThisSceneEnd;

	//罠ポイント
	int m_trapPoint;

	//アイテムのハンドル
	std::vector<int> m_slotHandle;
	int m_playerSlotNum;

	//HPバー関連
	std::vector<int> m_hpBarHandle;
	int m_hpBarWidth;
	int m_hpBarHeight;
	int m_playerHp;
	int m_playerMaxHp;

	//フォントハンドル
	int m_fontHandle;
};

