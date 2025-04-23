#pragma once
#include "SceneBase.h"

class UICursor;

/// <summary>
/// ポーズシーン
/// </summary>
class ScenePause : public SceneBase
{
public:
	//コンストラクタ
	ScenePause(SceneManager& mgr);
	//デストラクタ
	virtual ~ScenePause();

	//更新
	void Update(std::shared_ptr<Input>& input);
	//描画
	void Draw();

private:
	//遷移先
	enum class eDestination : int
	{
		InGame,
		Option,
		Title,
	};

	//カーソルクラスのポインタ
	std::shared_ptr<UICursor> m_pCursor;

	//遷移先
	eDestination m_destinationScene;

	//UIフレームのハンドル
	int m_uiFrame;

	//フォントハンドル(必要ないかも)
	int m_textHandle;
	int m_fontHandle;
};

