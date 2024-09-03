#pragma once
#include "CharacterBase.h"

class EnemyBase : public CharacterBase
{
public:
	EnemyBase(Priority priority);
	~EnemyBase();

	virtual void Init(std::shared_ptr<MyLib::Physics>physics, std::vector<MyLib::Vec3> route) {};

	// è’ìÀÇµÇΩÇ∆Ç´
	void OnCollideEnter(const std::shared_ptr<Collidable>& colider)override;
	void OnCollideStay(const std::shared_ptr<Collidable>& colider)override;
	void OnTriggerEnter(const std::shared_ptr<Collidable>& colider)override;

	const bool GetIsExist()const { return m_isExist; }
	const bool GetIsReach()const { return m_isReach; }

protected:
	bool m_isExist;
	bool m_isReach;

	bool m_isAttack;
	bool m_isKnock;

private:

};

