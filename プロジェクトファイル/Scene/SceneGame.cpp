#include "SceneGame.h"
#include "ScenePause.h"
#include "SceneResult.h"
#include "SceneManager.h"
#include "GameManager.h"

namespace
{
#ifdef _DEBUG
	//デバッググリッドの長さ
	constexpr int kGridLength = 50;
	//デバッググリッドの間隔
	constexpr int kGridInterval = 10;
#endif

	//クロスヘア関係
	constexpr int kCrossHairPosX = 640;
	constexpr int kCrossHairPosY = 372;
	constexpr int kCrossHairHalfSize = 2;
	constexpr unsigned int kCrossHairColor = 0xffffff;

	//次のシーンに遷移するまでの待機フレーム
	constexpr int kNextSceneWaitFrame = 200;
}

/// <summary>
/// コンストラクタ
/// </summary>
/// <param name="mgr">シーン管理クラスの参照</param>
SceneGame::SceneGame(SceneManager& mgr):
	SceneBase(mgr),
	m_pGameManager(nullptr),
	m_isFadeOuting(false),
	m_isThisSceneEnd(false),
	m_waitFrame(0)
{
	m_pGameManager = std::make_shared<GameManager>(mgr.GetStageName().c_str());
	m_pGameManager->Init(mgr.GetScoreInfoPtr());

	SetLightEnable(true);
}

/// <summary>
/// デストラクタ
/// </summary>
SceneGame::~SceneGame()
{

}

/// <summary>
/// 更新
/// </summary>
/// <param name="input">入力管理クラスの参照</param>
void SceneGame::Update(std::shared_ptr<Input>& input)
{
#ifdef _DEBUG
	MyLib::DebugDraw::Clear();
#endif

	if (m_isFadeOuting)
	{
		if (IsFinishFadeOut())
		{
			SceneBase::StartFadeIn();
			m_isFadeOuting = false;
		}
	}

	UpdateFade();

	m_pGameManager->Update(input,this);

	if (m_isThisSceneEnd)
	{
		return;
	}

	if (!IsFading())
	{
		if (input->IsTriggered("PAUSE"))
		{
			m_manager.PushScene(std::make_shared<ScenePause>(m_manager));
		}
	}
}

/// <summary>
/// 描画
/// </summary>
void SceneGame::Draw()
{
#ifdef _DEBUG

	//仮グリッド(移動しているかとかみるため)
	for (int x = -kGridLength; x <= kGridLength; x += kGridInterval)
	{
		DrawLine3D(VGet(static_cast<float>(x), 0, -kGridLength), VGet(static_cast<float>(x), 0, kGridLength), 0xffff00);
	}
	for (int z = -kGridLength; z <= kGridLength; z += kGridInterval)
	{
		DrawLine3D(VGet(-kGridLength, 0, static_cast<float>(z)), VGet(kGridLength, 0, static_cast<float>(z)), 0xff0000);
	}

	DrawFormatString(0, 0, 0xffffff, "SceneGame");

	MyLib::DebugDraw::Draw3D();
#endif

	m_pGameManager->Draw();

	//クロスヘア(仮)
	//DrawBox(kCrossHairPosX - kCrossHairHalfSize, kCrossHairPosY - kCrossHairHalfSize, kCrossHairPosX + kCrossHairHalfSize, kCrossHairPosY + kCrossHairHalfSize, kCrossHairColor, true);

	DrawFade();
}

/// <summary>
/// フェードアウト開始
/// </summary>
void SceneGame::StartFadeOut()
{
	SceneBase::StartFadeOut();
	m_isFadeOuting = true;
}

/// <summary>
/// リザルトシーンに遷移する
/// </summary>
/// <param name="isClear">true:クリアしている,false:ゲームオーバーしている</param>
void SceneGame::ToResultScene(bool isClear)
{
	if (isClear)
	{
		if (m_waitFrame > kNextSceneWaitFrame)
		{
			m_manager.SetIsClear(isClear);
			m_manager.PushScene(std::make_shared<SceneResult>(m_manager));
			m_isThisSceneEnd = true;
		}


		m_waitFrame++;
	}
	else
	{
		m_manager.SetIsClear(isClear);
		m_manager.PushScene(std::make_shared<SceneResult>(m_manager));
		m_isThisSceneEnd = true;
	}
}
