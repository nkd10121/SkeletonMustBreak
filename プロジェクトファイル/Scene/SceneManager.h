#pragma once
#include <list> 
#include <memory>
#include <windows.h>

class Input;
class SceneBase;

struct ScoreInfo
{
	int frame = 0;					//クリアタイム(フレーム数)
	int crystalHp = 0;				//クリスタルのHP
	int KilledcounterByPlayer = 0;	//プレイヤーによるキルのカウント
	int KilledcounterByTrap = 0;	//罠によるキルのカウント
	int MaxCombo = 0;				//最大コンボ数
};

class SceneManager
{
public:
	SceneManager();
	virtual ~SceneManager();

	/// <summary>
	/// 持っているシーンのUpdate関数を呼び出す
	/// </summary>
	/// <param name="input">入力クラス</param>
	void Update(std::shared_ptr<Input>& input);

	/// <summary>
	/// 持っているシーンのDraw関数を呼び出す
	/// </summary>
	void Draw();

	/// <summary>
	/// 実行中のシーンを引数で指定したシーンに切り替える
	/// </summary>
	/// <param name="nextScene"></param>
	void ChangeScene(std::shared_ptr<SceneBase> nextScene);

	/// <summary>
	/// Sceneをクリアしてシーンを切り替える
	/// </summary>
	/// <param name="nextScene"></param>
	void ChangeAndClearScene(std::shared_ptr<SceneBase> nextScene);

	/// <summary>
	/// 現在先頭で実行中のシーンの上にシーンを乗っける
	/// </summary>
	/// <param name="scene"></param>
	void PushScene(std::shared_ptr<SceneBase> scene);

	/// <summary>
	/// 現在の先頭シーンを削除する
	/// </summary>
	void popScene(bool isToTitle = false);


	void SetStageName(std::string name) { m_stageName = name; }
	const std::string GetStageName()const { return m_stageName; }

	void SetIsClear(bool isClear) { m_isClear = isClear; }
	const bool GetIsClear()const { return m_isClear; }

	std::shared_ptr<ScoreInfo> GetScoreInfoPtr() { return m_pScoreInfo; }
	void ResetScoreInfo();

#ifdef _DEBUG
private:
	void DrawDebug();

private:
	float m_updateTime;
	float m_drawTime;
#endif
private:
	std::list < std::shared_ptr<SceneBase>> scenes;

	bool m_isClear;
	std::string m_stageName;

	std::shared_ptr<ScoreInfo> m_pScoreInfo;
};

