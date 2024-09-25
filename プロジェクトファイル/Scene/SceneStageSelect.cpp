#include "SceneStageSelect.h"
#include "SceneTitle.h"
#include "SceneUpgrade.h"
#include "SceneGame.h"
#include "SceneManager.h"
#include "ScoreManager.h"
#include "SoundManager.h"

#include "TitlePlayer.h"
#include "Font.h"


namespace
{
	enum handle
	{
		BGmodel = 0,
		uiFrame,
		rightArrow,
		leftArrow,
		scoreBg,
		stage1MiniMap,
		stage2MiniMap,
		stage3MiniMap,
	};

	//フォントのパス
	const char* kFontPath = "data/font/Dela-Gothic-One.ttf";
	const char* kFontName = "Dela Gothic One";

	constexpr float kCameraNear = 10.5f;
	constexpr float kCameraFar = 1000.0f;

	//ミニマップのパス
	const std::string kMiniMapFrontName = "data/img/miniMap_stage";
	const std::string kMiniMapBackName = ".png";
}


SceneStageSelect::SceneStageSelect(SceneManager& mgr):
	SceneBase(mgr),
	m_angle(0),
	m_cursorOffsetX(0)
{
	m_destinationScene = e_Destination::stage1;

	m_textHandle = Font::GetInstance().GetFontHandle(kFontPath, kFontName, 30);
	m_fontHandle = Font::GetInstance().GetFontHandle(kFontPath, kFontName, 50);

	m_handles.push_back(MV1LoadModel("data/model/title.mv1"));
	m_handles.push_back(LoadGraph("data/img/ui_frame.png"));
	m_handles.push_back(LoadGraph("data/img/ArrowRight.png"));
	m_handles.push_back(LoadGraph("data/img/ArrowLeft.png"));
	m_handles.push_back(LoadGraph("data/img/ui_big.png"));

	for (int i = 1; i <= 3; i++)
	{
		auto str = (kMiniMapFrontName + std::to_string(i) + kMiniMapBackName).c_str();
		m_handles.push_back(LoadGraph(str));
	}


	m_uiPos.push_back(std::pair<int, int>(140, 680));
	m_uiPos.push_back(std::pair<int, int>(440, 240));
	m_uiPos.push_back(std::pair<int, int>(440, 360));
	m_uiPos.push_back(std::pair<int, int>(440, 480));

	MV1SetScale(m_handles[BGmodel], VECTOR(0.1f, 0.1f, 0.1f));
	MV1SetPosition(m_handles[BGmodel], VECTOR(0.0f, 0.0f, 0.0f));

	m_player = std::make_shared<TitlePlayer>();
	m_player->Init(MyLib::Vec3(-260.0f, 30.0f, 20.0f));

	SetCameraNearFar(kCameraNear, kCameraFar);
	SetCameraPositionAndTarget_UpVecY(VECTOR(0.0f, 180.0f, 400.0f), VECTOR(0.0f, 160.0f, 0.0f));

	m_lightHandle = CreatePointLightHandle(VGet(0.0f, 0.0f, 400.0), 1000.0f, 0.08f, 0.003f, 0.0f);
	SetLightEnable(FALSE);

	//念のためロード
	ScoreManager::GetInstance().Load();
	m_stage1Score = ScoreManager::GetInstance().GetStage1Score();
	m_stage2Score = ScoreManager::GetInstance().GetStage2Score();
	m_stage3Score = ScoreManager::GetInstance().GetStage3Score();
}

SceneStageSelect::~SceneStageSelect()
{

	for (int i = 0; i < m_handles.size(); i++)
	{
		if (i != BGmodel)
		{
			DeleteGraph(m_handles[i]);
		}
		else
		{
			MV1DeleteModel(m_handles[i]);
		}
	}

	m_handles.clear();

	DeleteLightHandle(m_lightHandle);
}

void SceneStageSelect::Update(std::shared_ptr<Input>& input)
{
	UpdateFade();

	SoundManager::GetInstance().PlayBGM("titleBgm", true);

	m_player->Update();

	m_angle += 0.14f;
	m_cursorOffsetX = static_cast<int>(sinf(m_angle) * 8);

	if (!m_isToNextScene)
	{
		if (input->IsTriggered("RIGHT"))
		{

			if (m_destinationScene == e_Destination::Title)
			{
				SoundManager::GetInstance().PlaySE("select");
				m_destinationScene = e_Destination::stage1;
			}
			//else
			//{
			//	m_destinationScene = e_Destination::Upgrade;
			//}

		}

		if (input->IsTriggered("DOWN"))
		{
			if (m_destinationScene != e_Destination::Title && m_destinationScene != e_Destination::Upgrade && m_destinationScene != e_Destination::stage3)
			{
				SoundManager::GetInstance().PlaySE("select");
				m_destinationScene = static_cast<e_Destination>(static_cast<int>(m_destinationScene) + 1);
			}
		}

		if (input->IsTriggered("UP"))
		{
			if (m_destinationScene != e_Destination::Title && m_destinationScene != e_Destination::Upgrade && m_destinationScene != e_Destination::stage1)
			{
				SoundManager::GetInstance().PlaySE("select");
				m_destinationScene = static_cast<e_Destination>(static_cast<int>(m_destinationScene) - 1);
			}
		}

		if (input->IsTriggered("LEFT"))
		{

			//if (m_destinationScene == e_Destination::Upgrade)
			//{
			//	m_destinationScene = e_Destination::stage1;
			//}
			//else
			{
				SoundManager::GetInstance().PlaySE("select");
				m_destinationScene = e_Destination::Title;

			}
		}

		if (input->IsTriggered("A"))
		{
			SoundManager::GetInstance().PlaySE("enter");

			if (m_destinationScene == e_Destination::Title)
			{
				m_isToNextScene = true;
				StartFadeOut();	// フェードアウト開始
			}

			//if (m_destinationScene == e_Destination::Upgrade)
			//{
			//	m_manager.PushScene(std::make_shared<SceneUpgrade>(m_manager));
			//	return;
			//}

			if (m_destinationScene == e_Destination::stage1)
			{
				m_isToNextScene = true;
				StartFadeOut();	// フェードアウト開始
			}

			if (m_destinationScene == e_Destination::stage2)
			{
				m_isToNextScene = true;
				StartFadeOut();	// フェードアウト開始
			}

			if (m_destinationScene == e_Destination::stage3)
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
				m_manager.ChangeScene(std::make_shared<SceneTitle>(m_manager));
				return;
			}

			if (m_destinationScene == e_Destination::stage1)
			{
				m_manager.SetStageName("stage1");
				m_manager.ChangeScene(std::make_shared<SceneGame>(m_manager));
				return;
			}
			if (m_destinationScene == e_Destination::stage2)
			{
				m_manager.SetStageName("stage2");
				m_manager.ChangeScene(std::make_shared<SceneGame>(m_manager));
				return;
			}
			if (m_destinationScene == e_Destination::stage3)
			{
				m_manager.SetStageName("stage3");
				m_manager.ChangeScene(std::make_shared<SceneGame>(m_manager));
				return;
			}
		}
	}

}

void SceneStageSelect::Draw()
{
	m_player->Draw();
	MV1DrawModel(m_handles[BGmodel]);

	int rightCursolX = m_uiPos[static_cast<int>(m_destinationScene)].first +120;
	int leftCursolX = m_uiPos[static_cast<int>(m_destinationScene)].first - 120;
	int cursorPosY = m_uiPos[static_cast<int>(m_destinationScene)].second;
	if (m_destinationScene != e_Destination::Title)
	{
		rightCursolX += 100;
		leftCursolX -= 100;
	}

	if (m_destinationScene == e_Destination::stage1)
	{
		DrawRotaGraph(850, 360, 1.0f, 0.0f, m_handles[scoreBg], true);

		DrawRotaGraph(850, 300, 1.6f, 0.0f, m_handles[stage1MiniMap], true);

		DrawStringToHandle(775, 465, "ハイスコア",0xffffff, m_textHandle);
		DrawFormatStringToHandle(765, 500, 0xffffff, m_fontHandle, "%d", m_stage1Score);
	}
	else if (m_destinationScene == e_Destination::stage2)
	{
		DrawRotaGraph(850, 360, 1.0f, 0.0f, m_handles[scoreBg], true);
		DrawRotaGraph(850, 300, 1.6f, 0.0f, m_handles[stage2MiniMap], true);

		DrawStringToHandle(775, 465, "ハイスコア", 0xffffff, m_textHandle);
		DrawFormatStringToHandle(765, 500, 0xffffff, m_fontHandle, "%d", m_stage2Score);
	}
	else if (m_destinationScene == e_Destination::stage3)
	{
		DrawRotaGraph(850, 360, 1.0f, 0.0f, m_handles[scoreBg], true);
		DrawRotaGraph(850, 300, 1.6f, 0.0f, m_handles[stage3MiniMap], true);

		DrawStringToHandle(775, 465, "ハイスコア", 0xffffff, m_textHandle);
		DrawFormatStringToHandle(765, 500, 0xffffff, m_fontHandle, "%d", m_stage3Score);
	}



	DrawRotaGraph(rightCursolX + m_cursorOffsetX, cursorPosY, 1.0f, 0.0f, m_handles[leftArrow], true);
	DrawRotaGraph(leftCursolX - m_cursorOffsetX, cursorPosY, 1.0f, 0.0f, m_handles[rightArrow], true);

	DrawRotaGraph(m_uiPos[static_cast<int>(e_Destination::Title)].first, m_uiPos[static_cast<int>(e_Destination::Title)].second, 0.6f, 0.0f, m_handles[uiFrame], true);
	DrawRotaGraph(m_uiPos[static_cast<int>(e_Destination::stage1)].first, m_uiPos[static_cast<int>(e_Destination::stage1)].second, 1.2f, 0.0f, m_handles[uiFrame], true);
	DrawRotaGraph(m_uiPos[static_cast<int>(e_Destination::stage2)].first, m_uiPos[static_cast<int>(e_Destination::stage2)].second, 1.2f, 0.0f, m_handles[uiFrame], true);
	DrawRotaGraph(m_uiPos[static_cast<int>(e_Destination::stage3)].first, m_uiPos[static_cast<int>(e_Destination::stage3)].second, 1.2f, 0.0f, m_handles[uiFrame], true);

	DrawStringToHandle(m_uiPos[static_cast<int>(e_Destination::Title)].first-45, m_uiPos[static_cast<int>(e_Destination::Title)].second-15 , "もどる", 0xffffff, m_textHandle);
	DrawStringToHandle(m_uiPos[static_cast<int>(e_Destination::stage1)].first-110, m_uiPos[static_cast<int>(e_Destination::stage1)].second-25 , "ステージ1", 0xffffff, m_fontHandle);
	DrawStringToHandle(m_uiPos[static_cast<int>(e_Destination::stage2)].first-120, m_uiPos[static_cast<int>(e_Destination::stage2)].second-25 , "ステージ2", 0xffffff, m_fontHandle);
	DrawStringToHandle(m_uiPos[static_cast<int>(e_Destination::stage3)].first-120, m_uiPos[static_cast<int>(e_Destination::stage3)].second-25 , "ステージ3", 0xffffff, m_fontHandle);


#ifdef _DEBUG
	DrawFormatString(0, 0, 0xffffff, "SceneStageSelect");

	DrawFormatString(64 / 2, 32 + static_cast<int>(m_destinationScene) * 16, 0xff0000, "→");

	DrawFormatString(64, 32 + static_cast<int>(e_Destination::Title) * 16, 0xffffff, "SceneTitle");
	DrawFormatString(64, 32 + static_cast<int>(e_Destination::stage1) * 16, 0xffffff, "Stage1");
	DrawFormatString(64, 32 + static_cast<int>(e_Destination::stage2) * 16, 0xffffff, "Stage2");
	DrawFormatString(64, 32 + static_cast<int>(e_Destination::stage3) * 16, 0xffffff, "Stage3");
	DrawFormatString(64, 32 + static_cast<int>(e_Destination::Upgrade) * 16, 0xffffff, "SceneUpgrade");
#endif
	DrawFade();
}