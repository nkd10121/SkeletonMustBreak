#include "SceneDebug.h"
#include "SceneManager.h"
#include "SceneTitle.h"
#include "SceneStageSelect.h"
#include "SceneUpgrade.h"
#include "SceneGame.h"
#include "SceneResult.h"
#include "ScenePause.h"
#include "SceneOption.h"
#include "SceneRanking.h"

namespace
{
	constexpr int kTextX = 64;
	constexpr int kTextBlankSpaceY = 32;
	constexpr int kTextIntervalY = 24;
}

SceneDebug::SceneDebug(SceneManager& mgr):
	SceneBase(mgr)
{
	m_destinationScene = e_Destination::Title;
}

SceneDebug::~SceneDebug()
{
}

void SceneDebug::Update(std::shared_ptr<Input>& input)
{
	if (input->IsTriggered("UP"))
	{
		if (m_destinationScene != e_Destination::Title)
		{
			m_destinationScene = static_cast<e_Destination>(static_cast<int>(m_destinationScene) - 1);
		}
	}

	if (input->IsTriggered("DOWN"))
	{
		if (m_destinationScene != e_Destination::Ranking)
		{
			m_destinationScene = static_cast<e_Destination>(static_cast<int>(m_destinationScene) + 1);
		}
	}

	if (input->IsTriggered("A"))
	{
		if (m_destinationScene == e_Destination::Title)
		{
			m_manager.ChangeScene(std::make_shared<SceneTitle>(m_manager));
		}

		if (m_destinationScene == e_Destination::StageSelect)
		{
			m_manager.ChangeScene(std::make_shared<SceneStageSelect>(m_manager));
		}

		if (m_destinationScene == e_Destination::Upgrade)
		{
			m_manager.PushScene(std::make_shared<SceneUpgrade>(m_manager));
		}

		if (m_destinationScene == e_Destination::InGame)
		{
			m_manager.ChangeScene(std::make_shared<SceneGame>(m_manager));
		}

		if (m_destinationScene == e_Destination::Result)
		{
			m_manager.PushScene(std::make_shared<SceneResult>(m_manager));
		}
		
		if (m_destinationScene == e_Destination::Pause)
		{
			m_manager.PushScene(std::make_shared<ScenePause>(m_manager));
		}
		
		if (m_destinationScene == e_Destination::Option)
		{
			m_manager.PushScene(std::make_shared<SceneOption>(m_manager));
		}
		
		if (m_destinationScene == e_Destination::Ranking)
		{
			m_manager.ChangeScene(std::make_shared<SceneRanking>(m_manager));
		}
	}
}

void SceneDebug::Draw()
{
#ifdef _DEBUG
	DrawFormatString(0, 0, 0xffffff, "SceneDebug");

	DrawFormatString(kTextX / 2, kTextBlankSpaceY + static_cast<int>(m_destinationScene) * kTextIntervalY, 0xff0000, "Å®");

	DrawFormatString(kTextX, kTextBlankSpaceY + static_cast<int>(e_Destination::Title) * kTextIntervalY, 0xffffff, "SceneTitle");
	DrawFormatString(kTextX, kTextBlankSpaceY + static_cast<int>(e_Destination::StageSelect) * kTextIntervalY, 0xffffff, "SceneStageSelect");
	DrawFormatString(kTextX, kTextBlankSpaceY + static_cast<int>(e_Destination::Upgrade) * kTextIntervalY, 0xffffff, "SceneUpgrade");
	DrawFormatString(kTextX, kTextBlankSpaceY + static_cast<int>(e_Destination::InGame) * kTextIntervalY, 0xffffff, "SceneGame");
	DrawFormatString(kTextX, kTextBlankSpaceY + static_cast<int>(e_Destination::Result) * kTextIntervalY, 0xffffff, "SceneResult");
	DrawFormatString(kTextX, kTextBlankSpaceY + static_cast<int>(e_Destination::Pause) * kTextIntervalY, 0xffffff, "ScenePause");
	DrawFormatString(kTextX, kTextBlankSpaceY + static_cast<int>(e_Destination::Option) * kTextIntervalY, 0xffffff, "SceneOption");
	DrawFormatString(kTextX, kTextBlankSpaceY + static_cast<int>(e_Destination::Ranking) * kTextIntervalY, 0xffffff, "SceneRanking");
#endif
}
