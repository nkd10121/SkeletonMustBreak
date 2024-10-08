#pragma once
#include "TrapBase.h"
class HammerTrap : public TrapBase
{
public:
	HammerTrap(std::shared_ptr<MyLib::Physics> physics);
	virtual ~HammerTrap();

	void Init(int handle, int subHandle = -1);
	void Update()override;
	void Draw();


private:
	MyLib::Vec3 m_frameModelPos;
	MyLib::Vec3 m_spikeModelPos;

	int m_isAttack;
	bool m_isInitCollision;
};

