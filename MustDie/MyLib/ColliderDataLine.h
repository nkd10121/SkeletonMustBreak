#pragma once
#include "ColliderData.h"
#include "Vec3.h"

namespace MyLib
{
class ColliderDataLine : public ColliderData
{
public:
	ColliderDataLine(bool isTrigger);

	Vec3 m_startPoint;	// 始点
	Vec3 m_endPoint;	// 終点

protected:
	//ColliderDataLine(ColliderData::Kind kind, bool isTrigger);
};

}