#pragma once
#include "ObjectBase.h"
#include "MyLib.h"

#include "SearchObject.h"


class TrapBase :public ObjectBase
{
public:
	TrapBase(GameObjectTag tag);
	virtual ~TrapBase() {};

	virtual void Init(int handle,int subHandle = -1) {};
	virtual void Update() {};
	virtual void Draw() {};

	// 衝突したとき
	void OnCollideEnter(const std::shared_ptr<Collidable>& colider) {};

	MyLib::Vec3 GetPos() { return m_pos; };
	void SetPos(MyLib::Vec3 pos);

	int GetAtk() { return m_attack; }

protected:
	MyLib::Vec3 m_pos;

	std::shared_ptr<MyLib::Physics> m_pPhysics;
	std::shared_ptr<SearchObject> m_pSearch;

	int m_attack;

};

