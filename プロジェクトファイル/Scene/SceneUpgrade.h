#pragma once
#include "SceneBase.h"
class SceneUpgrade : public SceneBase
{
public:
	SceneUpgrade(SceneManager& mgr);
	virtual ~SceneUpgrade();

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
};

