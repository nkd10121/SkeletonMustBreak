#pragma once
#include "ColliderData.h"
namespace MyLib
{
class ColliderDataSphere : public ColliderData
{
public:
	ColliderDataSphere(bool isTrigger);

	float m_radius;

protected:
};
}


