#include "ScenePause.h"
#include "SceneGame.h"
#include "SceneOption.h"
#include "SceneTitle.h"
#include "SceneManager.h"
#include "SoundManager.h"

#include "UICursor.h"
#include "Font.h"
#include "Game.h"

namespace
{
	//フォントのパス
	const char* kFontPath = "data/font/Dela-Gothic-One.ttf";
	const char* kFontName = "Dela Gothic One";

	constexpr int kTextFontSize = 50;
	constexpr int kTextSize = 64;

	constexpr int kUIPosX = 640;
	constexpr int kUIPosY[3] = {400,500,600};
}

/// <summary>
/// コンストラクタ
/// </summary>
/// <param name="mgr">シーン管理クラスのポインタ</param>
ScenePause::ScenePause(SceneManager& mgr) :
	SceneBase(mgr),
	m_destinationScene(eDestination::InGame),
	m_uiFrame(-1),
	m_textHandle(-1),
	m_fontHandle(-1)
{
	//フェードインをスキップする
	FadeInSkip();

	//カーソルの生成
	m_pCursor = std::make_shared<UICursor>();
	m_pCursor->Init();

	m_uiFrame = LoadGraph("data/img/ui_frame.png");

	m_textHandle = Font::GetInstance().GetFontHandle(kFontPath, kFontName, kTextFontSize);
	m_fontHandle = Font::GetInstance().GetFontHandle(kFontPath, kFontName, kTextSize);
}

/// <summary>
/// デストラクタ
/// </summary>
ScenePause::~ScenePause()
{
	DeleteGraph(m_uiFrame);
}

/// <summary>
/// 更新
/// </summary>
/// <param name="input">入力管理クラスの参照</param>
void ScenePause::Update(std::shared_ptr<Input>& input)
{
	UpdateFade();

	m_pCursor->Update(220, kUIPosY[static_cast<int>(m_destinationScene)]);


	if (!m_isChangeNextScene)
	{
		if (input->IsTriggered("UP"))
		{
			if (m_destinationScene != eDestination::InGame)
			{
				SoundManager::GetInstance().PlaySE("select");
				m_destinationScene = static_cast<eDestination>(static_cast<int>(m_destinationScene) - 1);
			}
		}

		if (input->IsTriggered("DOWN"))
		{
			if (m_destinationScene != eDestination::Title)
			{
				SoundManager::GetInstance().PlaySE("select");
				m_destinationScene = static_cast<eDestination>(static_cast<int>(m_destinationScene) + 1);
			}
		}

		if (input->IsTriggered("A"))
		{
			SoundManager::GetInstance().PlaySE("enter");

			if (m_destinationScene == eDestination::InGame)
			{
				m_manager.popScene();
			}

			if (m_destinationScene == eDestination::Option)
			{
				m_manager.PushScene(std::make_shared<SceneOption>(m_manager));
			}

			if (m_destinationScene == eDestination::Title)
			{
				m_isChangeNextScene = true;
				StartFadeOut();	// フェードアウト開始
			}
		}
	}

	if (m_isChangeNextScene)
	{
		if (!IsFadingOut())
		{
			if (m_destinationScene == eDestination::Title)
			{
				m_manager.ChangeAndClearScene(std::make_shared<SceneTitle>(m_manager));
			}
		}
	}
}

/// <summary>
/// 描画
/// </summary>
void ScenePause::Draw()
{
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 220);
	DrawBox(0, 0, Game::kWindowWidth, Game::kWindowHeight, 0x000000, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	m_pCursor->Draw();

	DrawStringToHandle(Game::kWindowWidth / 2 - kTextSize - static_cast<int>(kTextSize * 0.5f), Game::kWindowHeight / 4 - static_cast<int>(kTextSize * 0.5f), "ポーズ", 0xffffff, m_fontHandle);

	DrawStringToHandle(kUIPosX - kTextFontSize * 3, kUIPosY[static_cast<int>(eDestination::InGame)] - kTextFontSize / 2, "ゲームに戻る", 0xffffff, m_textHandle);
	DrawStringToHandle(kUIPosX - kTextFontSize * 2 - kTextFontSize / 2, kUIPosY[static_cast<int>(eDestination::Option)] - kTextFontSize / 2, "オプション", 0xffffff, m_textHandle);
	DrawStringToHandle(kUIPosX - kTextFontSize * 3 - kTextFontSize / 2, kUIPosY[static_cast<int>(eDestination::Title)] - kTextFontSize / 2, "タイトルに戻る", 0xffffff, m_textHandle);

	DrawRotaGraph(kUIPosX, kUIPosY[static_cast<int>(eDestination::InGame)], 1.2f, 0.0f, m_uiFrame, true);
	DrawRotaGraph(kUIPosX, kUIPosY[static_cast<int>(eDestination::Option)], 1.2f, 0.0f, m_uiFrame, true);
	DrawRotaGraph(kUIPosX, kUIPosY[static_cast<int>(eDestination::Title)], 1.2f, 0.0f, m_uiFrame, true);

#ifdef _DEBUG
	DrawFormatString(200, 0, 0xffffff, "ScenePause");

	DrawFormatString(200 - 16, 32 + static_cast<int>(m_destinationScene) * 16, 0xff0000, "→");

	DrawFormatString(200, 32 + static_cast<int>(eDestination::InGame) * 16, 0xffffff, "SceneGame");
	DrawFormatString(200, 32 + static_cast<int>(eDestination::Option) * 16, 0xffffff, "SceneOption");
	DrawFormatString(200, 32 + static_cast<int>(eDestination::Title) * 16, 0xffffff, "SceneTitle");
#endif
	DrawFade();
}
