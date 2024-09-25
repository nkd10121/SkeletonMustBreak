#pragma once
#include "SceneBase.h"
class SceneRanking :  public SceneBase
{
public:
	SceneRanking(SceneManager& mgr);
	virtual ~SceneRanking();

	/// <summary>
	/// XVˆ—
	/// </summary>
	/// <param name="input"></param>
	void Update(std::shared_ptr<Input>& input);

	/// <summary>
	/// •`‰æˆ—
	/// </summary>
	void Draw();

private:
};

