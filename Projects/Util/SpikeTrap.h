#pragma once
#include "TrapBase.h"
class SpikeTrap : public TrapBase
{
public:
	SpikeTrap(std::shared_ptr<MyLib::Physics> physics);
	virtual ~SpikeTrap()override;

	void Init();
	virtual GameObjectTag Finalize()override;
	void Update()override;
	void Draw();


private:
	//とげのモデルハンドル
	int m_spikeModelHandle;
	int m_attackCount;

	MyLib::Vec3 m_frameModelPos;
	MyLib::Vec3 m_spikeModelPos;

	bool m_isAttack;
	bool m_isInitCollision;

};