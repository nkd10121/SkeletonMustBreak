#include "SceneStageSelect.h"
#include "SceneTitle.h"
#include "SceneUpgrade.h"
#include "SceneGame.h"
#include "SceneManager.h"

#include "TitlePlayer.h"

namespace
{
	enum handle
	{
		BGmodel = 0,
	};

	constexpr float kCameraNear = 10.5f;
	constexpr float kCameraFar = 1000.0f;
}


SceneStageSelect::SceneStageSelect(SceneManager& mgr):
	SceneBase(mgr)
{
	m_destinationScene = e_Destination::Stage1;

	m_handles.push_back(MV1LoadModel("data/model/title.mv1"));

	MV1SetScale(m_handles[BGmodel], VECTOR(0.1f, 0.1f, 0.1f));
	MV1SetPosition(m_handles[BGmodel], VECTOR(0.0f, 0.0f, 0.0f));

	m_player = std::make_shared<TitlePlayer>();
	m_player->Init(MyLib::Vec3(-260.0f, 30.0f, 20.0f));

	SetCameraNearFar(kCameraNear, kCameraFar);
	SetCameraPositionAndTarget_UpVecY(VECTOR(0.0f, 180.0f, 400.0f), VECTOR(0.0f, 160.0f, 0.0f));

	m_lightHandle = CreatePointLightHandle(VGet(0.0f, 0.0f, 400.0), 1000.0f, 0.08f, 0.003f, 0.0f);
	SetLightEnable(FALSE);
}

SceneStageSelect::~SceneStageSelect()
{
	for (int i = 0; i < m_handles.size(); i++)
	{
		if (i != 0)
		{
			DeleteGraph(m_handles[i]);
		}
		else
		{
			MV1DeleteModel(m_handles[i]);
		}
	}

	DeleteLightHandle(m_lightHandle);
}

void SceneStageSelect::Update(std::shared_ptr<Input>& input)
{
	UpdateFade();

	m_player->Update();

	if (!m_isToNextScene)
	{
		if (input->IsTriggered("RIGHT"))
		{

			if (m_destinationScene == e_Destination::Title)
			{
				m_destinationScene = e_Destination::Stage1;
			}
			else
			{
				m_destinationScene = e_Destination::Upgrade;
			}

		}

		if (input->IsTriggered("DOWN"))
		{
			if (m_destinationScene != e_Destination::Title && m_destinationScene != e_Destination::Upgrade && m_destinationScene != e_Destination::Stage3)
			{
				m_destinationScene = static_cast<e_Destination>(static_cast<int>(m_destinationScene) + 1);
			}
		}

		if (input->IsTriggered("UP"))
		{
			if (m_destinationScene != e_Destination::Title && m_destinationScene != e_Destination::Upgrade && m_destinationScene != e_Destination::Stage1)
			{
				m_destinationScene = static_cast<e_Destination>(static_cast<int>(m_destinationScene) - 1);
			}
		}

		if (input->IsTriggered("LEFT"))
		{

			if (m_destinationScene == e_Destination::Upgrade)
			{
				m_destinationScene = e_Destination::Stage1;
			}
			else
			{
				m_destinationScene = e_Destination::Title;

			}
		}

		if (input->IsTriggered("OK"))
		{
			if (m_destinationScene == e_Destination::Title)
			{
				m_isToNextScene = true;
				StartFadeOut();	// フェードアウト開始
			}

			if (m_destinationScene == e_Destination::Upgrade)
			{
				m_manager.PushScene(std::make_shared<SceneUpgrade>(m_manager));
				return;
			}

			if (m_destinationScene == e_Destination::Stage1)
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

			if (m_destinationScene == e_Destination::Stage1)
			{
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

#ifdef _DEBUG
	DrawFormatString(0, 0, 0xffffff, "SceneStageSelect");

	DrawFormatString(64 / 2, 32 + static_cast<int>(m_destinationScene) * 16, 0xff0000, "→");

	DrawFormatString(64, 32 + static_cast<int>(e_Destination::Title) * 16, 0xffffff, "SceneTitle");
	DrawFormatString(64, 32 + static_cast<int>(e_Destination::Stage1) * 16, 0xffffff, "Stage1");
	DrawFormatString(64, 32 + static_cast<int>(e_Destination::Stage2) * 16, 0xffffff, "Stage2");
	DrawFormatString(64, 32 + static_cast<int>(e_Destination::Stage3) * 16, 0xffffff, "Stage3");
	DrawFormatString(64, 32 + static_cast<int>(e_Destination::Upgrade) * 16, 0xffffff, "SceneUpgrade");
#endif
	DrawFade();
}
