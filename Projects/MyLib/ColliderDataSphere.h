#pragma once
#include "ColliderData.h"
namespace MyLib
{
class ColliderDataSphere : public ColliderData
{
public:
	//コンストラクタ
	ColliderDataSphere(bool isTrigger);

	//半径
	float m_radius;

protected:
};
}