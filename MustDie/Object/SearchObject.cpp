#include "SearchObject.h"

SearchObject::SearchObject(float r) :
	ObjectBase(Priority::Low,GameObjectTag::Search)
{
	//�����蔻��̐ݒ�
	auto collider = Collidable::AddCollider(MyLib::ColliderData::Kind::Sphere, true);
	auto sphereCol = dynamic_cast<MyLib::ColliderDataSphere*>(collider.get());
	sphereCol->m_radius = r;
}

SearchObject::~SearchObject()
{
}

void SearchObject::Init(std::shared_ptr<MyLib::Physics> physics, MyLib::Vec3 pos)
{
	m_pPhysics = physics;

	Collidable::Init(m_pPhysics);

	rigidbody.Init();
	rigidbody.SetPos(pos);
}

void SearchObject::Update()
{
}

void SearchObject::OnTriggerEnter(const std::shared_ptr<Collidable>& colider)
{
	auto tag = colider->GetTag();
	if (tag == GameObjectTag::Enemy)
	{
		m_isTriggerEnterEnemy = true;
	}
}

bool SearchObject::GetIsTriggerEnemy()
{
	//true��Ԃ����Ƃ�����false�ɂ���
	if (m_isTriggerEnterEnemy)
	{
		m_isTriggerEnterEnemy = false;
		return true;
	}

	return false;
}
