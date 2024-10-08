#include "TrapBase.h"

TrapBase::TrapBase(GameObjectTag tag):
	ObjectBase(Priority::Low,tag),
	m_pos(),
	m_attack(0)
{
}

void TrapBase::SetPos(MyLib::Vec3 pos)
{
	m_pos = pos;
}
