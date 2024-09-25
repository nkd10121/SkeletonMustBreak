#include "HammerTrap.h"

namespace
{
	constexpr float kModelSize = 10.0f;
}

HammerTrap::HammerTrap(std::shared_ptr<MyLib::Physics> physics) :
	TrapBase(GameObjectTag::SpikeTrap),
	m_frameModelPos(),
	m_spikeModelPos(),
	m_isAttack(false),
	m_isInitCollision(false)
{
	m_pPhysics = physics;

	//�����蔻��̐ݒ�
	auto collider = Collidable::AddCollider(MyLib::ColliderData::Kind::Sphere, true);
	auto sphereCol = dynamic_cast<MyLib::ColliderDataSphere*>(collider.get());
	sphereCol->m_radius = 8.0f;

	m_pSearch = std::make_shared<SearchObject>(5.5f);

	//�U���͂̐ݒ�
	m_attack = 20;
}

HammerTrap::~HammerTrap()
{
	MV1DeleteModel(m_modelHandle);
}

void HammerTrap::Init(int handle, int subHandle)
{
	m_frameModelPos = m_pos;
	m_spikeModelPos = m_pos;

	m_modelHandle = handle;

	m_frameModelPos.y -= 0.5f;

	MV1SetPosition(m_modelHandle, m_frameModelPos.ConvertToVECTOR());
	MV1SetScale(m_modelHandle, VECTOR(kModelSize, kModelSize, kModelSize));

	MyLib::Vec3 searchPos = m_pos + MyLib::Vec3(0.0f, 0.0f, -10.0f);

	m_pSearch->Init(m_pPhysics, searchPos);
}

void HammerTrap::Update()
{
	//���G�����蔻��ɓG������������U���J�n
	//�U���̓A�j���[�V�����Œn�ʂɒ������Ƃ��ɍU��������o��

	//�ݒu����ۂɉ�]�������悤�ɂ��āA�U���̌�����ύX�ł���悤�ɂ�����
}

void HammerTrap::Draw()
{
	MV1DrawModel(m_modelHandle);
}
