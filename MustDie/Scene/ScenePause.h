#pragma once
#include "SceneBase.h"
class ScenePause : public SceneBase
{
public:
	ScenePause(SceneManager& mgr);
	virtual ~ScenePause();

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
	//‘JˆÚæ
	enum class e_Destination : int
	{
		InGame,
		Option,
		Title,
	};

	e_Destination m_destinationScene;
};

