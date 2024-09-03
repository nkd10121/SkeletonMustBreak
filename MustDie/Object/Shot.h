#pragma once
#include "MyLib.h"

class Input;

class Shot : public MyLib::Collidable
{
public:
	Shot();
	virtual ~Shot();

	void Init(std::shared_ptr<MyLib::Physics> physics, int model);
	void Finalize(std::shared_ptr<MyLib::Physics> physics) override;
	void Set(const MyLib::Vec3& pos,const MyLib::Vec3& dir,const int& atk);
	void Update();
	void Draw();

	// è’ìÀÇµÇΩÇ∆Ç´
	void OnCollideEnter(const std::shared_ptr<Collidable>& colider)override;
	void OnTriggerEnter(const std::shared_ptr<Collidable>& colider)override;


	const bool GetIsExist()const { return m_isExist; }
	const int GetAtk()const { return m_atk; }

private:
	MyLib::Vec3 m_moveDir;
	bool m_isExist;

	int m_arrowModel;

	int m_frameCount;

	int m_atk;
};

