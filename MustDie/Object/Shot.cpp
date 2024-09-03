#include "Shot.h"
#include "DxLib.h"
#include <string>

namespace
{
	constexpr float kRadius = 1.0f;
}

Shot::Shot() :
	Collidable(Collidable::Priority::Middle, GameObjectTag::Shot),
	m_moveDir(),
	m_isExist(false),
	m_frameCount(0),
	m_atk(0)
{
	auto collider = Collidable::AddCollider(MyLib::ColliderData::Kind::Sphere, true);
	auto sphereCol = dynamic_cast<MyLib::ColliderDataSphere*>(collider.get());
	sphereCol->m_radius = kRadius;
}

Shot::~Shot()
{
}

void Shot::Init(std::shared_ptr<MyLib::Physics> physics, int model)
{
	Collidable::Init(physics);

	// ���������̏�����
	//rigidbody.Init(true);
	rigidbody.Init();

	m_arrowModel = model;

	m_isExist = true;
}

void Shot::Finalize(std::shared_ptr<MyLib::Physics> physics)
{
	Collidable::Finalize(physics);
}

void Shot::Set(const MyLib::Vec3& pos, const MyLib::Vec3& dir, const int& atk)
{
	rigidbody.SetPos(pos);
	m_moveDir = dir;
	m_atk = atk;
}

void Shot::Update()
{
	//���݂��Ă��Ȃ���ԂȂ牽�������Ȃ�
	if (!m_isExist)return;

	m_frameCount++;

	auto dirNorm = m_moveDir.Normalize();
	rigidbody.SetVelocity(dirNorm * 1.0f);

	if (m_frameCount > 300)
	{
		m_isExist = false;
	}
}

void Shot::Draw()
{
	//���݂��Ă��Ȃ���ԂȂ牽�������Ȃ�
	if (!m_isExist)return;

	rigidbody.SetPos(rigidbody.GetNextPos());
	auto pos = rigidbody.GetPos();
	DrawSphere3D(pos.ConvertToVECTOR(), kRadius, 4, 0xff0000, 0xff0000, true);
}

void Shot::OnCollideEnter(const std::shared_ptr<Collidable>& colider)
{

}

void Shot::OnTriggerEnter(const std::shared_ptr<Collidable>& colider)
{
#ifdef _DEBUG
	std::string message = "�e��";
#endif
	auto tag = colider->GetTag();
	switch (tag)
	{
	case GameObjectTag::Player:
#ifdef _DEBUG
		message += "�v���C���[";
#endif
		break;
	case GameObjectTag::Enemy:
#ifdef _DEBUG
		message += "�G";
#endif
		m_isExist = false;
		break;
	}
#ifdef _DEBUG
	message += "�Ɠ��������I\n";
	printfDx(message.c_str());
#endif
}
