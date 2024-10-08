#include "Input.h"
#include <cassert>

namespace
{
	//コントローラーのボタン
	enum PadCheckMask : int
	{
		A = 0x00000010,	//Aボタン
		B = 0x00000020,	//Bボタン
		X = 0x00000040,	//Xボタン
		Y = 0x00000080,	//Yボタン

		L = 0x00000100,	//Lボタン
		R = 0x00000200,	//Rボタン

		RStickPush = 0x00002000,	//Rスティック押し込み

		P = 0x00000800,	//ホームボタン(?)

		UP = 0x00000008,	//上ボタン
		RIGHT = 0x00000004,	//右ボタン
		DOWN = 0x00000001,	//下ボタン
		LEFT = 0x00000002,	//左ボタン
	};

	//トリガーボタンのデッドゾーン
	constexpr int kTriggerDeadZone = 60;
}


/// <summary>
/// コンストラクタ
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
/// 入力情報を更新する
/// </summary>
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

	//ボタンの入力を取得
	GetJoypadDirectInputState(DX_INPUT_PAD1, &m_padState);
	//ZR,ZL,スティックの入力を取得
	GetJoypadXInputState(DX_INPUT_PAD1, XInputState);


}

/// <summary>
/// 指定のコマンドが押された瞬間なのか
/// </summary>
/// <param name="command">コマンド文字列</param>
/// <returns> true : 押された瞬間, false : 押されていないか、押しっぱ</returns>
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

/// <summary>
/// 指定のコマンドが押されている状態なのか
/// </summary>
/// <param name="command">コマンド文字列</param>
/// <returns> true : おしっぱ, false : 押されていないか、押された瞬間</returns>
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

/// <summary>
/// スティックの入力情報を取得
/// </summary>
/// <param name="isRight">true:右スティック, false:左スティック</param>
/// <returns>指定スティックの入力のX成分とY成分</returns>
std::pair<float, float> Input::GetInputStick(bool isRight)const
{
	if (isRight)
	{
		//右スティックの入力情報を返す
		return std::make_pair(static_cast<float>(m_padState.Rx), static_cast<float>(m_padState.Ry));
	}
	else
	{
		//左スティックの入力情報を返す
		return std::make_pair(static_cast<float>(m_padState.X), static_cast<float>(m_padState.Y));
	}
}

/// <summary>
/// ZR,ZLボタンの入力情報を取得
/// </summary>
/// <param name="isRight">true:ZR,false:ZL</param>
/// <returns>true:押されている,false:押されていない</returns>
bool Input::GetIsPushedTriggerButton(bool isRight)const
{
	if (isRight)
	{
		//ZRの入力情報を返す
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
		//ZRの入力情報を返す
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