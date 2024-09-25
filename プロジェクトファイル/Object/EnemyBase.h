#pragma once
#include "CharacterBase.h"

class HitBox;
class SearchObject;

namespace
{
	constexpr float kDistance = 2.6f;
}

class EnemyBase : public CharacterBase
{
public:
	EnemyBase(Priority priority);
	~EnemyBase();

	virtual void Init(std::shared_ptr<MyLib::Physics>physics, std::vector<MyLib::Vec3> route) {};
	virtual void Finalize(std::shared_ptr<MyLib::Physics>) {};
	virtual void Update(MyLib::Vec3 playerPos, bool isChase) {};
	virtual void Draw() {};

	// 衝突したとき
	void OnCollideEnter(const std::shared_ptr<Collidable>& colider)override;
	void OnCollideStay(const std::shared_ptr<Collidable>& colider)override;
	void OnTriggerEnter(const std::shared_ptr<Collidable>& colider)override;

	virtual  MyLib::Vec3 GetCenterPos()const { return m_centerPos; };
	virtual  MyLib::Vec3 GetPos()const { return rigidbody.GetPos(); };
	bool GetIsHit();

	int GetDropPoint();
	bool GetIsDropedPoint()const { return m_isDroped; };
	bool GetIsDead()const { return m_isDead; };

	const bool GetIsExist()const { return m_isExist; }
	const bool GetIsReach()const { return m_isReach; }

	const GameObjectTag GetLastHitObjectTag()const { return m_lastHitObjectTag; }
	
	//レイキャストをするためにモデルハンドルを取得
	const int GetModelHandle()const { return ObjectBase::m_modelHandle; }
	const int GetHp()const { return CharacterBase::m_status.hp; }
	const int GetMaxHp()const { return m_maxHp; }

protected:
	int m_maxHp;

	std::shared_ptr<HitBox> m_hitbox;
	std::shared_ptr<SearchObject> m_search;;

	bool m_isExist;	//存在するかどうか
	bool m_isReach;	//最終目的地に着いたかどうか

	bool m_isHit;

	bool m_isDead;		//死亡したかどうか
	int m_dropPoint;	//死亡時にドロップするポイント
	bool m_isDroped;	//ポイントをドロップしたかどうか

	bool m_isAttack;	
	bool m_isKnock;

	MyLib::Vec3 m_centerPos;

	GameObjectTag m_lastHitObjectTag;
};

