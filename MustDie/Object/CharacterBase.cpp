#include "CharacterBase.h"

CharacterBase::CharacterBase(Priority priority, GameObjectTag tag):
	ObjectBase(priority,tag),
	m_modelPos(),
	m_collisionPos(),
	m_moveVec(),
	m_status()
{
}

CharacterBase::~CharacterBase()
{
}
