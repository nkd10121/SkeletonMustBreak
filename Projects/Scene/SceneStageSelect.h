#pragma once
#include "SceneBase.h"

#include <vector>
#include <map>

class TitlePlayer;
class UICursor;

/// <summary>
/// ステージセレクトシーン
/// </summary>
class SceneStageSelect : public SceneBase
{
public:
	//コンストラクタ
	SceneStageSelect(SceneManager& mgr);
	//デストラクタ
	virtual ~SceneStageSelect();

	//更新
	void Update(std::shared_ptr<Input>& input);
	//描画
	void Draw();

private:
	//遷移先
	enum class eDestination : int
	{
		Title,
		stage1,
		stage2,
		stage3,
		Upgrade,
	};

	//カーソルクラスのポインタ
	std::shared_ptr<UICursor> m_pCursor;

	//遷移先
	eDestination m_destinationScene;
	//UI座標
	std::vector<std::pair<int, int>> m_uiPos;
	//UIの画像ハンドル
	std::vector<int> m_UIHandles;

	//ライトハンドル
	int m_lightHandle;
	//背景のプレイヤークラスのポインタ
	std::shared_ptr<TitlePlayer> m_pPlayer;
};

