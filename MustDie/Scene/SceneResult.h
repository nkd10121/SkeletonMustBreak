#pragma once
#include "SceneBase.h"
#include <memory>

class SceneResult : public SceneBase
{
public:
	SceneResult(SceneManager& mgr);
	virtual~SceneResult();

	/// <summary>
	/// 更新処理
	/// </summary>
	/// <param name="input"></param>
	void Update(std::shared_ptr<Input>& input);

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();

private:
	//遷移先
	enum class e_Destination : int
	{
		InGame,
		Upgrade,
		StageSelect,
	};

	e_Destination m_destinationScene;

	bool m_isClear;


	//フォントハンドル
	int m_fontHandle;
	int m_textHandle;
};

