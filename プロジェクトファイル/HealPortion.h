#pragma once
#include "ObjectBase.h"
class HealPortion : public ObjectBase
{
public:
	HealPortion();
	virtual ~HealPortion();

	void Init(std::shared_ptr<MyLib::Physics> physics, int model);
	void Finalize(std::shared_ptr<MyLib::Physics> physics) override;
	void Update();
	void Draw();

	void OnCollideEnter(const std::shared_ptr<Collidable>& colider) {};

	const bool GetIsExist()const { return m_isExist; }
	void SetPosition(MyLib::Vec3 pos);

	void End();

private:
	bool m_isExist;

	int m_frame;

	float m_angle;
	float m_posOffsetY;
};

