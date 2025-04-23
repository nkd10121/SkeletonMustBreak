#pragma once
#include <memory>
#include <string>
#include "MyLib.h"

class Input;
class EnemyManager;
class Camera;
class Player;
class TrapManager;
class HealPortion;
class SceneGame;
struct ScoreInfo;

/// <summary>
/// ゲームルールを管理するクラス
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

		GameClear,			//ゲームクリア
		Gameover			//ゲームオーバー
	};

public:
	//コンストラクタ
	GameManager(std::string stageName);
	//デストラクタ
	virtual ~GameManager();

	//初期化
	void Init(std::shared_ptr<ScoreInfo> pScore);
	//更新
	void Update(std::shared_ptr<Input>& input, SceneGame* pSceneGame);
	//描画
	void Draw();
	//デバッグ描画
	void DebugDraw();

	//フェーズを進める
	void ChangeNextPhase();

	//クリスタルHPを減らす
	const void SubCrystalHP();

	//現在のフェーズ数を取得
	const int GetThisPhaseCount()const { return m_phaseFrame; }
	//現在のフェーズ名を取得
	const int GetThisPhaseName()const { return m_nowPhase; }
	//ゲームシーンが終了したかを取得
	const int GetThisSceneEnd()const { return m_isThisSceneEnd; }

	//プレイヤー座標を取得
	const MyLib::Vec3 GetPlayerPos()const;

	//敵をキルしたときに罠ポイントを取得
	void AddTrapPoint(int point);

	//回復ポーションを生成
	void CreatePortion(MyLib::Vec3 pos);

private:
	//ステージ名
	std::string m_stageName;

	//プレイヤークラスポインタ
	std::shared_ptr<Player> m_pPlayer;
	//敵管理クラスポインタ
	std::shared_ptr<EnemyManager> m_pEnemyManager;
	//カメラクラスポインタ
	std::shared_ptr<Camera> m_pCamera;
	//罠管理クラスポインタ
	std::shared_ptr<TrapManager> m_pTrapManager;

	//ポーションクラスポインタ
	std::list<std::shared_ptr<HealPortion>> m_pPortion;
	//ポーションのドロップ数
	int m_potionNum;

	//スコア計算に必要な情報
	std::shared_ptr<ScoreInfo> m_score;
	int* m_playerKillCount;
	int* m_trapKillCount;

	//UIのグラフハンドル
	int m_uiHandle;

	// 物理
	std::shared_ptr<MyLib::Physics> m_pPhysics;

	//オブジェクトHP
	int m_objectHP;
	//オブジェクトHPの描画の背景
	int m_objectUIBgHandle;	

	//現在のフェーズ
	ePhaseName m_nowPhase;
	//フェーズ中のフレーム数
	int m_phaseFrame;
	//経過フレーム数
	int m_allFrame;

	//プレイヤーが死亡してからのカウント
	int m_playerDownCount;

	//ゲームシーンが終了したかどうか
	bool m_isThisSceneEnd;

	//罠ポイント
	int m_trapPoint;

	//アイテムのハンドル
	std::vector<int> m_slotHandle;
	//現在選択しているアイテムスロットの番号
	int m_playerSlotIdx;

	//HPバー関連
	std::vector<int> m_hpBarHandle;
	//HPバーの横幅
	int m_hpBarWidth;
	//HPバーの高さ
	int m_hpBarHeight;
	//プレイヤーHP
	int m_playerHp;
	//プレイヤーの最大HP
	int m_playerMaxHp;

	//フォントハンドル
	int m_fontHandle;
	int m_trapPointHandle;

	//クリアしたかどうか
	bool m_isClear;

	//エフェクトの生成フレーム
	int m_effectFrame;

	//ミニマップハンドル
	int m_miniMapHandle;
	//クリスタルの座標
	MyLib::Vec3 m_crystalPos;

#ifdef _DEBUG
	//ミニマップを作成する時に目安にする座標
	std::list<MyLib::Vec3> m_drawPos;
	//フレーム数
	int m_frame;
#endif

	//ミニマップ上のプレイヤー座標
	MyLib::Vec3 m_miniMapPlayerPos;
	//マップごとにY座標の中心が異なるため補正
	int m_offsetY;


};

