#include "ObjectBase.h"

ObjectBase::ObjectBase(Priority priority,GameObjectTag tag):
	Collidable(priority, tag),
	m_rot(),
	m_modelHandle(-1)
{
}

ObjectBase::~ObjectBase()
{
	MV1DeleteModel(m_modelHandle);
}
