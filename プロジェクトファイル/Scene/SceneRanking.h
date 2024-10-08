#pragma once
#include "SceneBase.h"

/// <summary>
/// ランキングシーン(未実装)
/// </summary>
class SceneRanking :  public SceneBase
{
public:
	//コンストラクタ
	SceneRanking(SceneManager& mgr);
	//デストラクタ
	virtual ~SceneRanking();

	//更新
	void Update(std::shared_ptr<Input>& input);
	//描画
	void Draw();

private:
};

