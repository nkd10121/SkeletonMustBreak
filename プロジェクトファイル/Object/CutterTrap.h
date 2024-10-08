#pragma once
#include "TrapBase.h"
class CutterTrap : public TrapBase
{
public:
	CutterTrap(std::shared_ptr<MyLib::Physics> physics);
	virtual ~CutterTrap();

	void Init();
	void Update()override;
	void Draw();

private:
	float m_rotationAngle;
	int m_frame;
};

