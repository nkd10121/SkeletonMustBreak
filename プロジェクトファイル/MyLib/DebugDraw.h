#pragma once
#include <List>
#include "Vec3.h"

namespace MyLib
{

/// <summary>
/// デバッグ用の描画情報をまとめ、後で表示するためのクラス
/// </summary>
class DebugDraw
{
public:
	static void Clear();
	static void Draw3D();

	static void DrawSphere(const Vec3& center,const float& radius,const unsigned int& color);
private:
	struct SphereInfo
	{
		Vec3 center;
		float radius;
		unsigned int color;
	};

	static std::list<SphereInfo> m_sphereInfo;
};

}