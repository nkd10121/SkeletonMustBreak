#pragma once
#include "TrapBase.h"
class CutterTrap : public TrapBase
{
public:
	CutterTrap(std::shared_ptr<MyLib::Physics> physics);
	virtual ~CutterTrap()override;

	void Init();
	virtual GameObjectTag Finalize()override;
	void Update()override;
	void Draw()override;

private:
	float m_rotationAngle;
	int m_frame;
};

