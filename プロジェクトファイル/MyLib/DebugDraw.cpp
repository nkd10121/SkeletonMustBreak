#include "MyLib.h"
#include "DxLib.h"

std::list<MyLib::DebugDraw::SphereInfo>	MyLib::DebugDraw::m_sphereInfo;

/// <summary>
/// 描画情報の削除
/// </summary>
void MyLib::DebugDraw::Clear()
{
	//球体の描画情報リストの中身を全削除
	m_sphereInfo.clear();
}

/// <summary>
/// 描画
/// </summary>
void MyLib::DebugDraw::Draw3D()
{
#ifdef _DEBUG
	//球体の描画情報リストにある情報分描画する
	for (auto& sphere : m_sphereInfo)
	{
		DrawSphere3D(sphere.center.ConvertToVECTOR(), sphere.radius, 8, sphere.color, sphere.color, false);
	}
#endif
}

/// <summary>
/// 球体の描画リストに追加する
/// </summary>
/// <param name="center">中心座標</param>
/// <param name="radius">半径</param>
/// <param name="color">色</param>
void MyLib::DebugDraw::AddDrawSphere(const Vec3& center, const float& radius, const unsigned int& color)
{
	//球体の描画リストに追加する
	SphereInfo addInfo;
	addInfo.center = center;
	addInfo.radius = radius;
	addInfo.color = color;
	m_sphereInfo.emplace_back(addInfo);
}
