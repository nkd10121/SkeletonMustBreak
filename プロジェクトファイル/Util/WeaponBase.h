#pragma once
#include "ObjectBase.h"
class WeaponBase : public ObjectBase
{
public:
	WeaponBase();
	virtual ~WeaponBase();

	void Init(int weaponModelHandle,int ownerModelHandle, const TCHAR* franeName, float modelSize = 1.0f);
	void Update(MyLib::Vec3 moveVec);
	void Draw();

	void SetAtk(int atk) { m_atk = atk; }
	const int GetAtk()const { return m_atk; }

	void CollisionInit(std::shared_ptr<MyLib::Physics> physics);
	void CollisionEnd();

	const bool GetIsCollisionOn()const { return m_isCollisionOn; }

private:
	void SetAttachFrame(const TCHAR* franeName);

	void ConvertMatToVec();

protected:
	//���f���T�C�Y�̕ύX
	void SetModelSize(float modelSize);

	// �Փ˂����Ƃ�
	void OnCollideEnter(const std::shared_ptr<Collidable>& colider);
	void OnTriggerEnter(const std::shared_ptr<Collidable>& colider)override;

private:
	//���탂�f���n���h��
	int m_weaponModelHandle;
	int m_ownerModelHandle;

	std::shared_ptr<MyLib::Physics> m_pPhysics;

	//�������������ꏊ
	int m_weaponAttachFrame;
	//���f���T�C�Y
	float m_modelScale;

	MATRIX m_mat;

	int m_atk;

	//�����蔻��̐؂�ւ�
	bool m_isCollisionOn;

};

