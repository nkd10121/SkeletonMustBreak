#pragma once
#include "SceneBase.h"

/// <summary>
/// デバッグシーン
/// </summary>
class SceneDebug : public SceneBase
{
public:
	//コンストラクタ
	SceneDebug(SceneManager& mgr);
	//デストラクタ
	virtual ~SceneDebug();

	//更新
	void Update(std::shared_ptr<Input>& input);
	//描画
	void Draw();

private:
	//遷移先列挙型
	enum class eDestination : int
	{
		Title,			//タイトル
		StageSelect,	//ステージセレクト
		Upgrade,		//強化
		InGame,			//インゲーム
		Result,			//リザルト
		Pause,			//ポーズ
		Option,			//オプション
		Ranking			//ランキング(未実装)
	};

	//遷移先
	eDestination m_destinationScene;
};

