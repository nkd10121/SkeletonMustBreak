#pragma once
#include "SceneBase.h"
class SceneDebug : public SceneBase
{
public:
	SceneDebug(SceneManager& mgr);
	virtual ~SceneDebug();

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
		Title,
		StageSelect,
		Upgrade,
		InGame,
		Result,
		Pause,
		Option,
		Ranking
	};

	e_Destination m_destinationScene;
};

