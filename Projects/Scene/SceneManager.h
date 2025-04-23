#pragma once
#include <list> 
#include <memory>
#include <windows.h>

class Input;
class SceneBase;

//スコア計算に必要な情報構造体
struct ScoreInfo
{
	int frame = 0;					//クリアタイム(フレーム数)
	int crystalHp = 0;				//クリスタルのHP
	int KilledcounterByPlayer = 0;	//プレイヤーによるキルのカウント
	int KilledcounterByTrap = 0;	//罠によるキルのカウント
	int MaxCombo = 0;				//最大コンボ数
};

/// <summary>
/// シーン管理クラス
/// </summary>
class SceneManager
{
public:
	//コンストラクタ
	SceneManager();
	//デストラクタ
	virtual ~SceneManager();

	//持っているシーンの更新関数を呼び出す
	void Update(std::shared_ptr<Input>& input);

	//持っているシーンの描画関数を呼び出す
	void Draw();

	//実行中のシーンを引数で指定したシーンに切り替える
	void ChangeScene(std::shared_ptr<SceneBase> nextScene);

	//シーンをリセットして切り替える
	void ChangeAndClearScene(std::shared_ptr<SceneBase> nextScene);

	//現在先頭で実行中のシーンの上にシーンを乗っける
	void PushScene(std::shared_ptr<SceneBase> scene);

	//現在の先頭シーンを削除する
	void popScene(bool isToTitle = false);

	//ステージ名を設定する
	void SetStageName(std::string name) { m_stageName = name; }
	//ステージ名を取得する
	const std::string GetStageName()const { return m_stageName; }

	//結果を設定する
	void SetIsClear(bool isClear) { m_isClear = isClear; }
	//結果を取得する
	const bool GetIsClear()const { return m_isClear; }

	//スコア計算に必要な情報
	std::shared_ptr<ScoreInfo> GetScoreInfoPtr() { return m_pScoreInfo; }
	//スコア計算に必要な情報をリセットする
	void ResetScoreInfo();

#ifdef _DEBUG
private:
	//処理速度をデバッグ描画する
	void DrawDebug();

private:
	//更新処理速度
	float m_updateTime;
	//描画処理速度
	float m_drawTime;
#endif
private:
	//現在のシーン
	std::list < std::shared_ptr<SceneBase>> scenes;

	//クリアフラグ
	bool m_isClear;
	//ステージ名
	std::string m_stageName;

	//スコア計算に必要な情報ポインタ
	std::shared_ptr<ScoreInfo> m_pScoreInfo;
};

