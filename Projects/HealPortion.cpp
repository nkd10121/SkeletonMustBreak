#include "HealPortion.h"
#include "EffekseerForDXLib.h"
#include "EffectManager.h"
#include "ModelManager.h"

namespace
{
	constexpr float kModelScale = 0.08f;
}

/// <summary>
/// コンストラクタ
/// </summary>
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

/// <summary>
/// デストラクタ
/// </summary>
HealPortion::~HealPortion()
{
	MV1DeleteModel(m_modelHandle);
}

/// <summary>
/// 初期化
/// </summary>
/// <param name="physics">物理クラスのポインタ</param>
/// <param name="model">モデルハンドル</param>
void HealPortion::Init(std::shared_ptr<MyLib::Physics> physics)
{
	Collidable::Init(physics);

	// 物理挙動の初期化
	//rigidbody.Init(true);
	rigidbody.Init();

	m_modelHandle = ModelManager::GetInstance().GetModelHandle("data/model/bottle_red.mv1");
	MV1SetScale(m_modelHandle, VECTOR(kModelScale, kModelScale, kModelScale));

	m_isExist = true;
}

/// <summary>
/// 当たり判定を削除
/// </summary>
/// <param name="physics">物理クラスのポインタ</param>
void HealPortion::Finalize(std::shared_ptr<MyLib::Physics> physics)
{
	Collidable::Finalize(physics);
}

/// <summary>
/// 更新
/// </summary>
void HealPortion::Update()
{
	//存在していない状態なら何もさせない
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

/// <summary>
/// 描画
/// </summary>
void HealPortion::Draw()
{
	//存在していない状態なら何もさせない
	if (!m_isExist)return;

	MV1DrawModel(m_modelHandle);
}

/// <summary>
/// 座標を設定
/// </summary>
/// <param name="pos">移動先の座標</param>
void HealPortion::SetPosition(MyLib::Vec3 pos)
{
	pos.y += 2.0f;
	rigidbody.SetPos(pos);

	MV1SetPosition(m_modelHandle, rigidbody.GetPosVECTOR());
}

/// <summary>
/// 終了させる
/// </summary>
void HealPortion::End()
{
	m_isExist = false;
}