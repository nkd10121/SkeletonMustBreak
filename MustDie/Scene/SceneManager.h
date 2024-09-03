#pragma once
#include <list> 
#include <memory>
#include <windows.h>

class Input;
class SceneBase;

class SceneManager
{
public:
	SceneManager();
	virtual ~SceneManager();

	/// <summary>
	/// �����Ă���V�[����Update�֐����Ăяo��
	/// </summary>
	/// <param name="input">���̓N���X</param>
	void Update(std::shared_ptr<Input>& input);

	/// <summary>
	/// �����Ă���V�[����Draw�֐����Ăяo��
	/// </summary>
	void Draw();

	/// <summary>
	/// ���s���̃V�[���������Ŏw�肵���V�[���ɐ؂�ւ���
	/// </summary>
	/// <param name="nextScene"></param>
	void ChangeScene(std::shared_ptr<SceneBase> nextScene);

	/// <summary>
	/// Scene���N���A���ăV�[����؂�ւ���
	/// </summary>
	/// <param name="nextScene"></param>
	void ChangeAndClearScene(std::shared_ptr<SceneBase> nextScene);

	/// <summary>
	/// ���ݐ擪�Ŏ��s���̃V�[���̏�ɃV�[�����������
	/// </summary>
	/// <param name="scene"></param>
	void PushScene(std::shared_ptr<SceneBase> scene);

	/// <summary>
	/// ���݂̐擪�V�[�����폜����
	/// </summary>
	void popScene(bool isToTitle = false);

	void SetIsClear(bool isClear) { m_isClear = isClear; }
	const bool GetIsClear()const { return m_isClear; }

#ifdef _DEBUG
private:
	void DrawDebug();

private:
	float m_updateTime;
	float m_drawTime;

#endif
private:
	std::list < std::shared_ptr<SceneBase>> scenes;

	bool m_isClear;
};

