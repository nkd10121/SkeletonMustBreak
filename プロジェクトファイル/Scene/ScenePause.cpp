#include "ScenePause.h"
#include "SceneGame.h"
#include "SceneOption.h"
#include "SceneTitle.h"
#include "SceneManager.h"
#include "SoundManager.h"

#include "Font.h"
#include "Game.h"

namespace
{
	//フォントのパス
	const char* kFontPath = "data/font/Dela-Gothic-One.ttf";
	const char* kFontName = "Dela Gothic One";

	constexpr int kTextFontSize = 50;
	constexpr int kTextSize = 64;

	enum ui
	{
		uiFrame,
		rightArrow,
		leftArrow,
	};
}

ScenePause::ScenePause(SceneManager& mgr) :
	SceneBase(mgr),
	m_angle(0),
	m_cursorOffsetX(0)
{
	m_destinationScene = e_Destination::InGame;

	FadeInSkip();

	m_uiPos.push_back(std::pair<int, int>(640, 400));
	m_uiPos.push_back(std::pair<int, int>(640, 500));
	m_uiPos.push_back(std::pair<int, int>(640, 600));

	m_handles.push_back(LoadGraph("data/img/ui_frame.png"));
	m_handles.push_back(LoadGraph("data/img/ArrowRight.png"));
	m_handles.push_back(LoadGraph("data/img/ArrowLeft.png"));

	m_textHandle = Font::GetInstance().GetFontHandle(kFontPath, kFontName, kTextFontSize);
	m_fontHandle = Font::GetInstance().GetFontHandle(kFontPath, kFontName, kTextSize);
}

ScenePause::~ScenePause()
{
	for (auto& h : m_handles)
	{
		DeleteGraph(h);
	}
}

void ScenePause::Update(std::shared_ptr<Input>& input)
{
	UpdateFade();

	m_angle += 0.14f;
	m_cursorOffsetX = static_cast<int>(sinf(m_angle) * 8);

	if (!m_isToNextScene)
	{
		if (input->IsTriggered("UP"))
		{
			if (m_destinationScene != e_Destination::InGame)
			{
				SoundManager::GetInstance().PlaySE("select");
				m_destinationScene = static_cast<e_Destination>(static_cast<int>(m_destinationScene) - 1);
			}
		}

		if (input->IsTriggered("DOWN"))
		{
			if (m_destinationScene != e_Destination::Title)
			{
				SoundManager::GetInstance().PlaySE("select");
				m_destinationScene = static_cast<e_Destination>(static_cast<int>(m_destinationScene) + 1);
			}
		}

		if (input->IsTriggered("A"))
		{
			SoundManager::GetInstance().PlaySE("enter");

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
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 220);
	DrawBox(0, 0, Game::kWindowWidth, Game::kWindowHeight, 0x000000, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	DrawRotaGraph(m_uiPos[static_cast<int>(m_destinationScene)].first + 220 + m_cursorOffsetX, m_uiPos[static_cast<int>(m_destinationScene)].second, 1.0f, 0.0f, m_handles[leftArrow], true);
	DrawRotaGraph(m_uiPos[static_cast<int>(m_destinationScene)].first - 220 - m_cursorOffsetX, m_uiPos[static_cast<int>(m_destinationScene)].second, 1.0f, 0.0f, m_handles[rightArrow], true);

	DrawStringToHandle(Game::kWindowWidth / 2 - kTextSize - static_cast<int>(kTextSize * 0.5f), Game::kWindowHeight / 4 - static_cast<int>(kTextSize * 0.5f), "ポーズ", 0xffffff, m_fontHandle);

	DrawStringToHandle(m_uiPos[static_cast<int>(e_Destination::InGame)].first - kTextFontSize * 3, m_uiPos[static_cast<int>(e_Destination::InGame)].second - kTextFontSize / 2, "ゲームに戻る", 0xffffff, m_textHandle);
	DrawStringToHandle(m_uiPos[static_cast<int>(e_Destination::Option)].first - kTextFontSize * 2 - kTextFontSize / 2, m_uiPos[static_cast<int>(e_Destination::Option)].second - kTextFontSize / 2, "オプション", 0xffffff, m_textHandle);
	DrawStringToHandle(m_uiPos[static_cast<int>(e_Destination::Title)].first - kTextFontSize * 3 - kTextFontSize / 2, m_uiPos[static_cast<int>(e_Destination::Title)].second - kTextFontSize / 2, "タイトルに戻る", 0xffffff, m_textHandle);

	DrawRotaGraph(m_uiPos[static_cast<int>(e_Destination::InGame)].first, m_uiPos[static_cast<int>(e_Destination::InGame)].second, 1.2f, 0.0f, m_handles[uiFrame], true);
	DrawRotaGraph(m_uiPos[static_cast<int>(e_Destination::Option)].first, m_uiPos[static_cast<int>(e_Destination::Option)].second, 1.2f, 0.0f, m_handles[uiFrame], true);
	DrawRotaGraph(m_uiPos[static_cast<int>(e_Destination::Title)].first, m_uiPos[static_cast<int>(e_Destination::Title)].second, 1.2f, 0.0f, m_handles[uiFrame], true);

#ifdef _DEBUG
	DrawFormatString(200, 0, 0xffffff, "ScenePause");

	DrawFormatString(200 - 16, 32 + static_cast<int>(m_destinationScene) * 16, 0xff0000, "→");

	DrawFormatString(200, 32 + static_cast<int>(e_Destination::InGame) * 16, 0xffffff, "SceneGame");
	DrawFormatString(200, 32 + static_cast<int>(e_Destination::Option) * 16, 0xffffff, "SceneOption");
	DrawFormatString(200, 32 + static_cast<int>(e_Destination::Title) * 16, 0xffffff, "SceneTitle");
#endif
	DrawFade();
}
