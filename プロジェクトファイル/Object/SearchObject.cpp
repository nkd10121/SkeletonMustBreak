#include "SearchObject.h"

SearchObject::SearchObject(float r) :
	ObjectBase(Priority::Low,GameObjectTag::Search)
{
	//当たり判定の設定
	auto collider = Collidable::AddCollider(MyLib::ColliderData::Kind::Sphere, true);
	auto sphereCol = dynamic_cast<MyLib::ColliderDataSphere*>(collider.get());
	sphereCol->m_radius = r;
}

SearchObject::~SearchObject()
{
}

void SearchObject::Init(std::shared_ptr<MyLib::Physics> physics, MyLib::Vec3 pos, bool isEnemy)
{
	m_pPhysics = physics;
	m_isEnemy = isEnemy;

	Collidable::Init(m_pPhysics);

	rigidbody.Init();
	rigidbody.SetPos(pos);
}

void SearchObject::Update(MyLib::Vec3 pos)
{
	m_isTriggerStay = false;
	rigidbody.SetPos(pos);
}

void SearchObject::Finalize(std::shared_ptr<MyLib::Physics> physics)
{
	Collidable::Finalize(physics);
}

void SearchObject::OnTriggerEnter(const std::shared_ptr<Collidable>& colider)
{
	//アタッチしたオブジェクトが敵じゃないなら
	if (!m_isEnemy)
	{
		auto tag = colider->GetTag();
		if (tag == GameObjectTag::Enemy)
		{
			m_isTriggerEnter = true;
		}
	}
	else	//アタッチしたオブジェクトが敵なら
	{
		auto tag = colider->GetTag();
		if (tag == GameObjectTag::Player)
		{
			m_isTriggerEnter = true;
		}
	}
}

void SearchObject::OnTriggerStay(const std::shared_ptr<Collidable>& colider)
{

	//アタッチしたオブジェクトが敵じゃないなら
	if (!m_isEnemy)
	{
		auto tag = colider->GetTag();
		if (tag == GameObjectTag::Enemy)
		{
			m_isTriggerEnter = true;
		}
	}
	else	//アタッチしたオブジェクトが敵なら
	{
		auto tag = colider->GetTag();

		if (tag == GameObjectTag::Player)
		{
			m_isTriggerStay = true;
		}
	}
}

bool SearchObject::GetIsTrigger()
{
	////trueを返したときだけfalseにする
	//if (m_isTriggerEnter)
	//{
	//	m_isTriggerEnter = false;
	//	return true;
	//}

	//return false;

	return m_isTriggerEnter;
}

bool SearchObject::GetIsStay() const
{
	return m_isTriggerStay;
}

void SearchObject::IsTriggerReset()
{
	m_isTriggerEnter = false;
}
