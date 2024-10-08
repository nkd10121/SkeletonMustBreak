#include "DxLib.h"
#include "MyLib.h"

/// <summary>
/// コンストラクタ
/// </summary>
/// <param name="isTrigger"></param>
MyLib::ColliderDataSphere::ColliderDataSphere(bool isTrigger):
	ColliderData(ColliderData::Kind::Sphere, isTrigger),
	m_radius(0)
{
}