#include "DxLib.h"
#include "MyLib.h"

MyLib::ColliderDataSphere::ColliderDataSphere(bool isTrigger):
	ColliderData(ColliderData::Kind::Sphere, isTrigger),
	m_radius(0)
{
}
