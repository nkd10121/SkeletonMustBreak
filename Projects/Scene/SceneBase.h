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
	//コンストラクタ
	SceneBase(SceneManager& manager);

	//更新
	virtual void Update(std::shared_ptr<Input>& input) = 0;

	//描画
	virtual void Draw() = 0;

protected:
	//フェードの更新
	void UpdateFade();
	//フェードの描画
	void DrawFade() const;

	//フェードアウト開始
	void StartFadeOut();
	//フェードイン開始
	void StartFadeIn();

	//フェードイン中かどうかを返す
	bool IsFadingIn() const;
	//フェードアウト中かどうかを返す
	bool IsFadingOut() const;
	//フェードインorアウト中かどうかを返す
	bool IsFading() const { return IsFadingIn() || IsFadingOut(); }	

	//フェードアウトが終了したかどうか
	bool IsFinishFadeOut()const;

	//フェードインをスキップする
	void FadeInSkip();
	//フェードアウトをスキップする
	void FadeOutSkip();

protected:
	//シーン管理クラスの参照
	SceneManager& m_manager;
	//次のシーンに遷移するタイミング
	bool m_isChangeNextScene;

private:
	//フェードカラー
	int m_fadeColor;
	//フェードの明るさ
	int m_fadeBright;
	//フェードスピード
	int m_fadeSpeed;
};