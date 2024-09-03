#include "SceneGame.h"
#include "ScenePause.h"
#include "SceneResult.h"
#include "SceneManager.h"
#include "GameManager.h"

namespace
{
	//クロスヘア関係
	constexpr int kCrossHairPosX = 640;
	constexpr int kCrossHairPosY = 372;
	constexpr int kCrossHairHalfSize = 2;
	constexpr unsigned int kCrossHairColor = 0xffffff;
}

SceneGame::SceneGame(SceneManager& mgr):
	SceneBase		(mgr)
{
	m_pGameManager = std::make_shared<GameManager>();
	m_pGameManager->Init();

	m_isFadeOuting = false;
	m_isThisSceneEnd = false;

	SetLightEnable(true);
}

SceneGame::~SceneGame()
{

}

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

	if (input->IsTriggered("PAUSE"))
	{
		m_manager.PushScene(std::make_shared<ScenePause>(m_manager));
	}

	if (input->IsTriggered("DEBUG2"))
	{

		//if (IsFineshFadeOut())
		{
			m_manager.PushScene(std::make_shared<SceneResult>(m_manager));
		}
	}
}

void SceneGame::Draw()
{
#ifdef _DEBUG
	//m_pTrapGrid->DebugDraw();

	//仮グリッド(移動しているかとかみるため)
	for (int x = -50; x <= 50; x += 10)
	{
		DrawLine3D(VGet(static_cast<float>(x), 0, -50), VGet(static_cast<float>(x), 0, 50), 0xffff00);
	}
	for (int z = -50; z <= 50; z += 10)
	{
		DrawLine3D(VGet(-50, 0, static_cast<float>(z)), VGet(50, 0, static_cast<float>(z)), 0xff0000);
	}

	DrawFormatString(0, 0, 0xffffff, "SceneGame");

	MyLib::DebugDraw::Draw3D();
#endif

	m_pGameManager->Draw();

	//クロスヘア(仮)
	//DrawBox(kCrossHairPosX - kCrossHairHalfSize, kCrossHairPosY - kCrossHairHalfSize, kCrossHairPosX + kCrossHairHalfSize, kCrossHairPosY + kCrossHairHalfSize, kCrossHairColor, true);

	DrawFade();
}

void SceneGame::StartFadeOut()
{
	SceneBase::StartFadeOut();
	m_isFadeOuting = true;
}

void SceneGame::ToResultScene(bool isClear)
{
	m_manager.SetIsClear(isClear);
	m_manager.PushScene(std::make_shared<SceneResult>(m_manager));
	m_isThisSceneEnd = true;
}
