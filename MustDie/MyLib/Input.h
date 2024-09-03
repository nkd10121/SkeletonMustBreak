#pragma once
#include<map>
#include<string>
#include<unordered_map>

#include <DxLib.h>

enum class InputType
{
	keyboard,	//キーボード
	pad			//パッド
};

using InputTable_t = std::unordered_map<std::string, std::map<InputType, int>>;

class Input
{
public:
	Input();

	/// <summary>
	/// 入力情報を更新する
	/// </summary>
	void Update();

	/// <summary>
	/// 指定のコマンドが押された瞬間なのか
	/// </summary>
	/// <param name="command">コマンド文字列</param>
	/// <returns> true : 押された瞬間, false : 押されていないか、押しっぱ</returns>
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
	//コマンドの入力を覚えておく
	std::map < std::string, bool> m_inputData;		//現在の入力
	std::map < std::string, bool> m_lastInputData;	//直前の入力

	//コントローラーの入力情報
	DINPUT_JOYSTATE m_padState;

	bool m_isZR = false;
	bool m_isZL = false;
};
