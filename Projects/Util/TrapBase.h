#pragma once
#include "ObjectBase.h"
#include "MyLib.h"

#include "SearchObject.h"


class TrapBase :public ObjectBase
{
public:
	//ステータス構造体
	struct Status
	{
		int atk;
		float searchRange;
		float atkRange;
		int coolTime;
		int cost;
	};
public:
	TrapBase(GameObjectTag tag);
	virtual ~TrapBase() {};

	virtual void Init() {};
	virtual GameObjectTag Finalize();
	virtual void Update() {};
	virtual void Draw() {};

	// 衝突したとき
	void OnCollideEnter(const std::shared_ptr<Collidable>& colider) {};

	MyLib::Vec3 GetPos() { return m_pos; };
	void SetPos(MyLib::Vec3 pos);

	int GetAtk() { return m_attack; }

	//存在しているかを取得
	const bool GetIsExist()const { return m_isExist; }

protected:
	Status status;

	MyLib::Vec3 m_pos;

	std::shared_ptr<MyLib::Physics> m_pPhysics;
	std::shared_ptr<SearchObject> m_pSearch;

	int m_attack;

	int m_isExist;	//存在フラグ

};

