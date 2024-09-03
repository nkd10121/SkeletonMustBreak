#pragma once
#include "GameObjectTag.h"
#include <List>
#include <memory>

namespace MyLib
{

class Rigidbody;
class Physics;
class ColliderData;

class Collidable : public std::enable_shared_from_this<Collidable>
{
	friend Physics;
public:

	// �ʒu�␳�̗D��x�̔��ʂɎg��
	enum class Priority : int
	{
		Low,		// ��
		Middle,		// ��
		High,		// ��
		Static,		// �����Ȃ��i�ō��j
	};

public:
	Collidable(Priority priority, GameObjectTag tag);	// �R���X�g���N�^
	virtual ~Collidable();													// �f�X�g���N�^
	virtual void Init(std::shared_ptr<MyLib::Physics> physics);
	virtual void Finalize(std::shared_ptr<MyLib::Physics> physics);

	//virtual void OnCollide(const Collidable& colider) abstract;				// �Փ˂����Ƃ�

	virtual void OnCollideEnter(const std::shared_ptr<Collidable>& colider) {}
	virtual void OnCollideStay(const std::shared_ptr<Collidable>& colider) {}
	virtual void OnCollideExit(const std::shared_ptr<Collidable>& colider) {}
	virtual void OnTriggerEnter(const std::shared_ptr<Collidable>& colider) {}
	virtual void OnTriggerStay(const std::shared_ptr<Collidable>& colider) {}
	virtual void OnTriggerExit(const std::shared_ptr<Collidable>& colider) {}

	GameObjectTag	GetTag()		const { return tag; }					// �^�O���
	Priority		GetPriority()	const { return priority; }				// �D��x

	// �����蔻��𖳎��i�X���[�j����^�O�̒ǉ�/�폜
	void AddThroughTag(GameObjectTag tag);
	void RemoveThroughTag(GameObjectTag tag);

	// �����蔻��𖳎��i�X���[�j����Ώۂ��ǂ���
	bool IsThroughTarget(const std::shared_ptr<Collidable> target) const;

protected:
	Rigidbody		rigidbody;		// �����f�[�^
	std::list<std::shared_ptr<ColliderData>> m_colliders;

	std::shared_ptr<ColliderData> AddCollider(const ColliderData::Kind& kind, bool isTrigger);
private:
	//ColliderData* CreateColliderData(ColliderData::Kind kind, bool isTrigger);



	GameObjectTag	tag;
	Priority		priority;



	// �����蔻��𖳎��i�X���[�j����^�O�̃��X�g
	// HACK: Unity�݂����Ƀ��C���[�̒ǉ��̕����X�}�[�g
	// FIXME: �{��ColliderData������Ȃ��H
	std::list<GameObjectTag>	throughTags;

	// Physics��Collidable�����R�ɊǗ����邽�߂Ƀt�����h��

	// �ȉ��A�t�����h�ł���Phisics�݂̂������^��ϐ�
private:
	// FIXME: �����Physics���������]�܂���
};
}


