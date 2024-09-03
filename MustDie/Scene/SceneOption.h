#pragma once
#include "SceneBase.h"
class SceneOption : public SceneBase
{
public:
	SceneOption(SceneManager& mgr);
	virtual ~SceneOption();

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

	int m_fontHandle;
	int m_bgHandle;
};

