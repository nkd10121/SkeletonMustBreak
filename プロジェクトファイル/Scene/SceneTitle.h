#pragma once
#include "SceneBase.h"
#include <vector>
#include <memory>

class TitlePlayer;

class SceneTitle : public SceneBase
{
public:
	SceneTitle(SceneManager& mgr);
	virtual~SceneTitle();

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
		StageSelect,
		Option,
		Quit,
		Ranking,
	};

	std::vector<int> m_handles;

	int m_lightHandle;

	float m_angle;
	int m_cursorOffsetX;
	e_Destination m_destinationScene;

	std::shared_ptr<TitlePlayer> m_player;
};

