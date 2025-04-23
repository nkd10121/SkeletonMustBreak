#include "HammerTrap.h"

namespace
{
	constexpr float kModelSizeScale = 10.0f;
}

HammerTrap::HammerTrap(std::shared_ptr<MyLib::Physics> physics) :
	TrapBase(GameObjectTag::SpikeTrap),
	m_frameModelPos(),
	m_spikeModelPos(),
	m_isAttack(false),
	m_isInitCollision(false)
{
	m_pPhysics = physics;

	//当たり判定の設定
	auto collider = Collidable::AddCollider(MyLib::ColliderData::Kind::Sphere, true);
	auto sphereCol = dynamic_cast<MyLib::ColliderDataSphere*>(collider.get());
	sphereCol->m_radius = 8.0f;

	m_pSearch = std::make_shared<SearchObject>(5.5f);

	//攻撃力の設定
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
	MV1SetScale(m_modelHandle, VECTOR(kModelSizeScale, kModelSizeScale, kModelSizeScale));

	MyLib::Vec3 searchPos = m_pos + MyLib::Vec3(0.0f, 0.0f, -10.0f);

	m_pSearch->Init(m_pPhysics, searchPos);
}

void HammerTrap::Update()
{
	//索敵当たり判定に敵が当たったら攻撃開始
	//攻撃はアニメーションで地面に着いたときに攻撃判定を出す

	//設置する際に回転させれるようにして、攻撃の向きを変更できるようにしたい
}

void HammerTrap::Draw()
{
	MV1DrawModel(m_modelHandle);
}
