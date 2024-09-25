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

	// �Փ˂����Ƃ�
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
	
	//���C�L���X�g�����邽�߂Ƀ��f���n���h�����擾
	const int GetModelHandle()const { return ObjectBase::m_modelHandle; }
	const int GetHp()const { return CharacterBase::m_status.hp; }
	const int GetMaxHp()const { return m_maxHp; }

protected:
	int m_maxHp;

	std::shared_ptr<HitBox> m_hitbox;
	std::shared_ptr<SearchObject> m_search;;

	bool m_isExist;	//���݂��邩�ǂ���
	bool m_isReach;	//�ŏI�ړI�n�ɒ��������ǂ���

	bool m_isHit;

	bool m_isDead;		//���S�������ǂ���
	int m_dropPoint;	//���S���Ƀh���b�v����|�C���g
	bool m_isDroped;	//�|�C���g���h���b�v�������ǂ���

	bool m_isAttack;	
	bool m_isKnock;

	MyLib::Vec3 m_centerPos;

	GameObjectTag m_lastHitObjectTag;
};

