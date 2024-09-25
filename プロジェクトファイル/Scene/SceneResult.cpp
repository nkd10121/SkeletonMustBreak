#include "SceneResult.h"
#include "SceneTitle.h"
#include "SceneGame.h"
#include "SceneUpgrade.h"
#include "SceneStageSelect.h"
#include "SceneManager.h"
#include "ScoreManager.h"
#include "SoundManager.h"

#include "Font.h"
#include "Game.h"

namespace
{
	//フォントのパス
	const char* kFontPath = "data/font/Dela-Gothic-One.ttf";
	const char* kFontName = "Dela Gothic One";
	//フォントサイズ
	constexpr int kFontSize = 64;
	constexpr int kTextSize = 42;
	constexpr int kDetailSize = 28;


	//アルファ値の最大
	constexpr int kAlphaMax = 220;
	//フェードアウトにかかるフレーム数
	constexpr int kFadeFrame = 40;


	enum ui
	{
		uiFrame,
		rightArrow,
		leftArrow,
		uiBg
	};
}

SceneResult::SceneResult(SceneManager& mgr) :
	SceneBase(mgr),
	m_frame(0),
	m_hp(0),
	m_playerKillCount(0),
	m_trapKillCount(0),
	m_combo(0),
	m_min(0),
	m_sec1(),
	m_sec2(),
	m_score(0),
	m_isClear(false),
	m_angle(0),
	m_cursorOffsetX(0),
	m_alpha(0)
{
	m_destinationScene = e_Destination::InGame;

	m_isClear = mgr.GetIsClear();



	m_fontHandle = Font::GetInstance().GetFontHandle(kFontPath, kFontName, kFontSize);
	m_textHandle = Font::GetInstance().GetFontHandle(kFontPath, kFontName, kTextSize);
	m_detailHandle = Font::GetInstance().GetFontHandle(kFontPath, kFontName, kDetailSize);

	m_handles.push_back(LoadGraph("data/img/ui_frame.png"));

	m_handles.push_back(LoadGraph("data/img/ArrowRight.png"));
	m_handles.push_back(LoadGraph("data/img/ArrowLeft.png"));
	m_handles.push_back(LoadGraph("data/img/ui_big.png"));


	m_uiPos.push_back(std::pair<int, int>(640, 420));
	m_uiPos.push_back(std::pair<int, int>(640, 500));
	m_uiPos.push_back(std::pair<int, int>(640, 580));
	m_uiPos.push_back(std::pair<int, int>(640, 660));

	m_updateFunc = &SceneResult::FadeOutUpdate;
	m_drawFunc = &SceneResult::FadeOutDraw;

	//スコア計算に必要な情報を取得
	m_scoreInfo = mgr.GetScoreInfoPtr();
	GetScore();
	mgr.ResetScoreInfo();

	FrameToTime();
	CalculationScore();

	//シーン遷移のフェードインを飛ばす
	FadeInSkip();


	auto name = mgr.GetStageName();
	if (name == "stage1")
	{
		if (ScoreManager::GetInstance().GetStage1Score() < m_score)
		{
			ScoreManager::GetInstance().SetStage1Score(m_score);
		}
	}
	else if (name == "stage2")
	{
		if (ScoreManager::GetInstance().GetStage2Score() < m_score)
		{
			ScoreManager::GetInstance().SetStage2Score(m_score);
		}
	}
	else if (name == "stage3")
	{
		if (ScoreManager::GetInstance().GetStage3Score() < m_score)
		{
			ScoreManager::GetInstance().SetStage3Score(m_score);
		}
	}

	SoundManager::GetInstance().StopBGM("inGameBgm");
	SoundManager::GetInstance().Load("gameover", "data/sound/se/gameover.mp3", false);

	if (!m_isClear)
	{
		SoundManager::GetInstance().PlaySE("gameover");
	}
}

SceneResult::~SceneResult()
{
	for (auto& handle : m_handles)
	{
		DeleteGraph(handle);
	}
}

void SceneResult::Update(std::shared_ptr<Input>& input)
{
	UpdateFade();

	//状態の更新
	(this->*m_updateFunc)(input);

	m_angle += 0.14f;
	m_cursorOffsetX = static_cast<int>(sinf(m_angle) * 8);


}

void SceneResult::Draw()
{
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, m_alpha);
	DrawBox(0, 0, Game::kWindowWidth, Game::kWindowHeight, 0x000000, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	//状態の更新
	(this->*m_drawFunc)();

#ifdef _DEBUG
	DrawFormatString(200, 0, 0xffffff, "SceneResult");
#endif
	DrawFade();
}

void SceneResult::GetScore()
{
	m_frame = m_scoreInfo->frame;
	m_hp = m_scoreInfo->crystalHp;
	m_playerKillCount = m_scoreInfo->KilledcounterByPlayer;
	m_trapKillCount = m_scoreInfo->KilledcounterByTrap;
	m_combo = m_scoreInfo->MaxCombo;
}

void SceneResult::FrameToTime()
{
	m_min = m_frame / 3600;

	int s = (m_frame % 3600) / 60;
	std::string sec = std::to_string(s);
	if (sec.size() == 1)
	{
		sec = "0" + sec;
	}

	m_sec1 = sec.at(0);
	m_sec2 = sec.at(1);
}

void SceneResult::CalculationScore()
{
	//秒数を出す
	int s = (m_frame % 3600) / 60;
	int allSec = m_min * 60 + s;

	//一応4分を目標タイムとする
	int timeScore = (4 * 60)- allSec;
	if (timeScore < 0)
	{
		timeScore = 0;
	}

	//スコアを計算する
	m_score += timeScore;
	m_score += m_hp * 100;
	m_score += m_playerKillCount * 50;
	m_score += m_trapKillCount * 100;
	m_score += m_combo * 100;
}

void SceneResult::FadeOutUpdate(std::shared_ptr<Input> input)
{
	m_alpha += static_cast<int>(kAlphaMax / kFadeFrame);

	if (m_alpha >= kAlphaMax)
	{
		m_updateFunc = &SceneResult::NormalUpdate;
		m_drawFunc = &SceneResult::NormalDraw;
	}
}

void SceneResult::NormalUpdate(std::shared_ptr<Input> input)
{
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
			if (m_destinationScene != e_Destination::StageSelect)
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
				m_isToNextScene = true;
				StartFadeOut();	// フェードアウト開始
			}

			if (m_destinationScene == e_Destination::details)
			{
				m_updateFunc = &SceneResult::DetailUpdate;
				m_drawFunc = &SceneResult::DetailDraw;
			}

			//if (m_destinationScene == e_Destination::Upgrade)
			//{
			//	m_manager.PushScene(std::make_shared<SceneUpgrade>(m_manager));
			//}

			if (m_destinationScene == e_Destination::StageSelect)
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
			if (m_destinationScene == e_Destination::InGame)
			{
				m_manager.ChangeAndClearScene(std::make_shared<SceneGame>(m_manager));
			}
			if (m_destinationScene == e_Destination::StageSelect)
			{
				m_manager.ChangeAndClearScene(std::make_shared<SceneStageSelect>(m_manager));
			}
		}
	}
}

void SceneResult::DetailUpdate(std::shared_ptr<Input> input)
{
	if (input->IsTriggered("A"))
	{
		m_updateFunc = &SceneResult::NormalUpdate;
		m_drawFunc = &SceneResult::NormalDraw;
	}
}

void SceneResult::FadeOutDraw()
{
	//特に描画する予定なし
}

void SceneResult::NormalDraw()
{
	if (m_isClear)
	{
		int x = 640 - 64 * 2 - 64 / 2;
		DrawStringToHandle(x, 200, "CLEAR!", 0xffffff, m_fontHandle);
	}
	else if (!m_isClear)
	{
		int x = 640 - 64 * 4;
		DrawStringToHandle(x, 200, "GAMEOVER...", 0xffffff, m_fontHandle);
	}

	DrawRotaGraph(m_uiPos[static_cast<int>(e_Destination::InGame)].first, m_uiPos[static_cast<int>(e_Destination::InGame)].second, 1.1f, 0.0f, m_handles[uiFrame], true);
	DrawRotaGraph(m_uiPos[static_cast<int>(e_Destination::details)].first, m_uiPos[static_cast<int>(e_Destination::details)].second, 1.1f, 0.0f, m_handles[uiFrame], true);
	//DrawRotaGraph(m_uiPos[static_cast<int>(e_Destination::Upgrade)].first, m_uiPos[static_cast<int>(e_Destination::Upgrade)].second, 1.1f, 0.0f, m_handles[uiFrame], true);
	DrawRotaGraph(m_uiPos[static_cast<int>(e_Destination::StageSelect)].first, m_uiPos[static_cast<int>(e_Destination::StageSelect)].second, 1.1f, 0.0f, m_handles[uiFrame], true);

	DrawStringToHandle(m_uiPos[static_cast<int>(e_Destination::InGame)].first - kTextSize*2 - kTextSize / 2, m_uiPos[static_cast<int>(e_Destination::InGame)].second - 25, "もういちど", 0xffffff, m_textHandle);
	DrawStringToHandle(m_uiPos[static_cast<int>(e_Destination::details)].first - kTextSize * 3, m_uiPos[static_cast<int>(e_Destination::details)].second - 25, "プレイデータ", 0xffffff, m_textHandle);
	//DrawStringToHandle(m_uiPos[static_cast<int>(e_Destination::Upgrade)].first - kTextSize, m_uiPos[static_cast<int>(e_Destination::Upgrade)].second - 25, "強化", 0xffffff, m_textHandle);
	DrawStringToHandle(m_uiPos[static_cast<int>(e_Destination::StageSelect)].first - kTextSize * 3 - kTextSize / 2, m_uiPos[static_cast<int>(e_Destination::StageSelect)].second - 25, "ステージ選択へ", 0xffffff, m_textHandle);

	DrawRotaGraph(m_uiPos[static_cast<int>(m_destinationScene)].first + 220 + m_cursorOffsetX, m_uiPos[static_cast<int>(m_destinationScene)].second, 1.0f, 0.0f, m_handles[leftArrow], true);
	DrawRotaGraph(m_uiPos[static_cast<int>(m_destinationScene)].first - 220 - m_cursorOffsetX, m_uiPos[static_cast<int>(m_destinationScene)].second, 1.0f, 0.0f, m_handles[rightArrow], true);

#ifdef _DEBUG
	DrawFormatString(200 - 16, 32 + static_cast<int>(m_destinationScene) * 16, 0xff0000, "→");

	DrawFormatString(200, 32 + static_cast<int>(e_Destination::InGame) * 16, 0xffffff, "SceneGame");
	DrawFormatString(200, 32 + static_cast<int>(e_Destination::details) * 16, 0xffffff, "ShowDetail");
	//DrawFormatString(200, 32 + static_cast<int>(e_Destination::Upgrade) * 16, 0xffffff, "SceneUpgrade");
	DrawFormatString(200, 32 + static_cast<int>(e_Destination::StageSelect) * 16, 0xffffff, "SceneStageSelect");
#endif
}

void SceneResult::DetailDraw()
{

	DrawRotaGraph(680, 360, 1.5f, 0.0f, m_handles[uiBg], true);
	DrawStringToHandle(580 - kFontSize - kFontSize / 2, 80, "スコア:", 0xffffff, m_fontHandle);
	DrawFormatStringToHandle(720, 80, 0xffffff, m_fontHandle, "%d",m_score);

	DrawStringToHandle(480 - kTextSize - kTextSize/2, 200, "タイム                    :",0xffffff, m_textHandle);
	DrawStringToHandle(560, 200 + kDetailSize/2, "目標4:00",0xffffff, m_detailHandle);
	DrawStringToHandle(600 - kTextSize*3 - kTextSize/2, 300, "クリスタルHP:",0xffffff, m_textHandle);
	DrawStringToHandle(600 - kTextSize*4/* - kFontSize/2*/, 400, "プレイヤーキル数:",0xffffff, m_textHandle);
	DrawStringToHandle(600 - kTextSize*3 - kTextSize/2, 500, "トラップキル数:",0xffffff, m_textHandle);
	DrawStringToHandle(600 - kTextSize*3, 600, "最大コンボ数:",0xffffff, m_textHandle);

	DrawFormatStringToHandle(800, 200, 0xffffff, m_textHandle, "%d:%c%c", m_min, m_sec1, m_sec2);
	DrawFormatStringToHandle(860, 300, 0xffffff, m_textHandle,"%d",m_scoreInfo->crystalHp);
	DrawFormatStringToHandle(860, 400, 0xffffff, m_textHandle,"%d",m_scoreInfo->KilledcounterByPlayer);
	DrawFormatStringToHandle(860, 500, 0xffffff, m_textHandle,"%d",m_scoreInfo->KilledcounterByTrap);
	DrawFormatStringToHandle(860, 600, 0xffffff, m_textHandle,"%d",m_scoreInfo->MaxCombo);
}
