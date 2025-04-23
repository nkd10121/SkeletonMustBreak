#pragma once
#include "ObjectBase.h"
class SearchObject : public ObjectBase
{
public:
	SearchObject(float r);
	~SearchObject();

	void Init(std::shared_ptr<MyLib::Physics> physics,MyLib::Vec3 pos,bool isEnemy = false);
	void Update(MyLib::Vec3 pos);

	void OnCollideEnter(const std::shared_ptr<Collidable>& colider) {};

	void Finalize(std::shared_ptr<MyLib::Physics> physics) override;

	void OnTriggerEnter(const std::shared_ptr<Collidable>& colider)override;
	void OnTriggerStay(const std::shared_ptr<Collidable>& colider)override;

	bool GetIsTrigger();
	bool GetIsStay()const;

	void IsTriggerReset();

private:
	std::shared_ptr<MyLib::Physics> m_pPhysics;

	bool m_isEnemy = false;
	bool m_isTriggerEnter = false;
	bool m_isTriggerStay = false;
};

