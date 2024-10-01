#include "MyLib.h"
#include <cassert> 
#include "DxLib.h"
#include "MyLib.h"

/// <summary>
/// �R���X�g���N�^
/// </summary>
/// <param name="priority">�D��x</param>
/// <param name="tag">�^�O</param>
MyLib::Collidable::Collidable(Priority priority, GameObjectTag tag):
	priority(priority),
	tag(tag)
{
}

/// <summary>
/// �f�X�g���N�^
/// </summary>
MyLib::Collidable::~Collidable()
{
}

/// <summary>
/// ������(���g�ɓ����蔻���ǉ�)
/// </summary>
/// <param name="physics">�����N���X</param>
void MyLib::Collidable::Init(std::shared_ptr<MyLib::Physics> physics)
{
	physics->Entry(shared_from_this());	// �������Ɏ��g��o�^
}

/// <summary>
/// �I��(���g�̓����蔻����폜)
/// </summary>
/// <param name="physics"></param>
void MyLib::Collidable::Finalize(std::shared_ptr<MyLib::Physics> physics)
{
	physics->Exit(shared_from_this());	// �������o�^����
}

/// <summary>
/// �����蔻��𖳎��i�X���[�j����^�O�̒ǉ�
/// </summary>
/// <param name="tag">�ǉ�����^�O</param>
void MyLib::Collidable::AddThroughTag(GameObjectTag tag)
{
	//�����蔻��𖳎�����^�O�̃��X�g�ɒǉ��\��̃^�O�����݂��邩���m�F
	bool found = (std::find(throughTags.begin(), throughTags.end(), tag) != throughTags.end());
	//���X�g�̒��Ɋ��ɑ��݂��Ă�����x�����o��
	if (found)
	{
		assert(0 && "�w��^�O�͊��ɒǉ�����Ă��܂�");
	}
	else //���݂��Ă��Ȃ�������ǉ�����
	{
		throughTags.emplace_back(tag);
	}
}

/// <summary>
/// �����蔻��𖳎��i�X���[�j����^�O�̍폜
/// </summary>
/// <param name="tag">�폜����^�O</param>
void MyLib::Collidable::RemoveThroughTag(GameObjectTag tag)
{
	//�����蔻��𖳎�����^�O�̃��X�g�ɒǉ��\��̃^�O�����݂��邩���m�F
	bool found = (std::find(throughTags.begin(), throughTags.end(), tag) != throughTags.end());
	//���݂��Ă��Ȃ�������x�����o��
	if (!found)
	{
		assert(0 && "�w��^�O�͑��݂��܂���");
	}
	else //���݂��Ă�����폜����
	{
		throughTags.remove(tag);
	}
}

/// <summary>
/// �����蔻��𖳎��i�X���[�j����Ώۂ��ǂ���
/// </summary>
/// <param name="target">��������Ώۂ��m�F�����������蔻��</param>
/// <returns></returns>
bool MyLib::Collidable::IsThroughTarget(const std::shared_ptr<Collidable> target) const
{
	//�m�F�����������蔻��̃^�O����������^�O�̃��X�g�Ɋ܂܂�Ă��邩�ǂ������ׂ�
	bool found = (std::find(throughTags.begin(), throughTags.end(), target->GetTag()) != throughTags.end());
	return found;
}

/// <summary>
/// �����蔻���ǉ�
/// </summary>
/// <param name="kind">�����蔻��̎��</param>
/// <param name="isTrigger">�����o����������邩�ǂ���</param>
/// <returns></returns>
std::shared_ptr<MyLib::ColliderData> MyLib::Collidable::AddCollider(const ColliderData::Kind& kind,bool isTrigger)
{
	std::shared_ptr<ColliderData> add;

	//��ނɂ���Ēǉ����铖���蔻��̔h�����ύX����
	if (kind == ColliderData::Kind::Sphere)
	{
		add = std::make_shared<ColliderDataSphere>(isTrigger);
	}
	else if (kind == ColliderData::Kind::Line)
	{
		add = std::make_shared<ColliderDataLine>(isTrigger);
	}
	else
	{
		assert(0 && "�w�肳�ꂽ��ނ̓����蔻���ǉ��ł��܂���ł���");
	}

	//�����蔻���ǉ�����
	m_colliders.emplace_back(add);

	return add;
}
