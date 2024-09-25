#include "HealPortion.h"
#include "EffekseerForDXLib.h"
#include "EffectManager.h"

namespace
{
	constexpr float kModelScale = 0.08f;
}

HealPortion::HealPortion() :
	ObjectBase(Collidable::Priority::Low, GameObjectTag::Portion),
	m_isExist(false),
	m_frame(0),
	m_angle(0.0f),
	m_posOffsetY(0.0f)
{
	auto collider = Collidable::AddCollider(MyLib::ColliderData::Kind::Sphere, true);
	auto sphereCol = dynamic_cast<MyLib::ColliderDataSphere*>(collider.get());
	sphereCol->m_radius = 2.4f;

	EffectManager::GetInstance().Load("Portion", "data/effect/portion.efk", 120);
}

HealPortion::~HealPortion()
{
	MV1DeleteModel(m_modelHandle);
}

void HealPortion::Init(std::shared_ptr<MyLib::Physics> physics, int model)
{
	Collidable::Init(physics);

	// ï®óùãììÆÇÃèâä˙âª
	//rigidbody.Init(true);
	rigidbody.Init();

	m_modelHandle = model;
	MV1SetScale(m_modelHandle, VECTOR(kModelScale, kModelScale, kModelScale));

	m_isExist = true;
}

void HealPortion::Finalize(std::shared_ptr<MyLib::Physics> physics)
{
	Collidable::Finalize(physics);
}

void HealPortion::Update()
{
	//ë∂ç›ÇµÇƒÇ¢Ç»Ç¢èÛë‘Ç»ÇÁâΩÇ‡Ç≥ÇπÇ»Ç¢
	if (!m_isExist)return;


	if (m_frame % 120 == 0)
	{
		auto pos = rigidbody.GetPos();

		EffectManager::GetInstance().CreateEffect("Portion",pos,pos);
	}

	m_frame++;

	m_angle += 0.03f;
	m_posOffsetY = sinf(m_angle) * 0.01f;

	auto pos = rigidbody.GetPos();
	pos.y += m_posOffsetY;

	rigidbody.SetPos(pos);
	MV1SetPosition(m_modelHandle, pos.ConvertToVECTOR());
	MV1SetRotationXYZ(m_modelHandle, VECTOR(0.0f, m_angle/2, 0.0f));
}

void HealPortion::Draw()
{
	//ë∂ç›ÇµÇƒÇ¢Ç»Ç¢èÛë‘Ç»ÇÁâΩÇ‡Ç≥ÇπÇ»Ç¢
	if (!m_isExist)return;

	MV1DrawModel(m_modelHandle);
}

void HealPortion::SetPosition(MyLib::Vec3 pos)
{
	pos.y += 2.0f;
	rigidbody.SetPos(pos);

	MV1SetPosition(m_modelHandle, rigidbody.GetPosVECTOR());
}

void HealPortion::End()
{
	m_isExist = false;
}