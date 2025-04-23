#include "Shot.h"
#include "DxLib.h"
#include "MapDataLoad.h"
#include <string>

#include "ModelManager.h"
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
	MV1DeleteModel(m_arrowModel);
}

void Shot::Init(std::shared_ptr<MyLib::Physics> physics)
{
	Collidable::Init(physics);

	// 物理挙動の初期化
	//rigidbody.Init(true);
	rigidbody.Init();

	m_arrowModel = ModelManager::GetInstance().GetModelHandle("data/model/arrow.mv1");
	MV1SetScale(m_arrowModel, VECTOR(8.0f, 8.0f, 8.0f));

	m_isExist = true;

	m_mapHandle = MapDataLoad::GetInstance().GetStageCollisionHandle();
}

void Shot::Finalize(std::shared_ptr<MyLib::Physics> physics)
{
	Collidable::Finalize(physics);
}

void Shot::Set(const MyLib::Vec3& pos, const MyLib::Vec3& m_dir, const int& atk)
{
	rigidbody.SetPos(pos+m_dir);
	m_moveDir = m_dir;
	m_atk = atk;

	//回転させる方向を計算する
	m_angle = -atan2f(m_moveDir.z, m_moveDir.x) - DX_PI_F / 2;
	m_rot = MyLib::Vec3(0.0f, m_angle, 0.0f);

	MV1SetRotationXYZ(m_arrowModel, m_rot.ConvertToVECTOR());
}

void Shot::Update()
{
	//存在していない状態なら何もさせない
	if (!m_isExist)return;

	m_frameCount++;

	auto dirNorm = m_moveDir.Normalize();
	rigidbody.SetVelocity(dirNorm * 1.0f);

	auto hitDim = MV1CollCheck_Sphere(m_mapHandle, -1, rigidbody.GetPosVECTOR(), kRadius);

	// 検出した周囲のポリゴン情報を開放する
	MV1CollResultPolyDimTerminate(hitDim);

	if (hitDim.HitNum != 0)
	{
		// 検出した周囲のポリゴン情報を開放する
		m_isExist = false;
	}

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
	//DrawSphere3D(pos.ConvertToVECTOR(), kRadius, 4, 0xff0000, 0xff0000, true);
	MV1SetPosition(m_arrowModel, pos.ConvertToVECTOR());
	MV1DrawModel(m_arrowModel);
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
		//m_isExist = false;
		break;
	case GameObjectTag::HitBox:
#ifdef _DEBUG
		message += "当たり判定";
#endif
		m_isExist = false;
		break;
	}
#ifdef _DEBUG
	message += "と当たった！\n";
	printfDx(message.c_str());
#endif
}
