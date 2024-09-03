#include "ScenePause.h"
#include "SceneGame.h"
#include "SceneOption.h"
#include "SceneTitle.h"
#include "SceneManager.h"

ScenePause::ScenePause(SceneManager& mgr) :
	SceneBase(mgr)
{
	m_destinationScene = e_Destination::InGame;

	FadeInSkip();

}

ScenePause::~ScenePause()
{
}

void ScenePause::Update(std::shared_ptr<Input>& input)
{
	UpdateFade();

	if (!m_isToNextScene)
	{
		if (input->IsTriggered("UP"))
		{
			if (m_destinationScene != e_Destination::InGame)
			{
				m_destinationScene = static_cast<e_Destination>(static_cast<int>(m_destinationScene) - 1);
			}
		}

		if (input->IsTriggered("DOWN"))
		{
			if (m_destinationScene != e_Destination::Title)
			{
				m_destinationScene = static_cast<e_Destination>(static_cast<int>(m_destinationScene) + 1);
			}
		}

		if (input->IsTriggered("OK"))
		{
			if (m_destinationScene == e_Destination::InGame)
			{
				m_manager.popScene();
			}

			if (m_destinationScene == e_Destination::Option)
			{
				m_manager.PushScene(std::make_shared<SceneOption>(m_manager));
			}

			if (m_destinationScene == e_Destination::Title)
			{
				m_isToNextScene = true;
				StartFadeOut();	// フェードアウト開始
			}
		}
	}

	if (m_isToNextScene)
	{
		if (!IsFadingOut())
		{
			if (m_destinationScene == e_Destination::Title)
			{
				m_manager.ChangeAndClearScene(std::make_shared<SceneTitle>(m_manager));
			}
		}
	}
}

void ScenePause::Draw()
{
#ifdef _DEBUG
	DrawFormatString(200, 0, 0xffffff, "ScenePause");

	DrawFormatString(200 - 16, 32 + static_cast<int>(m_destinationScene) * 16, 0xff0000, "→");

	DrawFormatString(200, 32 + static_cast<int>(e_Destination::InGame) * 16, 0xffffff, "SceneGame");
	DrawFormatString(200, 32 + static_cast<int>(e_Destination::Option) * 16, 0xffffff, "SceneOption");
	DrawFormatString(200, 32 + static_cast<int>(e_Destination::Title) * 16, 0xffffff, "SceneTitle");
#endif
	DrawFade();
}
