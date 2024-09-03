#pragma once
#include "SceneBase.h"
class SceneOption : public SceneBase
{
public:
	SceneOption(SceneManager& mgr);
	virtual ~SceneOption();

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

	int m_fontHandle;
	int m_bgHandle;
};

