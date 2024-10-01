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
	//�R���X�g���N�^
	Collidable(Priority priority, GameObjectTag tag);	
	//�f�X�g���N�^
	virtual ~Collidable();												
	//������
	virtual void Init(std::shared_ptr<MyLib::Physics> physics);
	//�I��
	virtual void Finalize(std::shared_ptr<MyLib::Physics> physics);

	//�����蔻��֐�
	virtual void OnCollideEnter(const std::shared_ptr<Collidable>& colider) {}
	virtual void OnCollideStay(const std::shared_ptr<Collidable>& colider) {}
	virtual void OnCollideExit(const std::shared_ptr<Collidable>& colider) {}
	virtual void OnTriggerEnter(const std::shared_ptr<Collidable>& colider) {}
	virtual void OnTriggerStay(const std::shared_ptr<Collidable>& colider) {}
	virtual void OnTriggerExit(const std::shared_ptr<Collidable>& colider) {}

	//�^�O���擾
	GameObjectTag GetTag() const { return tag; }					
	//�D��x���擾
	Priority GetPriority() const { return priority; }				

	//�����蔻��𖳎��i�X���[�j����^�O�̒ǉ�/�폜
	void AddThroughTag(GameObjectTag tag);
	void RemoveThroughTag(GameObjectTag tag);

	//�����蔻��𖳎��i�X���[�j����Ώۂ��ǂ���
	bool IsThroughTarget(const std::shared_ptr<Collidable> target) const;

protected:
	//�����蔻���ǉ�
	std::shared_ptr<ColliderData> AddCollider(const ColliderData::Kind& kind, bool isTrigger);

protected:
	// �����f�[�^
	Rigidbody rigidbody;		
	//�����蔻����
	std::list<std::shared_ptr<ColliderData>> m_colliders;

private:
	//�^�O
	GameObjectTag tag;
	//�D��x
	Priority priority;

	// �����蔻��𖳎��i�X���[�j����^�O�̃��X�g
	std::list<GameObjectTag>	throughTags;
};
}