#include "WeaponBase.h"

WeaponBase::WeaponBase() :
	ObjectBase(Priority::Low, GameObjectTag::Sword),
	m_ownerModelHandle(-1),
	m_weaponAttachFrame(-1),
	m_modelScale(1.0f),
	m_mat(),
	m_isCollisionOn(false)
{
	//当たり判定の設定
	auto collider = Collidable::AddCollider(MyLib::ColliderData::Kind::Sphere, true);
	auto sphereCol = dynamic_cast<MyLib::ColliderDataSphere*>(collider.get());
	sphereCol->m_radius = 3.0f;
}

WeaponBase::~WeaponBase()
{
	MV1DeleteModel(m_ownerModelHandle);
}

void WeaponBase::Init(int ownerModelHandle, const TCHAR* franeName,float modelSize)
{
	m_ownerModelHandle = ownerModelHandle;
	SetAttachFrame(franeName);
	SetModelSize(modelSize);
}

void WeaponBase::Update(MyLib::Vec3 moveVec)
{
	auto positionMat = MV1GetFrameLocalWorldMatrix(m_ownerModelHandle, m_weaponAttachFrame);
	auto rotationMat = MGetRotY(DX_PI_F);
	MATRIX temp = MMult(rotationMat, positionMat);
	auto scaleMat = MGetScale(VGet(m_modelScale, m_modelScale, m_modelScale));
	m_mat = MMult(scaleMat, temp);
	MV1SetMatrix(m_modelHandle, m_mat);

	rigidbody.SetPos(moveVec);
}

void WeaponBase::Draw()
{
	MV1DrawModel(m_modelHandle);
}

void WeaponBase::InitCollision(std::shared_ptr<MyLib::Physics> physics)
{
	m_isCollisionOn = true;
	m_pPhysics = physics;

	Collidable::Init(physics);

	// 物理挙動の初期化
	//rigidbody.Init(true);
	rigidbody.Init();
}

void WeaponBase::CollisionEnd()
{
	if (m_isCollisionOn)
	{
		m_isCollisionOn = false;
		Finalize(m_pPhysics);
	}
}

void WeaponBase::SetAttachFrame(const TCHAR* franeName)
{
	m_weaponAttachFrame = MV1SearchFrame(m_ownerModelHandle, franeName);
}

void WeaponBase::ConvertMatToVec()
{
	rigidbody.SetPos(MyLib::Vec3(m_mat.m[3][0], m_mat.m[3][1], m_mat.m[3][2]));
}

void WeaponBase::SetModelSize(float modelSize)
{
	m_modelScale = modelSize;
}

void WeaponBase::OnCollideEnter(const std::shared_ptr<Collidable>& colider)
{

}

void WeaponBase::OnTriggerEnter(const std::shared_ptr<Collidable>& colider)
{
	if (colider->GetTag() == GameObjectTag::Player)
	{
		if (m_isCollisionOn)
		{
			CollisionEnd();
			m_isCollisionOn = false;
		}
	}
}
