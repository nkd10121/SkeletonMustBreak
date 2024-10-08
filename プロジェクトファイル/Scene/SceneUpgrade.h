#pragma once
#include "SceneBase.h"
class SceneUpgrade : public SceneBase
{
public:
	SceneUpgrade(SceneManager& mgr);
	virtual ~SceneUpgrade();

	/// <summary>
	/// 更新処理
	/// </summary>
	/// <param name="input"></param>
	void Update(std::shared_ptr<Input>& input);

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();

private:
};

