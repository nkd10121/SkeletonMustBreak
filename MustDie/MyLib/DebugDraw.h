#pragma once
#include <List>
#include "Vec3.h"

namespace MyLib
{

/// <summary>
/// �f�o�b�O�p�̕`������܂Ƃ߁A��ŕ\�����邽�߂̃N���X
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