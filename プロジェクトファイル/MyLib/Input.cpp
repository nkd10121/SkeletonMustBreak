#include "Input.h"
#include <cassert>

enum PadCheckMask : int
{
	A = 0x00000010,	//A�{�^��
	B = 0x00000020,	//B�{�^��
	X = 0x00000040,	//X�{�^��
	Y = 0x00000080,	//Y�{�^��

	L = 0x00000100,	//L�{�^��
	R = 0x00000200,	//R�{�^��

	RStickPush = 0x00002000,	//R�X�e�B�b�N��������
	test = 0x00008000,	//R�X�e�B�b�N��������

	P = 0x00000800,	//�z�[���{�^��(?)

	UP = 0x00000008,	//��{�^��
	RIGHT = 0x00000004,	//�E�{�^��
	DOWN = 0x00000001,	//���{�^��
	LEFT = 0x00000002,	//���{�^��
};

Input::Input():
	m_padState()
{
	m_commandTable["A"] = { { InputType::keyboard,KEY_INPUT_RETURN },
							{InputType::pad,PadCheckMask::A} };

	m_commandTable["PAUSE"] = { { InputType::keyboard,KEY_INPUT_P },
							{InputType::pad,PadCheckMask::P} };

	m_commandTable["DEBUG"] = { { InputType::keyboard,KEY_INPUT_O },
							{InputType::pad,PAD_INPUT_10} };

	m_commandTable["DEBUG2"] = { { InputType::keyboard,KEY_INPUT_I },
							{InputType::pad,PadCheckMask::test} };

	m_commandTable["Y"] = { { InputType::keyboard,KEY_INPUT_U },
							{InputType::pad,PadCheckMask::Y} };

	m_commandTable["L"] = { { InputType::keyboard,KEY_INPUT_U },
							{InputType::pad,PadCheckMask::L} };

	m_commandTable["R"] = { { InputType::keyboard,KEY_INPUT_U },
							{InputType::pad,PadCheckMask::R} };

	m_commandTable["B"] = { { InputType::keyboard,KEY_INPUT_SPACE },
							{InputType::pad,PadCheckMask::B} };

	m_commandTable["UP"] = { {InputType::keyboard, KEY_INPUT_UP },
							{InputType::pad, PadCheckMask::UP} };

	m_commandTable["DOWN"] = { {InputType::keyboard, KEY_INPUT_DOWN },
							{InputType::pad, PadCheckMask::DOWN} };

	m_commandTable["RIGHT"] = { {InputType::keyboard, KEY_INPUT_RIGHT },
							{InputType::pad, PadCheckMask::RIGHT} };

	m_commandTable["LEFT"] = { {InputType::keyboard, KEY_INPUT_LEFT },
							{InputType::pad, PadCheckMask::LEFT} };
}

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

	//�A�i���O�X�e�B�b�N���g���Ĉړ�
	GetJoypadDirectInputState(DX_INPUT_PAD1, &m_padState);

	XINPUT_STATE* XInputState = new XINPUT_STATE;
	GetJoypadXInputState(DX_INPUT_PAD1, XInputState);
	if (XInputState->RightTrigger > 60)
	{
		m_isZR = true;
	}
	else
	{
		m_isZR = false;
	}

	if (XInputState->LeftTrigger > 60)
	{
		m_isZL = true;
	}
	else
	{
		m_isZL = false;
	}
}

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

std::pair<float, float> Input::GetLeftStick()const
{
	return std::make_pair(static_cast<float>(m_padState.X), static_cast<float>(m_padState.Y));
}

std::pair<float, float> Input::GetRightStick() const
{
	return std::make_pair(static_cast<float>(m_padState.Rx), static_cast<float>(m_padState.Ry));
}

bool Input::GetIsPushedZR()
{
	return m_isZR;
}

bool Input::GetIsPushedZL()
{
	return m_isZL;
}
