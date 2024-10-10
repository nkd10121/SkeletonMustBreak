#include "SceneStageSelect.h"
#include "SceneTitle.h"
#include "SceneUpgrade.h"
#include "SceneGame.h"
#include "SceneManager.h"
#include "ScoreManager.h"
#include "SoundManager.h"
#include "ModelManager.h"

#include "TitlePlayer.h"
#include "UICursor.h"
#include "Font.h"


namespace
{
	enum handle
	{
		BGmodel = 0,
		uiFrame,
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

/// <summary>
/// コンストラクタ
/// </summary>
/// <param name="mgr">シーン管理クラスの参照</param>
SceneStageSelect::SceneStageSelect(SceneManager& mgr) :
	SceneBase(mgr),
	m_pCursor(),
	m_destinationScene(eDestination::stage1),
	m_UIHandles(),
	m_lightHandle(-1),
	m_pPlayer()
{
	//カーソルの生成
	m_pCursor = std::make_shared<UICursor>();
	m_pCursor->Init();

	m_UIHandles.push_back(ModelManager::GetInstance().GetModelHandle("data/model/title.mv1"));
	m_UIHandles.push_back(LoadGraph("data/img/ui_frame.png"));
	m_UIHandles.push_back(LoadGraph("data/img/ui_big.png"));

	for (int i = 1; i <= 3; i++)
	{
		auto num = std::to_string(i);
		auto path = (kMiniMapFrontName + num + kMiniMapBackName);
		m_UIHandles.push_back(LoadGraph(path.c_str()));
	}

	m_uiPos.push_back(std::pair<int, int>(140, 680));
	m_uiPos.push_back(std::pair<int, int>(440, 240));
	m_uiPos.push_back(std::pair<int, int>(440, 360));
	m_uiPos.push_back(std::pair<int, int>(440, 480));

	MV1SetScale(m_UIHandles[BGmodel], VECTOR(0.1f, 0.1f, 0.1f));
	MV1SetPosition(m_UIHandles[BGmodel], VECTOR(0.0f, 0.0f, 0.0f));

	m_pPlayer = std::make_shared<TitlePlayer>();
	m_pPlayer->Init(MyLib::Vec3(-260.0f, 30.0f, 20.0f));

	SetCameraNearFar(kCameraNear, kCameraFar);
	SetCameraPositionAndTarget_UpVecY(VECTOR(0.0f, 180.0f, 400.0f), VECTOR(0.0f, 160.0f, 0.0f));

	m_lightHandle = CreatePointLightHandle(VGet(0.0f, 0.0f, 400.0), 1000.0f, 0.08f, 0.003f, 0.0f);
	SetLightEnable(FALSE);

	//念のためロード
	ScoreManager::GetInstance().Load();

	Font::GetInstance().GetFontHandle(kFontPath, kFontName, 30);
}

/// <summary>
/// デストラクタ
/// </summary>
SceneStageSelect::~SceneStageSelect()
{

	for (int i = 0; i < m_UIHandles.size(); i++)
	{
		if (i != BGmodel)
		{
			DeleteGraph(m_UIHandles[i]);
		}
		else
		{
			MV1DeleteModel(m_UIHandles[i]);
		}
	}

	m_UIHandles.clear();

	DeleteLightHandle(m_lightHandle);
}

/// <summary>
/// 更新
/// </summary>
/// <param name="input">入力管理クラスの参照</param>
void SceneStageSelect::Update(std::shared_ptr<Input>& input)
{
	UpdateFade();

	SoundManager::GetInstance().PlayBGM("titleBgm", true);

	m_pPlayer->Update();
	
	int offset = 220;
	if (m_destinationScene == eDestination::Title)
	{
		offset -= 100;
	}

	m_pCursor->Update(offset, m_uiPos[static_cast<int>(m_destinationScene)].second, m_uiPos[static_cast<int>(m_destinationScene)].first);

	if (!m_isChangeNextScene)
	{
		if (input->IsTriggered("RIGHT"))
		{
			if (m_destinationScene == eDestination::Title)
			{
				SoundManager::GetInstance().PlaySE("select");
				m_destinationScene = eDestination::stage1;
			}
		}

		if (input->IsTriggered("DOWN"))
		{
			if (m_destinationScene != eDestination::Title && m_destinationScene != eDestination::Upgrade && m_destinationScene != eDestination::stage3)
			{
				SoundManager::GetInstance().PlaySE("select");
				m_destinationScene = static_cast<eDestination>(static_cast<int>(m_destinationScene) + 1);
			}
		}

		if (input->IsTriggered("UP"))
		{
			if (m_destinationScene != eDestination::Title && m_destinationScene != eDestination::Upgrade && m_destinationScene != eDestination::stage1)
			{
				SoundManager::GetInstance().PlaySE("select");
				m_destinationScene = static_cast<eDestination>(static_cast<int>(m_destinationScene) - 1);
			}
		}

		if (input->IsTriggered("LEFT"))
		{
			SoundManager::GetInstance().PlaySE("select");
			m_destinationScene = eDestination::Title;
		}

		if (input->IsTriggered("A"))
		{
			SoundManager::GetInstance().PlaySE("enter");

			if (m_destinationScene == eDestination::Title)
			{
				m_isChangeNextScene = true;
				StartFadeOut();	// フェードアウト開始
			}

			if (m_destinationScene == eDestination::stage1)
			{
				m_isChangeNextScene = true;
				StartFadeOut();	// フェードアウト開始
			}

			if (m_destinationScene == eDestination::stage2)
			{
				m_isChangeNextScene = true;
				StartFadeOut();	// フェードアウト開始
			}

			if (m_destinationScene == eDestination::stage3)
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
				m_manager.ChangeScene(std::make_shared<SceneTitle>(m_manager));
				return;
			}

			if (m_destinationScene == eDestination::stage1)
			{
				m_manager.SetStageName("stage1");
				m_manager.ChangeScene(std::make_shared<SceneGame>(m_manager));
				return;
			}
			if (m_destinationScene == eDestination::stage2)
			{
				m_manager.SetStageName("stage2");
				m_manager.ChangeScene(std::make_shared<SceneGame>(m_manager));
				return;
			}
			if (m_destinationScene == eDestination::stage3)
			{
				m_manager.SetStageName("stage3");
				m_manager.ChangeScene(std::make_shared<SceneGame>(m_manager));
				return;
			}
		}
	}

}

/// <summary>
/// 描画
/// </summary>
void SceneStageSelect::Draw()
{
	m_pPlayer->Draw();
	MV1DrawModel(m_UIHandles[BGmodel]);

	if (m_destinationScene != eDestination::Title)
	{
		//背景の描画
		DrawRotaGraph(850, 360, 1.0f, 0.0f, m_UIHandles[scoreBg], true);
		Font::GetInstance().DrawStringUseFont(850, 465, "ハイスコア", 0xffffff, kFontName, 30);
	}


	if (m_destinationScene == eDestination::stage1)
	{
		DrawRotaGraph(850, 300, 1.6f, 0.0f, m_UIHandles[stage1MiniMap], true);

		DrawFormatStringToHandle(765, 500, 0xffffff, Font::GetInstance().GetFontHandle(kFontPath, kFontName, 50), "%d", ScoreManager::GetInstance().GetStage1Score());
	}
	else if (m_destinationScene == eDestination::stage2)
	{
		DrawRotaGraph(850, 300, 1.6f, 0.0f, m_UIHandles[stage2MiniMap], true);

		DrawFormatStringToHandle(765, 500, 0xffffff, Font::GetInstance().GetFontHandle(kFontPath, kFontName, 50), "%d", ScoreManager::GetInstance().GetStage2Score());
	}
	else if (m_destinationScene == eDestination::stage3)
	{
		DrawRotaGraph(850, 300, 1.6f, 0.0f, m_UIHandles[stage3MiniMap], true);

		DrawFormatStringToHandle(765, 500, 0xffffff, Font::GetInstance().GetFontHandle(kFontPath, kFontName, 50), "%d", ScoreManager::GetInstance().GetStage3Score());
	}

	m_pCursor->Draw();

	DrawRotaGraph(m_uiPos[static_cast<int>(eDestination::Title)].first, m_uiPos[static_cast<int>(eDestination::Title)].second, 0.6f, 0.0f, m_UIHandles[uiFrame], true);
	DrawRotaGraph(m_uiPos[static_cast<int>(eDestination::stage1)].first, m_uiPos[static_cast<int>(eDestination::stage1)].second, 1.2f, 0.0f, m_UIHandles[uiFrame], true);
	DrawRotaGraph(m_uiPos[static_cast<int>(eDestination::stage2)].first, m_uiPos[static_cast<int>(eDestination::stage2)].second, 1.2f, 0.0f, m_UIHandles[uiFrame], true);
	DrawRotaGraph(m_uiPos[static_cast<int>(eDestination::stage3)].first, m_uiPos[static_cast<int>(eDestination::stage3)].second, 1.2f, 0.0f, m_UIHandles[uiFrame], true);

	Font::GetInstance().DrawStringUseFont(m_uiPos[static_cast<int>(eDestination::Title)].first, m_uiPos[static_cast<int>(eDestination::Title)].second, "もどる", 0xffffff, kFontName, 30);
	Font::GetInstance().DrawStringUseFont(m_uiPos[static_cast<int>(eDestination::stage1)].first, m_uiPos[static_cast<int>(eDestination::stage1)].second, "ステージ1", 0xffffff, kFontName, 50);
	Font::GetInstance().DrawStringUseFont(m_uiPos[static_cast<int>(eDestination::stage2)].first, m_uiPos[static_cast<int>(eDestination::stage2)].second, "ステージ2", 0xffffff, kFontName, 50);
	Font::GetInstance().DrawStringUseFont(m_uiPos[static_cast<int>(eDestination::stage3)].first, m_uiPos[static_cast<int>(eDestination::stage3)].second, "ステージ3", 0xffffff, kFontName, 50);


#ifdef _DEBUG
	DrawFormatString(0, 0, 0xffffff, "SceneStageSelect");

	DrawFormatString(64 / 2, 32 + static_cast<int>(m_destinationScene) * 16, 0xff0000, "→");

	DrawFormatString(64, 32 + static_cast<int>(eDestination::Title) * 16, 0xffffff, "SceneTitle");
	DrawFormatString(64, 32 + static_cast<int>(eDestination::stage1) * 16, 0xffffff, "Stage1");
	DrawFormatString(64, 32 + static_cast<int>(eDestination::stage2) * 16, 0xffffff, "Stage2");
	DrawFormatString(64, 32 + static_cast<int>(eDestination::stage3) * 16, 0xffffff, "Stage3");
	DrawFormatString(64, 32 + static_cast<int>(eDestination::Upgrade) * 16, 0xffffff, "SceneUpgrade");
#endif
	DrawFade();
}