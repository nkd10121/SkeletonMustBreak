#pragma once
#include "ColliderData.h"
namespace MyLib
{
class ColliderDataSphere : public ColliderData
{
public:
	//�R���X�g���N�^
	ColliderDataSphere(bool isTrigger);

	//���a
	float m_radius;

protected:
};
}