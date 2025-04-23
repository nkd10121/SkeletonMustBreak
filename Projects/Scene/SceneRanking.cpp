#include "SceneRanking.h"
#include "SceneTitle.h"
#include "SceneManager.h"

/// <summary>
/// コンストラクタ
/// </summary>
/// <param name="mgr">シーン管理クラスの参照</param>
SceneRanking::SceneRanking(SceneManager& mgr) :
	SceneBase(mgr)
{
}

/// <summary>
/// デストラクタ
/// </summary>
SceneRanking::~SceneRanking()
{
}

/// <summary>
/// 更新
/// </summary>
/// <param name="input">入力管理クラスの参照</param>
void SceneRanking::Update(std::shared_ptr<Input>& input)
{
	if (input->IsTriggered("A"))
	{
		m_manager.ChangeScene(std::make_shared<SceneTitle>(m_manager));
	}
}

/// <summary>
/// 描画
/// </summary>
void SceneRanking::Draw()
{
#ifdef _DEBUG
	DrawFormatString(0, 0, 0xffffff, "SceneRanking");
#endif
}
