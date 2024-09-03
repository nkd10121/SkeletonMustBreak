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

	// 物理挙動の初期化
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
	//存在していない状態なら何もさせない
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
	//存在していない状態なら何もさせない
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
	std::string message = "弾が";
#endif
	auto tag = colider->GetTag();
	switch (tag)
	{
	case GameObjectTag::Player:
#ifdef _DEBUG
		message += "プレイヤー";
#endif
		break;
	case GameObjectTag::Enemy:
#ifdef _DEBUG
		message += "敵";
#endif
		m_isExist = false;
		break;
	}
#ifdef _DEBUG
	message += "と当たった！\n";
	printfDx(message.c_str());
#endif
}
