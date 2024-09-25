#include "Input.h"
#include <cassert>

enum PadCheckMask : int
{
	A = 0x00000010,	//Aボタン
	B = 0x00000020,	//Bボタン
	X = 0x00000040,	//Xボタン
	Y = 0x00000080,	//Yボタン

	L = 0x00000100,	//Lボタン
	R = 0x00000200,	//Rボタン

	RStickPush = 0x00002000,	//Rスティック押し込み
	test = 0x00008000,	//Rスティック押し込み

	P = 0x00000800,	//ホームボタン(?)

	UP = 0x00000008,	//上ボタン
	RIGHT = 0x00000004,	//右ボタン
	DOWN = 0x00000001,	//下ボタン
	LEFT = 0x00000002,	//左ボタン
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
	m_lastInputData = m_inputData;	//直前入力をコピーしておく(押した瞬間を取得する用)

	//ハードウェア入力チェック
	char keystate[256];
	GetHitKeyStateAll(keystate);//現在のキーボード入力を取得
	int padstate = GetJoypadInputState(DX_INPUT_PAD1);//パッド情報の取得

	//登録された情報とハードの情報を照らし合わせながら
	//inputData_の内容を更新していく
	for (const auto& cmd : m_commandTable)
	{
		auto& input = m_inputData[cmd.first];//コマンドの名前から入力データを作る

		for (const auto& hardIO : cmd.second)
		{
			//キーボードのチェック
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

	//アナログスティックを使って移動
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
	if (it == m_inputData.end())	//情報がなければfalseを返す
	{
		assert(0 && "指定されたコマンドは登録されていません");
		return false;
	}
	return (m_inputData.at(command) && !m_lastInputData.at(command));
}

bool Input::IsPushed(const char* command) const
{
	auto it = m_inputData.find(command);
	if (it == m_inputData.end())	//情報がなければfalseを返す
	{
		assert(0 && "指定されたコマンドは登録されていません");
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
