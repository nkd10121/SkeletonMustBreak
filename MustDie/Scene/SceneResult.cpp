#include "SceneResult.h"
#include "SceneTitle.h"
#include "SceneGame.h"
#include "SceneUpgrade.h"
#include "SceneStageSelect.h"
#include "SceneManager.h"

#include "Font.h"

namespace
{
	//フォントのパス
	const char* kFontPath = "data/font/Dela-Gothic-One.ttf";
	const char* kFontName = "Dela Gothic One";
}

SceneResult::SceneResult(SceneManager& mgr) :
	SceneBase		(mgr)
{
	m_destinationScene = e_Destination::InGame;

	m_isClear = mgr.GetIsClear();

	m_fontHandle = Font::GetInstance().GetFontHandle(kFontPath, kFontName, 64);
	m_textHandle = Font::GetInstance().GetFontHandle(kFontPath, kFontName, 50);
}

SceneResult::~SceneResult()
{
	// ********** フォントのアンロード **********
	if (RemoveFontResourceEx(kFontPath, FR_PRIVATE, NULL)) {
	}
}

void SceneResult::Update(std::shared_ptr<Input>& input)
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
		if (m_destinationScene != e_Destination::StageSelect)
		{
			m_destinationScene = static_cast<e_Destination>(static_cast<int>(m_destinationScene) + 1);
		}
	}

	if (input->IsTriggered("OK"))
	{
		if (m_destinationScene == e_Destination::InGame)
		{
			m_manager.ChangeAndClearScene(std::make_shared<SceneGame>(m_manager));
		}

		if (m_destinationScene == e_Destination::Upgrade)
		{
			m_manager.PushScene(std::make_shared<SceneUpgrade>(m_manager));
		}
		
		if (m_destinationScene == e_Destination::StageSelect)
		{
			m_manager.ChangeAndClearScene(std::make_shared<SceneStageSelect>(m_manager));
		}
	}
}

void SceneResult::Draw()
{
	if (m_isClear)
	{
		DrawStringToHandle(640, 200, "クリア!", 0xffffff, m_fontHandle);
	}
	else if(!m_isClear)
	{
		DrawStringToHandle(640, 200, "GAMEOVER...", 0xffffff, m_fontHandle);
	}


	DrawStringToHandle(540, 440, "次のステージに進む", 0xffffff, m_textHandle);
	DrawStringToHandle(540, 520, "強化画面へ", 0xffffff, m_textHandle);
	DrawStringToHandle(540, 600, "ステージセレクトに戻る", 0xffffff, m_textHandle);

#ifdef _DEBUG
	DrawFormatString(200, 0, 0xffffff, "SceneResult");

	DrawFormatString(200 - 16, 32 + static_cast<int>(m_destinationScene) * 16, 0xff0000, "→");

	DrawFormatString(200, 32 + static_cast<int>(e_Destination::InGame) * 16, 0xffffff, "SceneGame");
	DrawFormatString(200, 32 + static_cast<int>(e_Destination::Upgrade) * 16, 0xffffff, "SceneUpgrade");
	DrawFormatString(200, 32 + static_cast<int>(e_Destination::StageSelect) * 16, 0xffffff, "SceneStageSelect");
#endif
}
