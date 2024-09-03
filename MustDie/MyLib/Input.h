#pragma once
#include<map>
#include<string>
#include<unordered_map>

#include <DxLib.h>

enum class InputType
{
	keyboard,	//�L�[�{�[�h
	pad			//�p�b�h
};

using InputTable_t = std::unordered_map<std::string, std::map<InputType, int>>;

class Input
{
public:
	Input();

	/// <summary>
	/// ���͏����X�V����
	/// </summary>
	void Update();

	/// <summary>
	/// �w��̃R�}���h�������ꂽ�u�ԂȂ̂�
	/// </summary>
	/// <param name="command">�R�}���h������</param>
	/// <returns> true : �����ꂽ�u��, false : ������Ă��Ȃ����A��������</returns>
	bool IsTriggered(const char* command)const;

	/// <summary>
	/// 
	/// </summary>
	/// <param name="command"></param>
	/// <returns></returns>
	bool IsPushed(const char* command)const;

	std::pair<float, float> GetLeftStick()const;
	std::pair<float, float> GetRightStick()const;

	bool GetIsPushedZR();
	bool GetIsPushedZL();

private:
	InputTable_t m_commandTable;
	//�R�}���h�̓��͂��o���Ă���
	std::map < std::string, bool> m_inputData;		//���݂̓���
	std::map < std::string, bool> m_lastInputData;	//���O�̓���

	//�R���g���[���[�̓��͏��
	DINPUT_JOYSTATE m_padState;

	bool m_isZR = false;
	bool m_isZL = false;
};
