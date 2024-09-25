#pragma once
#include "SceneBase.h"
#include <memory>
#include <List>

class GameManager;

class SceneGame : public SceneBase
{
public:
	SceneGame(SceneManager& mgr);
	virtual ~SceneGame();

	/// <summary>
	/// XVˆ—
	/// </summary>
	/// <param name="input"></param>
	void Update(std::shared_ptr<Input>& input);

	/// <summary>
	/// •`‰æˆ—
	/// </summary>
	void Draw();

	void StartFadeOut();

	void ToResultScene(bool isClear);

private:
	std::shared_ptr<GameManager> m_pGameManager;

	bool m_isFadeOuting;

	bool m_isThisSceneEnd;
	int m_waitFrame;
};

