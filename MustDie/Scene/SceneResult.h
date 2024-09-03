#pragma once
#include "SceneBase.h"
#include <memory>

class SceneResult : public SceneBase
{
public:
	SceneResult(SceneManager& mgr);
	virtual~SceneResult();

	/// <summary>
	/// �X�V����
	/// </summary>
	/// <param name="input"></param>
	void Update(std::shared_ptr<Input>& input);

	/// <summary>
	/// �`�揈��
	/// </summary>
	void Draw();

private:
	//�J�ڐ�
	enum class e_Destination : int
	{
		InGame,
		Upgrade,
		StageSelect,
	};

	e_Destination m_destinationScene;

	bool m_isClear;


	//�t�H���g�n���h��
	int m_fontHandle;
	int m_textHandle;
};

