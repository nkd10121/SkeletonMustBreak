#include "SceneBase.h"
#include "SceneManager.h"

#include "Game.h"

namespace
{
#ifdef _DEBUG
	//説明用処理バーの文字の座標
	constexpr int kUpdateBarStringPosX = 0;
	constexpr int kUpdateBarStringPosY = Game::kWindowHeight - 48;
	constexpr int kDrawBarPosStringPosX = 0;
	constexpr int kDrawBarPosStringPosY = Game::kWindowHeight - 32;

	//説明用処理バーの座標
	constexpr int kExplainUpdateBarPosX1 = 34;
	constexpr int kExplainUpdateBarPosY1 = Game::kWindowHeight - 46;
	constexpr int kExplainUpdateBarPosX2 = 62;
	constexpr int kExplainUpdateBarPosY2 = Game::kWindowHeight - 34;
	constexpr int kExplainDrawBarPosX1 = 34;
	constexpr int kExplainDrawBarPosY1 = Game::kWindowHeight - 30;
	constexpr int kExplainDrawBarPosX2 = 62;
	constexpr int kExplainDrawBarPosY2 = Game::kWindowHeight - 18;

	//処理バーの色
	constexpr int kUpdateBarColor = 0x0000ff;
	constexpr int kDrawBarColor = 0xff0000;

	//処理バーの座標
	constexpr int kBarPosX = 0;
	constexpr int kBarPosY = Game::kWindowHeight - 16;
#endif
}

/// <summary>
/// コンストラクタ
/// </summary>
SceneManager::SceneManager()
#ifdef _DEBUG
	:m_updateTime(0.0f),
	m_drawTime(0.0f),
	m_isClear(false)
#endif
{
	m_pScoreInfo = std::make_shared<ScoreInfo>();
}

/// <summary>
/// デストラクタ
/// </summary>
SceneManager::~SceneManager()
{
}

/// <summary>
/// 持っているシーンの更新関数を呼び出す
/// </summary>
/// <param name="input">入力管理クラスの参照</param>
void SceneManager::Update(std::shared_ptr<Input>& input)
{
#ifdef _DEBUG
	//更新前のローディング時間取得
	LONGLONG start = GetNowHiPerformanceCount();
#endif

	//末尾のみ実行
	scenes.back()->Update(input);

#ifdef _DEBUG
	//更新後のローディング時間から更新前のローディング時間を引いた値を取得
	m_updateTime = static_cast<float>(GetNowHiPerformanceCount() - start);
#endif
}

/// <summary>
/// 持っているシーンの描画関数を呼び出す
/// </summary>
void SceneManager::Draw()
{
#ifdef _DEBUG
	//更新前のローディング時間取得
	LONGLONG start = GetNowHiPerformanceCount();
#endif
	//先頭から順に描画
	//最後に積んだものが最後に描画される
	for (auto& scenes : scenes)
	{
		scenes->Draw();
	}
	
#ifdef _DEBUG
	//更新後のローディング時間から更新前のローディング時間を引いた値を取得
	m_drawTime = static_cast<float>(GetNowHiPerformanceCount() - start);

	DrawDebug();
#endif
}

/// <summary>
/// 実行中のシーンを引数で指定したシーンに切り替える
/// </summary>
/// <param name="nextScene">遷移先のシーンのポインタ</param>
void SceneManager::ChangeScene(std::shared_ptr<SceneBase> nextScene)
{
	//リストが空っぽだったら入れ替えずに
	if (scenes.empty())
	{
		//末尾に追加
		scenes.push_back(nextScene);
	}
	else
	{
		//既に一つ以上あれば末尾を入れ替える
		scenes.back() = nextScene;
	}
}

/// <summary>
/// シーンをリセットして切り替える
/// </summary>
/// <param name="nextScene">遷移先のシーンのポインタ</param>
void SceneManager::ChangeAndClearScene(std::shared_ptr<SceneBase> nextScene)
{
	//シーンをリセット
	scenes.clear();

	//末尾に追加
	scenes.push_back(nextScene);
}

/// <summary>
/// 現在先頭で実行中のシーンの上にシーンを乗っける
/// </summary>
/// <param name="scene">乗っけたいシーンのポインタ</param>
void SceneManager::PushScene(std::shared_ptr<SceneBase> scene)
{
	scenes.push_back(scene);
}

/// <summary>
/// 現在の先頭シーンを削除する
/// </summary>
void SceneManager::popScene(bool isToTitle)
{
	scenes.pop_back();
}

/// <summary>
/// スコア計算に必要な情報をリセットする
/// </summary>
void SceneManager::ResetScoreInfo()
{
	m_pScoreInfo.reset();
	m_pScoreInfo = std::make_shared<ScoreInfo>();
}

#ifdef _DEBUG
/// <summary>
/// 処理速度をデバッグ描画する
/// </summary>
void SceneManager::DrawDebug()
{
	//処理バーの表示　
	//説明
	DrawString(kUpdateBarStringPosX, kUpdateBarStringPosY, "処理", 0xffffff, 0x000000);
	DrawBox(kExplainUpdateBarPosX1, kExplainUpdateBarPosY1, kExplainUpdateBarPosX2, kExplainUpdateBarPosY2, kUpdateBarColor, true);
	DrawString(kDrawBarPosStringPosX, kDrawBarPosStringPosY, "描画", 0xffffff, 0x000000);
	DrawBox(kExplainDrawBarPosX1, kExplainDrawBarPosY1, kExplainDrawBarPosX2, kExplainDrawBarPosY2, kDrawBarColor, true);

	float rate = static_cast<float>(m_updateTime + m_drawTime) / 16666.6f;
	float width = static_cast<float>(Game::kWindowWidth * rate);
	DrawBox(kBarPosX, kBarPosY, static_cast<int>(width), Game::kWindowHeight, kDrawBarColor, true);

	rate = static_cast<float>(m_updateTime) / 16666.6f;
	width = static_cast<float>(Game::kWindowWidth * rate);
	DrawBox(kBarPosX, kBarPosY, static_cast<int>(width), Game::kWindowHeight, kUpdateBarColor, true);
}
#endif