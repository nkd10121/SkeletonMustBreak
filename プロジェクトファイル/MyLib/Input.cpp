#include "Input.h"
#include <cassert>

namespace
{
	//�R���g���[���[�̃{�^��
	enum PadCheckMask : int
	{
		A = 0x00000010,	//A�{�^��
		B = 0x00000020,	//B�{�^��
		X = 0x00000040,	//X�{�^��
		Y = 0x00000080,	//Y�{�^��

		L = 0x00000100,	//L�{�^��
		R = 0x00000200,	//R�{�^��

		RStickPush = 0x00002000,	//R�X�e�B�b�N��������

		P = 0x00000800,	//�z�[���{�^��(?)

		UP = 0x00000008,	//��{�^��
		RIGHT = 0x00000004,	//�E�{�^��
		DOWN = 0x00000001,	//���{�^��
		LEFT = 0x00000002,	//���{�^��
	};

	//�g���K�[�{�^���̃f�b�h�]�[��
	constexpr int kTriggerDeadZone = 60;
}


/// <summary>
/// �R���X�g���N�^
/// </summary>
Input::Input():
	m_padState(),
	XInputState(new XINPUT_STATE)
{
	m_commandTable["A"] = { { InputType::keyboard,KEY_INPUT_RETURN },
							{InputType::pad,PadCheckMask::A} };

	m_commandTable["B"] = { { InputType::keyboard,KEY_INPUT_SPACE },
							{InputType::pad,PadCheckMask::B} };

	m_commandTable["Y"] = { { InputType::keyboard,KEY_INPUT_U },
							{InputType::pad,PadCheckMask::Y} };

	m_commandTable["PAUSE"] = { { InputType::keyboard,KEY_INPUT_P },
							{InputType::pad,PadCheckMask::P} };

	m_commandTable["L"] = { { InputType::keyboard,KEY_INPUT_U },
							{InputType::pad,PadCheckMask::L} };

	m_commandTable["R"] = { { InputType::keyboard,KEY_INPUT_U },
							{InputType::pad,PadCheckMask::R} };

	m_commandTable["UP"] = { {InputType::keyboard, KEY_INPUT_W },
							{InputType::pad, PadCheckMask::UP} };

	m_commandTable["DOWN"] = { {InputType::keyboard, KEY_INPUT_S },
							{InputType::pad, PadCheckMask::DOWN} };

	m_commandTable["RIGHT"] = { {InputType::keyboard, KEY_INPUT_D },
							{InputType::pad, PadCheckMask::RIGHT} };

	m_commandTable["LEFT"] = { {InputType::keyboard, KEY_INPUT_A },
							{InputType::pad, PadCheckMask::LEFT} };
}

/// <summary>
/// ���͏����X�V����
/// </summary>
void Input::Update()
{
	m_lastInputData = m_inputData;	//���O���͂��R�s�[���Ă���(�������u�Ԃ��擾����p)

	//�n�[�h�E�F�A���̓`�F�b�N
	char keystate[256];
	GetHitKeyStateAll(keystate);//���݂̃L�[�{�[�h���͂��擾
	int padstate = GetJoypadInputState(DX_INPUT_PAD1);//�p�b�h���̎擾

	//�o�^���ꂽ���ƃn�[�h�̏����Ƃ炵���킹�Ȃ���
	//inputData_�̓��e���X�V���Ă���
	for (const auto& cmd : m_commandTable)
	{
		auto& input = m_inputData[cmd.first];//�R�}���h�̖��O������̓f�[�^�����

		for (const auto& hardIO : cmd.second)
		{
			//�L�[�{�[�h�̃`�F�b�N
			input = false;
			if (hardIO.first == InputType::keyboard)
			{
				if (keystate[hardIO.second])
				{
					input = true;
					break;
				}
			}
			else if (hardIO.first == InputType::pad)
			{
				if (padstate & hardIO.second)
				{
					input = true;
					break;
				}
			}
		}
	}

	//�{�^���̓��͂��擾
	GetJoypadDirectInputState(DX_INPUT_PAD1, &m_padState);
	//ZR,ZL,�X�e�B�b�N�̓��͂��擾
	GetJoypadXInputState(DX_INPUT_PAD1, XInputState);


}

/// <summary>
/// �w��̃R�}���h�������ꂽ�u�ԂȂ̂�
/// </summary>
/// <param name="command">�R�}���h������</param>
/// <returns> true : �����ꂽ�u��, false : ������Ă��Ȃ����A��������</returns>
bool Input::IsTriggered(const char* command) const
{
	auto it = m_inputData.find(command);
	if (it == m_inputData.end())	//��񂪂Ȃ����false��Ԃ�
	{
		assert(0 && "�w�肳�ꂽ�R�}���h�͓o�^����Ă��܂���");
		return false;
	}
	return (m_inputData.at(command) && !m_lastInputData.at(command));
}

/// <summary>
/// �w��̃R�}���h��������Ă����ԂȂ̂�
/// </summary>
/// <param name="command">�R�}���h������</param>
/// <returns> true : ��������, false : ������Ă��Ȃ����A�����ꂽ�u��</returns>
bool Input::IsPushed(const char* command) const
{
	auto it = m_inputData.find(command);
	if (it == m_inputData.end())	//��񂪂Ȃ����false��Ԃ�
	{
		assert(0 && "�w�肳�ꂽ�R�}���h�͓o�^����Ă��܂���");
		return false;
	}
	return (m_inputData.at(command) && m_lastInputData.at(command));
}

/// <summary>
/// �X�e�B�b�N�̓��͏����擾
/// </summary>
/// <param name="isRight">true:�E�X�e�B�b�N, false:���X�e�B�b�N</param>
/// <returns>�w��X�e�B�b�N�̓��͂�X������Y����</returns>
std::pair<float, float> Input::GetInputStick(bool isRight)const
{
	if (isRight)
	{
		//�E�X�e�B�b�N�̓��͏���Ԃ�
		return std::make_pair(static_cast<float>(m_padState.Rx), static_cast<float>(m_padState.Ry));
	}
	else
	{
		//���X�e�B�b�N�̓��͏���Ԃ�
		return std::make_pair(static_cast<float>(m_padState.X), static_cast<float>(m_padState.Y));
	}
}

/// <summary>
/// ZR,ZL�{�^���̓��͏����擾
/// </summary>
/// <param name="isRight">true:ZR,false:ZL</param>
/// <returns>true:������Ă���,false:������Ă��Ȃ�</returns>
bool Input::GetIsPushedTriggerButton(bool isRight)const
{
	if (isRight)
	{
		//ZR�̓��͏���Ԃ�
		if (XInputState->RightTrigger > kTriggerDeadZone)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		//ZR�̓��͏���Ԃ�
		if (XInputState->LeftTrigger > kTriggerDeadZone)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
}