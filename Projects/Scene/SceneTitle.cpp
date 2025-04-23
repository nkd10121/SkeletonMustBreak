#include "SceneTitle.h"
#include "SceneStageSelect.h"
#include "SceneOption.h"
#include "SceneRanking.h"
#include "SceneManager.h"
#include "TitlePlayer.h"
#include "UICursor.h"

#include "EffekseerForDXLib.h"
#include "SoundManager.h"
#include "ModelManager.h"

namespace
{
	//背景モデルのスケール
	constexpr float kBGModelScale = 0.1f;

	enum ui
	{
		rogo,
		play,
		option,
		quit,
		BGmodel
	};

	//カメラのニアファー
	constexpr float kCameraNear = 10.5f;
	constexpr float kCameraFar = 1000.0f;
	//カメラの座標
	const VECTOR kCameraPos = VGet(0.0f,180.0f,400.0f);
	//カメラの注視点の座標
	const VECTOR kCameraTargetPos = VGet(0.0f, 160.0f, 0.0f);
	//プレイヤーの座標
	const MyLib::Vec3 kPlayerPos = MyLib::Vec3(-260.0f, 30.0f, 20.0f);
}

/// <summary>
/// コンストラクタ
/// </summary>
/// <param name="mgr">シーン管理クラスの参照</param>
SceneTitle::SceneTitle(SceneManager& mgr) :
	SceneBase(mgr),
	m_pCursor(nullptr),
	m_UIHandles(),
	m_lightHandle(-1),
	m_destinationScene(eDestination::StageSelect),
	m_pPlayer()
{
	m_destinationScene = eDestination::StageSelect;

	//カーソルの生成
	m_pCursor = std::make_shared<UICursor>();
	m_pCursor->Init();

	m_UIHandles.push_back(LoadGraph("data/img/rogo.png"));
	m_UIHandles.push_back(LoadGraph("data/img/ui_play.png"));
	m_UIHandles.push_back(LoadGraph("data/img/ui_option.png"));
	m_UIHandles.push_back(LoadGraph("data/img/ui_quit.png"));
	m_UIHandles.push_back(ModelManager::GetInstance().GetModelHandle("data/model/title.mv1",true));

	MV1SetScale(m_UIHandles[BGmodel],VECTOR(kBGModelScale, kBGModelScale, kBGModelScale));
	MV1SetPosition(m_UIHandles[BGmodel],VECTOR(0.0f, 0.0f,0.0f));

	SetCameraNearFar(kCameraNear, kCameraFar);

	SetCameraPositionAndTarget_UpVecY(kCameraPos, kCameraTargetPos);

	m_pPlayer = std::make_shared<TitlePlayer>();
	m_pPlayer->Init(kPlayerPos);

	m_lightHandle = CreatePointLightHandle(VGet(0.0f, 0.0f, 400.0), 1000.0f, 0.08f, 0.003f, 0.0f);

	SetLightEnable(FALSE);

	SoundManager::GetInstance().Load("select","data/sound/se/select.mp3",false);
	SoundManager::GetInstance().Load("enter","data/sound/se/enter.mp3",false);

	SoundManager::GetInstance().Load("titleBgm","data/sound/bgm/title.mp3",true);

}

/// <summary>
/// デストラクタ
/// </summary>
SceneTitle::~SceneTitle()
{
	for (int i = 0; i < m_UIHandles.size(); i++)
	{
		if (i != 6)
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
void SceneTitle::Update(std::shared_ptr<Input>& input)
{
	UpdateFade();

	SoundManager::GetInstance().PlayBGM("titleBgm", true);

	m_pPlayer->Update();

	m_pCursor->Update(200, 460 + static_cast<int>(m_destinationScene) * 80);

	if (!m_isChangeNextScene)
	{
		if (input->IsTriggered("UP"))
		{
			if (m_destinationScene != eDestination::StageSelect)
			{
				SoundManager::GetInstance().PlaySE("select");
				m_destinationScene = static_cast<eDestination>(static_cast<int>(m_destinationScene) - 1);
			}
		}

		if (input->IsTriggered("DOWN"))
		{
			if (m_destinationScene != eDestination::Quit)
			{
				SoundManager::GetInstance().PlaySE("select");
				m_destinationScene = static_cast<eDestination>(static_cast<int>(m_destinationScene) + 1);
			}
		}

		if (input->IsTriggered("A"))
		{
			SoundManager::GetInstance().PlaySE("enter");

			if (m_destinationScene == eDestination::StageSelect)
			{
				m_isChangeNextScene = true;
				StartFadeOut();	// フェードアウト開始
			}

			if (m_destinationScene == eDestination::Option)
			{
				m_manager.PushScene(std::make_shared<SceneOption>(m_manager));
				return;
			}

			if (m_destinationScene == eDestination::Ranking)
			{
				m_isChangeNextScene = true;
				StartFadeOut();	// フェードアウト開始
			}

			if (m_destinationScene == eDestination::Quit)
			{
				Effkseer_End();
				DxLib_End();
			}

		}
	}

	if (m_isChangeNextScene)
	{
		if (!IsFadingOut())
		{
			if (m_destinationScene == eDestination::StageSelect)
			{
				m_manager.ChangeScene(std::make_shared<SceneStageSelect>(m_manager));
				return;
			}

			if (m_destinationScene == eDestination::Ranking)
			{
				m_manager.ChangeScene(std::make_shared<SceneRanking>(m_manager));
				return;
			}
		}
	}
}

/// <summary>
/// 描画
/// </summary>
void SceneTitle::Draw()
{

	MV1DrawModel(m_UIHandles[BGmodel]);

	m_pPlayer->Draw();

	DrawRotaGraph(640,200,0.7f,0.0f, m_UIHandles[rogo], true);

	DrawRotaGraph(640,460 + static_cast<int>(eDestination::StageSelect) * 80,1.0f,0.0f, m_UIHandles[play], true);
	DrawRotaGraph(640,460 + static_cast<int>(eDestination::Option) * 80,1.0f,0.0f, m_UIHandles[option], true);
	DrawRotaGraph(640,460 + static_cast<int>(eDestination::Quit) * 80,1.0f,0.0f, m_UIHandles[quit], true);

	m_pCursor->Draw();

#ifdef _DEBUG
	DrawFormatString(0, 0, 0xffffff, "SceneTitle");

	DrawFormatString(64 / 2, 32 + static_cast<int>(m_destinationScene) * 16, 0xff0000, "→");

	DrawFormatString(64, 32 + static_cast<int>(eDestination::StageSelect) * 16, 0xffffff, "SceneStageSelect");
	DrawFormatString(64, 32 + static_cast<int>(eDestination::Option) * 16, 0xffffff, "SceneOption");
	DrawFormatString(64, 32 + static_cast<int>(eDestination::Ranking) * 16, 0xffffff, "SceneRanking");
	DrawFormatString(64, 32 + static_cast<int>(eDestination::Quit) * 16, 0xffffff, "Quit");

#endif

	DrawFade();
}