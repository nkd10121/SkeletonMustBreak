#include "HitBox.h"
#include "Shot.h"
#include "SpikeTrap.h"
#include "CutterTrap.h"

HitBox::HitBox(float r) :
	ObjectBase(Priority::Low, GameObjectTag::HitBox),
	m_lastHitObjectAttack(0)
{
	//�����蔻��̐ݒ�
	auto collider = Collidable::AddCollider(MyLib::ColliderData::Kind::Sphere, true);
	auto sphereCol = dynamic_cast<MyLib::ColliderDataSphere*>(collider.get());
	sphereCol->m_radius = r;
}

HitBox::~HitBox()
{
}

void HitBox::Init(std::shared_ptr<MyLib::Physics> physics, MyLib::Vec3 pos, bool isEnemy)
{
	m_pPhysics = physics;
	m_isEnemy = isEnemy;

	Collidable::Init(m_pPhysics);

	rigidbody.Init();
	rigidbody.SetPos(pos);
}

void HitBox::Update(MyLib::Vec3 pos)
{
	rigidbody.SetPos(pos);
}

void HitBox::Finalize(std::shared_ptr<MyLib::Physics> physics)
{
	Collidable::Finalize(physics);
}

void HitBox::OnTriggerEnter(const std::shared_ptr<Collidable>& colider)
{
	if (m_isEnemy)
	{
		//��A�g���b�v
		m_hitObjectTag = colider->GetTag();
		if (m_hitObjectTag == GameObjectTag::Shot)
		{
			{	//�Ȃ�{}���Ȃ��ƃG���[�f��
				Shot* col = dynamic_cast<Shot*>(colider.get());
				m_lastHitObjectAttack = col->GetAtk();
			}
			m_isHit = true;
		}

		if (m_hitObjectTag == GameObjectTag::SpikeTrap)
		{
			{	//�Ȃ�{}���Ȃ��ƃG���[�f��
				SpikeTrap* col = dynamic_cast<SpikeTrap*>(colider.get());
				m_lastHitObjectAttack = col->GetAtk();
			}
			m_isHit = true;
		}

		if (m_hitObjectTag == GameObjectTag::CutterTrap)
		{
			{	//�Ȃ�{}���Ȃ��ƃG���[�f��
				CutterTrap* col = dynamic_cast<CutterTrap*>(colider.get());
				m_lastHitObjectAttack = col->GetAtk();
			}
			m_isHit = true;
		}
	}

}

int HitBox::GetIsAttackNum()
{
	return m_lastHitObjectAttack;
}

bool HitBox::GetIsHit()
{
	//true��Ԃ����Ƃ�����false�ɂ���
	if (m_isHit)
	{
		m_isHit = false;
		return true;
	}

	return false;
}
