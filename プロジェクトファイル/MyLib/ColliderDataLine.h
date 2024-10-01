#pragma once
#include "ColliderData.h"
#include "Vec3.h"

namespace MyLib
{
class ColliderDataLine : public ColliderData
{
public:
	//�R���X�g���N�^
	ColliderDataLine(bool isTrigger);

	//�n�_
	Vec3 m_startPoint;
	//�I�_
	Vec3 m_endPoint;
};
}