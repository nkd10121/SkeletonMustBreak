#include "UICursor.h"
#include "DxLib.h"
#include <cmath>

namespace
{
	//ハンドル
	enum eHandle
	{
		rightArrow,
		leftArrow,
	};

	//カーソルの移動速度
	constexpr float kMoveSpeed = 0.14f;
	//カーソルの移動幅
	constexpr float kMoveRange = 8.0f;
}

/// <summary>
/// コンストラクタ
/// </summary>
UICursor::UICursor():
	m_handle(),
	m_centerX(0),
	m_angle(0),
	m_cursorVariableOffsetX(0),
	m_cursorFixedOffsetX(0),
	m_cursorPosY(0)
{
	//カーソルの画像をロード
	m_handle[eHandle::rightArrow] = LoadGraph("data/img/ArrowRight.png");
	m_handle[eHandle::leftArrow] = LoadGraph("data/img/ArrowLeft.png");
}

/// <summary>
/// デストラクタ
/// </summary>
UICursor::~UICursor()
{
	for (auto& h : m_handle)
	{
		DeleteGraph(h);
	}
}

/// <summary>
/// 初期化
/// </summary>
void UICursor::Init()
{
}

/// <summary>
/// 更新
/// </summary>
void UICursor::Update(int offset, int y, int centerX)
{
	m_centerX = centerX;
	//固定の動き幅を設定する
	m_cursorFixedOffsetX = offset;
	//高さを設定する
	m_cursorPosY = y;

	//可変の動き幅を計算する
	m_angle += kMoveSpeed;
	m_cursorVariableOffsetX = static_cast<int>(sinf(m_angle) * kMoveRange);
}

/// <summary>
/// 描画
/// </summary>
void UICursor::Draw()
{
	int offset = m_cursorFixedOffsetX + m_cursorVariableOffsetX;
	//カーソルを描画
	DrawRotaGraph(m_centerX + offset, m_cursorPosY, 1.0f, 0.0f, m_handle[leftArrow], true);
	DrawRotaGraph(m_centerX - offset, m_cursorPosY, 1.0f, 0.0f, m_handle[rightArrow], true);
}
