#include "SceneBase.h"
#include "Game.h"

namespace
{
	//フェード速度
	constexpr int kFadeSpeed = 12;

	//明るさの最大
	constexpr int kBrightMax = 255;
	//明るさの最低
	constexpr int kBrightMin = 0;
}

/// <summary>
/// コンストラクタ
/// 生成時にSceneManagerの参照を受け取っておく
/// </summary>
/// <param name="manager">シーン管理クラスの参照</param>
SceneBase::SceneBase(SceneManager& manager):
	m_manager(manager),
	m_isChangeNextScene(false),
	m_fadeColor(GetColor(0, 0, 0)),
	m_fadeBright(kBrightMax),
	m_fadeSpeed(-kFadeSpeed)
{
	// 初期設定はフェードアウト状態
}

/// <summary>
/// フェードの更新
/// </summary>
void SceneBase::UpdateFade()
{
	m_fadeBright += m_fadeSpeed;
	if (m_fadeBright >= kBrightMax)
	{
		m_fadeBright = kBrightMax;
		if (m_fadeSpeed > 0)
		{
			m_fadeSpeed = 0;
		}
	}
	if (m_fadeBright <= kBrightMin)
	{
		m_fadeBright = kBrightMin;
		if (m_fadeSpeed < 0)
		{
			m_fadeSpeed = 0;
		}
	}
}

/// <summary>
/// フェードの描画
/// </summary>
void SceneBase::DrawFade() const
{
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, m_fadeBright);
	DrawBox(0, 0, Game::kWindowWidth, Game::kWindowHeight, m_fadeColor, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

/// <summary>
/// フェードアウト開始
/// </summary>
void SceneBase::StartFadeOut()
{
	m_fadeSpeed = kFadeSpeed;
}

/// <summary>
/// フェードイン開始
/// </summary>
void SceneBase::StartFadeIn()
{
	m_fadeSpeed = -kFadeSpeed;
}

/// <summary>
/// フェードイン中かどうかを返す
/// </summary>
/// <returns>true:フェードイン中,false:フェードイン中じゃない</returns>
bool SceneBase::IsFadingIn() const
{
	if (m_fadeSpeed < 0)	return true;
	return false;
}

/// <summary>
/// フェードアウト中かどうかを返す
/// </summary>
/// <returns>true:フェードアウト中,false:フェードアウト中じゃない</returns>
bool SceneBase::IsFadingOut() const
{
	if (m_fadeSpeed > 0)	return true;
	return false;
}

/// <summary>
/// フェードアウトが終了したかどうか
/// </summary>
/// <returns>true:終了した,false:終了していない</returns>
bool SceneBase::IsFinishFadeOut() const
{
	if (m_fadeBright == 255 && m_fadeSpeed == 0)
	{
		return true;
	}

	return false;
}

/// <summary>
/// フェードインをスキップする
/// </summary>
void SceneBase::FadeInSkip()
{
	m_fadeBright = kBrightMin;
	m_fadeSpeed = -kFadeSpeed;
}

/// <summary>
/// フェードアウトをスキップする
/// </summary>
void SceneBase::FadeOutSkip()
{
	m_fadeBright = kBrightMax;
	m_fadeSpeed = kFadeSpeed;
}
