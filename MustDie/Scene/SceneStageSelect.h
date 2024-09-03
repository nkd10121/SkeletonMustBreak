#pragma once
#include "SceneBase.h"

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
		Stage1,
		Stage2,
		Stage3,
		Upgrade,
	};

	e_Destination m_destinationScene;

	std::vector<int> m_handles;


	int m_lightHandle;
	std::shared_ptr<TitlePlayer> m_player;
};

