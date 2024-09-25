#include "HitBox.h"
#include "Shot.h"
#include "SpikeTrap.h"
#include "CutterTrap.h"

HitBox::HitBox(float r) :
	ObjectBase(Priority::Low, GameObjectTag::HitBox),
	m_lastHitObjectAttack(0)
{
	//当たり判定の設定
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
		//矢、トラップ
		m_hitObjectTag = colider->GetTag();
		if (m_hitObjectTag == GameObjectTag::Shot)
		{
			{	//なんか{}がないとエラー吐く
				Shot* col = dynamic_cast<Shot*>(colider.get());
				m_lastHitObjectAttack = col->GetAtk();
			}
			m_isHit = true;
		}

		if (m_hitObjectTag == GameObjectTag::SpikeTrap)
		{
			{	//なんか{}がないとエラー吐く
				SpikeTrap* col = dynamic_cast<SpikeTrap*>(colider.get());
				m_lastHitObjectAttack = col->GetAtk();
			}
			m_isHit = true;
		}

		if (m_hitObjectTag == GameObjectTag::CutterTrap)
		{
			{	//なんか{}がないとエラー吐く
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
	//trueを返したときだけfalseにする
	if (m_isHit)
	{
		m_isHit = false;
		return true;
	}

	return false;
}
