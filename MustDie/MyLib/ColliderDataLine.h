#pragma once
#include "ColliderData.h"
#include "Vec3.h"

namespace MyLib
{
class ColliderDataLine : public ColliderData
{
public:
	ColliderDataLine(bool isTrigger);

	Vec3 m_startPoint;	// �n�_
	Vec3 m_endPoint;	// �I�_

protected:
	//ColliderDataLine(ColliderData::Kind kind, bool isTrigger);
};

}