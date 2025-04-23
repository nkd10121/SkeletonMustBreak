#pragma once
#include "ColliderData.h"
#include "Vec3.h"

namespace MyLib
{
class ColliderDataLine : public ColliderData
{
public:
	//コンストラクタ
	ColliderDataLine(bool isTrigger);

	//始点
	Vec3 m_startPoint;
	//終点
	Vec3 m_endPoint;
};
}