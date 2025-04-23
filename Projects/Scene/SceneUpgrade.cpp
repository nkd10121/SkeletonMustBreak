#include "SceneUpgrade.h"
#include "SceneStageSelect.h"
#include "SceneManager.h"

SceneUpgrade::SceneUpgrade(SceneManager& mgr) :
	SceneBase(mgr)
{
}

SceneUpgrade::~SceneUpgrade()
{
}

void SceneUpgrade::Update(std::shared_ptr<Input>& input)
{
	if (input->IsTriggered("A"))
	{
		m_manager.popScene();
	}
}

void SceneUpgrade::Draw()
{
#ifdef _DEBUG
	DrawBox(0, 0, 1280, 720, 0x000000, true);
	DrawFormatString(0, 0, 0xffffff, "SceneUpgrade");
#endif
}
