#pragma once
#include "ObjectBase.h"

class CharacterBase : public ObjectBase
{
public:
	//�X�e�[�^�X�\����
	struct Status
	{
		int hp;		//�̗�
		int atk;	//�U����
		int def;	//�h���
		float speed;	//�ړ����x
		int point;	//���S���̃|�C���g
	};
public:
	CharacterBase(Priority priority, GameObjectTag tag);
	virtual ~CharacterBase();

	// �Փ˂����Ƃ�
	virtual void OnCollideEnter(const std::shared_ptr<Collidable>& colider) = 0;

protected:

protected:
	//�ړ��x�N�g��
	MyLib::Vec3 m_modelPos;
	MyLib::Vec3 m_collisionPos;
	MyLib::Vec3 m_moveVec;
	Status m_status;
};

