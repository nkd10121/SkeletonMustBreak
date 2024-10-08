#pragma once
#include "Game.h"

/// <summary>
/// UIのカーソルクラス
/// </summary>
class UICursor
{
public:
	//static const int kCneterX = Game::kWindowWidth / 2;
public:
	//コンストラクタ
	UICursor();
	//デストラクタ
	virtual ~UICursor();

	//初期化
	void Init();
	//更新
	void Update(int offset,int y, int centerX = 640);
	//描画
	void Draw();

private:
	int m_handle[2];

	int m_centerX;
	//カーソルを動かすための角度
	float m_angle;
	//カーソルの動き幅(可変)
	int m_cursorVariableOffsetX;
	//カーソルの動き幅(固定)
	int m_cursorFixedOffsetX;
	//カーソルのY座標
	int m_cursorPosY;
};

