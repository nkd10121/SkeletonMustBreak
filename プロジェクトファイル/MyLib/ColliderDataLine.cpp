#include "DxLib.h"
#include "MyLib.h"

/// <summary>
/// コンストラクタ
/// </summary>
MyLib::ColliderDataLine::ColliderDataLine(bool isTrigger):
	ColliderData(ColliderData::Kind::Line, isTrigger),
	m_startPoint(),
	m_endPoint()
{
}
