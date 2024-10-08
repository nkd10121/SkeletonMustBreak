#pragma once
#include "SceneBase.h"
#include <memory>
#include <vector>
#include <map>

struct ScoreInfo;
class UICursor;

/// <summary>
/// リザルトシーン
/// </summary>
class SceneResult : public SceneBase
{
public:
	//コンストラクタ
	SceneResult(SceneManager& mgr);
	//デストラクタ
	virtual~SceneResult();

	//更新
	void Update(std::shared_ptr<Input>& input);
	//描画
	void Draw();

private:
	//スコア計算に必要な情報を取得
	void GetScore();
	//フレーム数を秒数に変換
	void FrameToTime();
	//スコアを計算する
	void CalculationScore();

private:
	//遷移先
	enum class eDestination : int
	{
		InGame,
		details,
		//Upgrade,
		StageSelect,
	};

	eDestination m_destinationScene;

	//スコア計算に必要な情報
	std::shared_ptr<ScoreInfo> m_scoreInfo;

	//秒数
	int m_min;
	//ミリ秒の2桁目
	char m_sec1;
	//ミリ秒の1桁目
	char m_sec2;

	//最終的なスコア
	int m_score;

	//クリアしたかどうか
	bool m_isClear;

	//UIの画像ハンドル
	std::vector<int> m_UIHandles;

	//カーソルクラスのポインタ
	std::shared_ptr<UICursor> m_pCursor;

	//フォントハンドル
	int m_fontHandle;
	int m_textHandle;
	int m_detailHandle;

	int m_alpha;

	//更新メンバ関数ポインタ
	void (SceneResult::* m_updateFunc)(std::shared_ptr<Input> input);
	/*状態関数*/
	//更新(フェードアウト)
	void FadeOutUpdate(std::shared_ptr<Input> input);
	//更新(通常)
	void NormalUpdate(std::shared_ptr<Input> input);
	//更新(詳細画面)
	void DetailUpdate(std::shared_ptr<Input> input);

	//描画メンバ関数ポインタ
	void (SceneResult::* m_drawFunc)();
	//描画(フェードアウト)
	void FadeOutDraw();
	//描画(通常)
	void NormalDraw();
	//描画(詳細画面)
	void DetailDraw();
};

