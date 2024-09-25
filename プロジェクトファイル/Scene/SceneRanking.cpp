#include "SceneRanking.h"
#include "SceneTitle.h"
#include "SceneManager.h"

SceneRanking::SceneRanking(SceneManager& mgr) :
	SceneBase(mgr)
{
}

SceneRanking::~SceneRanking()
{
}

void SceneRanking::Update(std::shared_ptr<Input>& input)
{
	if (input->IsTriggered("A"))
	{
		m_manager.ChangeScene(std::make_shared<SceneTitle>(m_manager));
	}
}

void SceneRanking::Draw()
{
#ifdef _DEBUG
	DrawFormatString(0, 0, 0xffffff, "SceneRanking");
#endif
}
