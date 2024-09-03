#pragma once
#include "ObjectBase.h"
class SearchObject : public ObjectBase
{
public:
	SearchObject(float r);
	~SearchObject();

	void Init(std::shared_ptr<MyLib::Physics> physics,MyLib::Vec3 pos);
	void Update();

	void OnCollideEnter(const std::shared_ptr<Collidable>& colider) {};


	void OnTriggerEnter(const std::shared_ptr<Collidable>& colider)override;

	bool GetIsTriggerEnemy();

private:
	std::shared_ptr<MyLib::Physics> m_pPhysics;

	bool m_isTriggerEnterEnemy = false;
};

