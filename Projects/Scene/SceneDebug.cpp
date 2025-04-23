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
	//テキストのX座標
	constexpr int kTextX = 64;
	//テキストのY座標
	constexpr int kTextY = 32;
	//テキストのY座標の間隔
	constexpr int kTextIntervalY = 24;
}

/// <summary>
/// コンストラクタ
/// </summary>
/// <param name="mgr">シーン管理クラスの参照</param>
SceneDebug::SceneDebug(SceneManager& mgr):
	SceneBase(mgr),
	m_destinationScene(eDestination::Title)
{
}

/// <summary>
/// デストラクタ
/// </summary>
SceneDebug::~SceneDebug()
{
}

/// <summary>
/// 更新
/// </summary>
/// <param name="input">入力管理クラスの参照</param>
void SceneDebug::Update(std::shared_ptr<Input>& input)
{
	if (input->IsTriggered("UP"))
	{
		if (m_destinationScene != eDestination::Title)
		{
			m_destinationScene = static_cast<eDestination>(static_cast<int>(m_destinationScene) - 1);
		}
	}

	if (input->IsTriggered("DOWN"))
	{
		if (m_destinationScene != eDestination::Ranking)
		{
			m_destinationScene = static_cast<eDestination>(static_cast<int>(m_destinationScene) + 1);
		}
	}

	if (input->IsTriggered("A"))
	{
		if (m_destinationScene == eDestination::Title)
		{
			m_manager.ChangeScene(std::make_shared<SceneTitle>(m_manager));
		}

		if (m_destinationScene == eDestination::StageSelect)
		{
			m_manager.ChangeScene(std::make_shared<SceneStageSelect>(m_manager));
		}

		if (m_destinationScene == eDestination::Upgrade)
		{
			m_manager.PushScene(std::make_shared<SceneUpgrade>(m_manager));
		}

		if (m_destinationScene == eDestination::InGame)
		{
			m_manager.ChangeScene(std::make_shared<SceneGame>(m_manager));
		}

		if (m_destinationScene == eDestination::Result)
		{
			m_manager.PushScene(std::make_shared<SceneResult>(m_manager));
		}
		
		if (m_destinationScene == eDestination::Pause)
		{
			m_manager.PushScene(std::make_shared<ScenePause>(m_manager));
		}
		
		if (m_destinationScene == eDestination::Option)
		{
			m_manager.PushScene(std::make_shared<SceneOption>(m_manager));
		}
		
		if (m_destinationScene == eDestination::Ranking)
		{
			m_manager.ChangeScene(std::make_shared<SceneRanking>(m_manager));
		}
	}
}

/// <summary>
/// 描画
/// </summary>
void SceneDebug::Draw()
{
#ifdef _DEBUG
	DrawFormatString(0, 0, 0xffffff, "SceneDebug");

	DrawFormatString(kTextX / 2, kTextY + static_cast<int>(m_destinationScene) * kTextIntervalY, 0xff0000, "→");

	DrawFormatString(kTextX, kTextY + static_cast<int>(eDestination::Title) * kTextIntervalY, 0xffffff, "SceneTitle");
	DrawFormatString(kTextX, kTextY + static_cast<int>(eDestination::StageSelect) * kTextIntervalY, 0xffffff, "SceneStageSelect");
	DrawFormatString(kTextX, kTextY + static_cast<int>(eDestination::Upgrade) * kTextIntervalY, 0xffffff, "SceneUpgrade");
	DrawFormatString(kTextX, kTextY + static_cast<int>(eDestination::InGame) * kTextIntervalY, 0xffffff, "SceneGame");
	DrawFormatString(kTextX, kTextY + static_cast<int>(eDestination::Result) * kTextIntervalY, 0xffffff, "SceneResult");
	DrawFormatString(kTextX, kTextY + static_cast<int>(eDestination::Pause) * kTextIntervalY, 0xffffff, "ScenePause");
	DrawFormatString(kTextX, kTextY + static_cast<int>(eDestination::Option) * kTextIntervalY, 0xffffff, "SceneOption");
	DrawFormatString(kTextX, kTextY + static_cast<int>(eDestination::Ranking) * kTextIntervalY, 0xffffff, "SceneRanking");
#endif
}
