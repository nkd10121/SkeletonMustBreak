#include "TrapBase.h"

TrapBase::TrapBase(GameObjectTag tag):
	ObjectBase(Priority::Low,tag),
	m_pos(),
	m_attack(0),
	m_isExist(false)
{
}

GameObjectTag TrapBase::Finalize()
{
	//存在していない状態にする
	m_isExist = false;

	return Collidable::GetTag();
}

void TrapBase::SetPos(MyLib::Vec3 pos)
{
	m_pos = pos;
}
