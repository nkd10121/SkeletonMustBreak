#pragma once
#include "DxLib.h"
#include "Input.h"
#include <memory>

class SceneManager;

/// <summary>
/// �V�[�����N���X
/// �����ȃV�[���̊�ɂȂ�N���X
/// </summary>
class SceneBase
{
public:
	/// <summary>
	/// ��������SceneManager�̎Q�Ƃ��󂯎���Ă���
	/// </summary>
	/// <param name="manager">SceneManager�̎Q��</param>
	SceneBase(SceneManager& manager);

	/// <summary>
	/// �V�[���̍X�V
	/// </summary>
	/// <param name="input"></param>
	virtual void Update(std::shared_ptr<Input>& input) = 0;

	/// <summary>
	/// �V�[���̕`��
	/// </summary>
	virtual void Draw() = 0;

protected:
	void UpdateFade();
	void DrawFade() const;

	void StartFadeOut();	// �t�F�[�h�A�E�g�J�n
	void StartFadeIn();	// �t�F�[�h�C���J�n

	bool IsFadingIn() const;	// �t�F�[�h�C����
	bool IsFadingOut() const;	// �t�F�[�h�A�E�g��
	bool IsFading() const { return IsFadingIn() || IsFadingOut(); }	// �t�F�[�h�C��or�A�E�g��

	bool IsFinishFadeOut()const;

	void FadeInSkip();
	void FadeOutSkip();

protected:
	SceneManager& m_manager;	//�V�[���Ǘ��N���X�̎Q��
	bool m_isToNextScene;

private:
	// �t�F�[�h�֘A����
	int m_fadeColor;
	int m_fadeBright;
	int m_fadeSpeed;

};

