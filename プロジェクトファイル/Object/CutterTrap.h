#pragma once
#include "TrapBase.h"
class CutterTrap : public TrapBase
{
public:
	CutterTrap(std::shared_ptr<MyLib::Physics> physics);
	virtual ~CutterTrap();

	void Init(int handle, int subHandle = -1);
	void Update()override;
	void Draw();

private:
	MyLib::Vec3 m_frameModelPos;
	MyLib::Vec3 m_spikeModelPos;

	float m_rotationAngle;

	int m_frame;

	int m_isAttack;
	bool m_isInitCollision;
};

