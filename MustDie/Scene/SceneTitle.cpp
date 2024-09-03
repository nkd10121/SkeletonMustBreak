#include "SceneTitle.h"
#include "SceneStageSelect.h"
#include "SceneOption.h"
#include "SceneRanking.h"
#include "SceneManager.h"
#include "TitlePlayer.h"

namespace
{
	enum ui
	{
		rogo,
		play,
		option,
		quit,
		rightArrow,
		leftArrow,
		BGmodel
	};


	constexpr float kCameraNear = 10.5f;
	constexpr float kCameraFar = 180.0f;

}

SceneTitle::SceneTitle(SceneManager& mgr) :
	SceneBase(mgr),
	m_angle(0),
	m_cursorOffsetX(0)
{
	m_destinationScene = e_Destination::StageSelect;

	m_handles.push_back(LoadGraph("data/img/rogo.png"));
	m_handles.push_back(LoadGraph("data/img/ui_play.png"));
	m_handles.push_back(LoadGraph("data/img/ui_option.png"));
	m_handles.push_back(LoadGraph("data/img/ui_quit.png"));

	m_handles.push_back(LoadGraph("data/img/ArrowRight.png"));
	m_handles.push_back(LoadGraph("data/img/ArrowLeft.png"));

	m_handles.push_back(MV1LoadModel("data/model/title.mv1"));

	MV1SetScale(m_handles[BGmodel],VECTOR(0.1f,0.1f,0.1f));
	MV1SetPosition(m_handles[BGmodel],VECTOR(0.0f, 0.0f,0.0f));

	SetCameraNearFar(kCameraNear, 1000.0f);

	SetCameraPositionAndTarget_UpVecY(VECTOR(0.0f,180.0f,400.0f), VECTOR(0.0f,160.0f,0.0f));

	m_player = std::make_shared<TitlePlayer>();
	m_player->Init(MyLib::Vec3(-260.0f, 30.0f, 20.0f));

	//ChangeLightTypePoint(VGet(0.0f, 0.0f, 400.0), 1000.0f, 0.08f, 0.003f, 0.0f);
	//SetLightPosition(VECTOR(0.0f, 0.0f, 400.0f));
	m_lightHandle = CreatePointLightHandle(VGet(0.0f, 0.0f, 400.0), 1000.0f, 0.08f, 0.003f, 0.0f);

	SetLightEnable(FALSE);
}

SceneTitle::~SceneTitle()
{
	for (int i = 0; i < m_handles.size(); i++)
	{
		if (i != 6)
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

void SceneTitle::Update(std::shared_ptr<Input>& input)
{
	UpdateFade();

	m_player->Update();

	m_angle += 0.14f;
	m_cursorOffsetX = static_cast<int>(sinf(m_angle) * 8);

	if (!m_isToNextScene)
	{
		if (input->IsTriggered("UP"))
		{
			if (m_destinationScene != e_Destination::StageSelect)
			{
				m_destinationScene = static_cast<e_Destination>(static_cast<int>(m_destinationScene) - 1);
			}
		}

		if (input->IsTriggered("DOWN"))
		{
			if (m_destinationScene != e_Destination::Quit)
			{
				m_destinationScene = static_cast<e_Destination>(static_cast<int>(m_destinationScene) + 1);
			}
		}

		if (input->IsTriggered("OK"))
		{

			if (m_destinationScene == e_Destination::StageSelect)
			{
				m_isToNextScene = true;
				StartFadeOut();	// フェードアウト開始
			}

			if (m_destinationScene == e_Destination::Option)
			{
				m_manager.PushScene(std::make_shared<SceneOption>(m_manager));
				return;
			}

			if (m_destinationScene == e_Destination::Ranking)
			{
				m_isToNextScene = true;
				StartFadeOut();	// フェードアウト開始
			}

			if (m_destinationScene == e_Destination::Quit)
			{
				DxLib_End();
			}

		}
	}




	if (m_isToNextScene)
	{
		if (!IsFadingOut())
		{
			if (m_destinationScene == e_Destination::StageSelect)
			{
				m_manager.ChangeScene(std::make_shared<SceneStageSelect>(m_manager));
				return;
			}

			if (m_destinationScene == e_Destination::Ranking)
			{
				m_manager.ChangeScene(std::make_shared<SceneRanking>(m_manager));
				return;
			}
		}
	}
}

void SceneTitle::Draw()
{

	MV1DrawModel(m_handles[BGmodel]);

	m_player->Draw();

	DrawRotaGraph(640,200,0.7f,0.0f, m_handles[rogo], true);

	DrawRotaGraph(640,460 + static_cast<int>(e_Destination::StageSelect) * 80,1.0f,0.0f, m_handles[play], true);
	DrawRotaGraph(640,460 + static_cast<int>(e_Destination::Option) * 80,1.0f,0.0f, m_handles[option], true);
	DrawRotaGraph(640,460 + static_cast<int>(e_Destination::Quit) * 80,1.0f,0.0f, m_handles[quit], true);

	DrawRotaGraph(440 + m_cursorOffsetX, 460 + static_cast<int>(m_destinationScene) * 80, 1.0f, 0.0f, m_handles[rightArrow], true);
	DrawRotaGraph(840 - m_cursorOffsetX, 460 + static_cast<int>(m_destinationScene) * 80, 1.0f, 0.0f, m_handles[leftArrow], true);

#ifdef _DEBUG
	DrawFormatString(0, 0, 0xffffff, "SceneTitle");

	DrawFormatString(64 / 2, 32 + static_cast<int>(m_destinationScene) * 16, 0xff0000, "→");

	DrawFormatString(64, 32 + static_cast<int>(e_Destination::StageSelect) * 16, 0xffffff, "SceneStageSelect");
	DrawFormatString(64, 32 + static_cast<int>(e_Destination::Option) * 16, 0xffffff, "SceneOption");
	DrawFormatString(64, 32 + static_cast<int>(e_Destination::Ranking) * 16, 0xffffff, "SceneRanking");
	DrawFormatString(64, 32 + static_cast<int>(e_Destination::Quit) * 16, 0xffffff, "Quit");

#endif

	DrawFade();
}