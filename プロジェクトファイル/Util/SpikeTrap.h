#pragma once
#include "TrapBase.h"
class SpikeTrap : public TrapBase
{
public:
	SpikeTrap(std::shared_ptr<MyLib::Physics> physics);
	virtual ~SpikeTrap();

	void Init(int handle, int subHandle = -1);
	void Update()override;
	void Draw();


private:
	int m_spikeModelHandle;

	MyLib::Vec3 m_frameModelPos;
	MyLib::Vec3 m_spikeModelPos;

	int m_isAttack;
	bool m_isInitCollision;

	int m_attackCount;
};