#include "DxLib.h"
#include "MyLib.h"

/// <summary>
/// �R���X�g���N�^
/// </summary>
MyLib::ColliderDataLine::ColliderDataLine(bool isTrigger):
	ColliderData(ColliderData::Kind::Line, isTrigger),
	m_startPoint(),
	m_endPoint()
{
}
