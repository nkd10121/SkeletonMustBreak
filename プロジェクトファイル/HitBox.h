#pragma once
#include "ObjectBase.h"
class HitBox : public ObjectBase
{
public:
	HitBox(float r);
	~HitBox();

	void Init(std::shared_ptr<MyLib::Physics> physics, MyLib::Vec3 pos,bool isEnemy);
	void Update(MyLib::Vec3 pos);

	void OnCollideEnter(const std::shared_ptr<Collidable>& colider) {};

	void Finalize(std::shared_ptr<MyLib::Physics> physics) override;

	void OnTriggerEnter(const std::shared_ptr<Collidable>& colider)override;

	int GetIsAttackNum();
	bool GetIsHit();
	const GameObjectTag GetHitObjectTag()const { return m_hitObjectTag; };

private:
	std::shared_ptr<MyLib::Physics> m_pPhysics;

	int m_lastHitObjectAttack;
	GameObjectTag m_hitObjectTag;

	bool m_isEnemy = false;
	bool m_isHit = false;
};

