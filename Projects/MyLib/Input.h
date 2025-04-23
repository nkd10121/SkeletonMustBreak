#pragma once
#include<map>
#include<string>
#include<unordered_map>

#include <DxLib.h>

class Input
{
private:
	//入力デバイス
	enum class InputType
	{
		keyboard,	//キーボード
		pad			//パッド
	};

	//入力情報の型
	using InputTable_t = std::unordered_map<std::string, std::map<InputType, int>>;

public:
	//コンストラクタ
	Input();

	//入力情報を更新する
	void Update();

	//指定のコマンドが押された瞬間なのか
	bool IsTriggered(const char* command)const;

	//指定のコマンドが押されている状態なのか
	bool IsPushed(const char* command)const;

	//スティックの入力情報を取得
	std::pair<float, float> GetInputStick(bool isRight)const;

	//ZR,ZLボタンの入力情報を取得
	bool GetIsPushedTriggerButton(bool isRight)const;

private:
	//コマンド
	InputTable_t m_commandTable;

	//コマンドの入力を覚えておく
	std::map < std::string, bool> m_inputData;		//現在の入力
	std::map < std::string, bool> m_lastInputData;	//直前の入力

	//コントローラーの入力情報
	DINPUT_JOYSTATE m_padState;
	XINPUT_STATE* XInputState;
};
