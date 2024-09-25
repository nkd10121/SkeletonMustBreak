#pragma once
#include "SceneBase.h"
#include <memory>
#include <vector>
#include <map>

struct ScoreInfo;

class SceneResult : public SceneBase
{
public:
	SceneResult(SceneManager& mgr);
	virtual~SceneResult();

	/// <summary>
	/// 更新処理
	/// </summary>
	/// <param name="input"></param>
	void Update(std::shared_ptr<Input>& input);

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();

private:
	void GetScore();
	void FrameToTime();

	void CalculationScore();

private:
	//遷移先
	enum class e_Destination : int
	{
		InGame,
		details,
		//Upgrade,
		StageSelect,
	};

	e_Destination m_destinationScene;

	//スコア計算に必要な情報
	std::shared_ptr<ScoreInfo> m_scoreInfo;

	int m_frame;
	int m_hp;
	int m_playerKillCount;
	int m_trapKillCount;
	int m_combo;

	int m_min;
	char m_sec1;
	char m_sec2;

	int m_score;

	bool m_isClear;

	std::vector<int> m_handles;

	std::vector<std::pair<int, int>> m_uiPos;


	float m_angle;
	int m_cursorOffsetX;

	//フォントハンドル
	int m_fontHandle;
	int m_textHandle;
	int m_detailHandle;

	int m_alpha;

	//更新メンバ関数ポインタ
	void (SceneResult::* m_updateFunc)(std::shared_ptr<Input> input);
	/*状態関数*/
	//フェードアウト
	void FadeOutUpdate(std::shared_ptr<Input> input);
	//通常
	void NormalUpdate(std::shared_ptr<Input> input);
	//詳細
	void DetailUpdate(std::shared_ptr<Input> input);

	//描画メンバ関数ポインタ
	void (SceneResult::* m_drawFunc)();
	//フェードアウト
	void FadeOutDraw();
	//通常
	void NormalDraw();
	//詳細
	void DetailDraw();
};

