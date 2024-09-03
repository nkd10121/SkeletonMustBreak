#include "MyLib.h"
#include "DxLib.h"

std::list<MyLib::DebugDraw::SphereInfo>	MyLib::DebugDraw::m_sphereInfo;

void MyLib::DebugDraw::Clear()
{
	m_sphereInfo.clear();
}

void MyLib::DebugDraw::Draw3D()
{
	for (auto& sphere : m_sphereInfo)
	{
		DrawSphere3D(sphere.center.ConvertToVECTOR(), sphere.radius, 8, sphere.color, sphere.color, false);
	}
}

void MyLib::DebugDraw::DrawSphere(const Vec3& center, const float& radius, const unsigned int& color)
{
	SphereInfo addInfo;
	addInfo.center = center;
	addInfo.radius = radius;
	addInfo.color = color;
	m_sphereInfo.emplace_back(addInfo);
}
