#pragma once
#include "DxLib.h"
#include "Input.h"
#include <memory>

class SceneManager;

/// <summary>
/// シーン基底クラス
/// いろんなシーンの基になるクラス
/// </summary>
class SceneBase
{
public:
	/// <summary>
	/// 生成時にSceneManagerの参照を受け取っておく
	/// </summary>
	/// <param name="manager">SceneManagerの参照</param>
	SceneBase(SceneManager& manager);

	/// <summary>
	/// シーンの更新
	/// </summary>
	/// <param name="input"></param>
	virtual void Update(std::shared_ptr<Input>& input) = 0;

	/// <summary>
	/// シーンの描画
	/// </summary>
	virtual void Draw() = 0;

protected:
	void UpdateFade();
	void DrawFade() const;

	void StartFadeOut();	// フェードアウト開始
	void StartFadeIn();	// フェードイン開始

	bool IsFadingIn() const;	// フェードイン中
	bool IsFadingOut() const;	// フェードアウト中
	bool IsFading() const { return IsFadingIn() || IsFadingOut(); }	// フェードインorアウト中

	bool IsFinishFadeOut()const;

	void FadeInSkip();
	void FadeOutSkip();

protected:
	SceneManager& m_manager;	//シーン管理クラスの参照
	bool m_isToNextScene;

private:
	// フェード関連処理
	int m_fadeColor;
	int m_fadeBright;
	int m_fadeSpeed;

};

