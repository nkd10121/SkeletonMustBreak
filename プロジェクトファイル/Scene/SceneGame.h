#pragma once
#include "SceneBase.h"
#include <memory>
#include <List>

class GameManager;

/// <summary>
/// インゲームシーン
/// </summary>
class SceneGame : public SceneBase
{
public:
	//コンストラクタ
	SceneGame(SceneManager& mgr);
	//デストラクタ
	virtual ~SceneGame();

	//更新
	void Update(std::shared_ptr<Input>& input);
	//描画
	void Draw();

	//フェードアウト開始
	void StartFadeOut();
	//リザルトシーンに遷移する
	void ToResultScene(bool isClear);

private:
	//ゲームを管理するクラスのポインタ
	std::shared_ptr<GameManager> m_pGameManager;

	//フェードアウト中かどうか
	bool m_isFadeOuting;
	//このシーンが終了するかどうか
	bool m_isThisSceneEnd;
	//リザルトシーンに遷移するまでの待機フレーム
	int m_waitFrame;
};

