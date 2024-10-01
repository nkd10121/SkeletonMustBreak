#pragma once
#include<map>
#include<string>
#include<unordered_map>

#include <DxLib.h>

class Input
{
private:
	//���̓f�o�C�X
	enum class InputType
	{
		keyboard,	//�L�[�{�[�h
		pad			//�p�b�h
	};

	//���͏��̌^
	using InputTable_t = std::unordered_map<std::string, std::map<InputType, int>>;

public:
	//�R���X�g���N�^
	Input();

	//���͏����X�V����
	void Update();

	//�w��̃R�}���h�������ꂽ�u�ԂȂ̂�
	bool IsTriggered(const char* command)const;

	//�w��̃R�}���h��������Ă����ԂȂ̂�
	bool IsPushed(const char* command)const;

	//�X�e�B�b�N�̓��͏����擾
	std::pair<float, float> GetInputStick(bool isRight)const;

	//ZR,ZL�{�^���̓��͏����擾
	bool GetIsPushedTriggerButton(bool isRight)const;

private:
	//�R�}���h
	InputTable_t m_commandTable;

	//�R�}���h�̓��͂��o���Ă���
	std::map < std::string, bool> m_inputData;		//���݂̓���
	std::map < std::string, bool> m_lastInputData;	//���O�̓���

	//�R���g���[���[�̓��͏��
	DINPUT_JOYSTATE m_padState;
	XINPUT_STATE* XInputState;
};
