#include "SceneOption.h"
#include "SceneManager.h"

#include "Font.h"

SceneOption::SceneOption(SceneManager& mgr) :
	SceneBase(mgr)
{
	FadeInSkip();

	m_fontHandle = Font::GetInstance().GetFontHandle("data/font/Dela-Gothic-One.ttf", "Dela Gothic One", 32);

	m_bgHandle = LoadGraph("data/img/optionBG.png");
}

SceneOption::~SceneOption()
{
}

void SceneOption::Update(std::shared_ptr<Input>& input)
{
	UpdateFade();

	if (input->IsTriggered("OK"))
	{
		m_manager.popScene();
	}
}

void SceneOption::Draw()
{
	DrawRotaGraph(640, 360, 1.0f, 0.0f, m_bgHandle, true);

	DrawStringToHandle(300, 200, "BGM", 0xffffff, m_fontHandle);
	DrawStringToHandle(300, 250, "SE", 0xffffff, m_fontHandle);
	DrawStringToHandle(300, 300, "感度", 0xffffff, m_fontHandle);
	DrawStringToHandle(300, 350, "フルスクリーン", 0xffffff, m_fontHandle);
#ifdef _DEBUG
	//DrawBox(0, 0, 1280, 720, 0x000000, true);
	DrawFormatString(0, 0, 0xffffff, "SceneOption");
#endif
	DrawFade();
}
