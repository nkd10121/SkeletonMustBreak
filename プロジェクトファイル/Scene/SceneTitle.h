#pragma once
#include "SceneBase.h"
#include <vector>
#include <memory>

class TitlePlayer;
class UICursor;

/// <summary>
/// タイトルシーン
/// </summary>
class SceneTitle : public SceneBase
{
public:
	//コンストラクタ
	SceneTitle(SceneManager& mgr);
	//デストラクタ
	virtual~SceneTitle();
	
	//更新
	void Update(std::shared_ptr<Input>& input);
	//描画
	void Draw();

private:
	//遷移先
	enum class eDestination : int
	{
		StageSelect,
		Option,
		Quit,
		Ranking,
	};

	//カーソルクラスのポインタ
	std::shared_ptr<UICursor> m_pCursor;
	//UI画像ハンドル
	std::vector<int> m_UIHandles;
	//ライトハンドル
	int m_lightHandle;
	//遷移先
	eDestination m_destinationScene;
	//背景のプレイヤーポインタ
	std::shared_ptr<TitlePlayer> m_pPlayer;
};

