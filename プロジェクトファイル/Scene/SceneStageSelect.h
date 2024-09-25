#pragma once
#include "SceneBase.h"

#include <vector>
#include <map>

class TitlePlayer;

class SceneStageSelect : public SceneBase
{
public:
	SceneStageSelect(SceneManager& mgr);
	virtual ~SceneStageSelect();

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
		stage1,
		stage2,
		stage3,
		Upgrade,
	};

	e_Destination m_destinationScene;

	std::vector<std::pair<int, int>> m_uiPos;

	std::vector<int> m_handles;

	float m_angle;
	int m_cursorOffsetX;

	int m_fontHandle;
	int m_textHandle;

	int m_lightHandle;
	std::shared_ptr<TitlePlayer> m_player;

	int m_stage1Score;
	int m_stage2Score;
	int m_stage3Score;
};

