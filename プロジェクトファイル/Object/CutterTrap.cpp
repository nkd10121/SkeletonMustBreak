#include "CutterTrap.h"

namespace
{
	constexpr float kModelSize = 0.08f;

	constexpr int kCollisionResetFrame = 60;
}

CutterTrap::CutterTrap(std::shared_ptr<MyLib::Physics> physics) :
	TrapBase(GameObjectTag::CutterTrap),
	m_rotationAngle(0.0f),
	m_frame(0),
	m_isAttack(false),
	m_isInitCollision(false)
{
	m_pPhysics = physics;

	//�����蔻��̐ݒ�
	auto collider = Collidable::AddCollider(MyLib::ColliderData::Kind::Sphere, true);
	auto sphereCol = dynamic_cast<MyLib::ColliderDataSphere*>(collider.get());
	sphereCol->m_radius = 10.0f;

	//�U���͂̐ݒ�
	m_attack = 8;
}

CutterTrap::~CutterTrap()
{
	MV1DeleteModel(m_modelHandle);
}

void CutterTrap::Init(int handle, int subHandle)
{
	m_modelHandle = handle;
	MV1SetScale(m_modelHandle, VECTOR(kModelSize, kModelSize, kModelSize));
	MV1SetPosition(m_modelHandle, m_pos.ConvertToVECTOR());
}

void CutterTrap::Update()
{
	//���f������]������
	m_rotationAngle += 0.08f;
	MV1SetRotationXYZ(m_modelHandle, VECTOR(0.0f, m_rotationAngle, 0.0f));

	//���b�Ɉ��U������������Ă��Ȃ���
	if (m_frame % kCollisionResetFrame == 0)
	{
		Collidable::Init(m_pPhysics);
		rigidbody.Init();
		rigidbody.SetPos(m_pos);
	}

	if (m_frame % kCollisionResetFrame == kCollisionResetFrame-1)
	{
		Finalize(m_pPhysics);
	}

	m_frame++;
}

void CutterTrap::Draw()
{
	MV1DrawModel(m_modelHandle);
}
