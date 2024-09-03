#pragma once
#include "CharacterBase.h"

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

	int GetDropPoint();
	bool GetIsDropedPoint()const { return m_isDroped; };
	bool GetIsDead()const { return m_isDead; };

	const bool GetIsExist()const { return m_isExist; }
	const bool GetIsReach()const { return m_isReach; }

protected:
	bool m_isExist;	//���݂��邩�ǂ���
	bool m_isReach;	//�ŏI�ړI�n�ɒ��������ǂ���

	bool m_isDead;		//���S�������ǂ���
	int m_dropPoint;	//���S���Ƀh���b�v����|�C���g
	bool m_isDroped;	//�|�C���g���h���b�v�������ǂ���

	bool m_isAttack;	
	bool m_isKnock;

private:

};

