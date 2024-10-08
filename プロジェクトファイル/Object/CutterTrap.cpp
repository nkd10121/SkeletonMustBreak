#include "CutterTrap.h"
#include "ModelManager.h"
#include "CsvLoad.h"

namespace
{
	const char* kTrapName = "Cutter";
	constexpr float kModelSizeScale = 0.08f;
	//モデルのパス
	const char* kModelPath = "data/model/cutter.mv1";
	//回転速度
	constexpr float kRotationSpeed = 0.08f;
}

CutterTrap::CutterTrap(std::shared_ptr<MyLib::Physics> physics) :
	TrapBase(GameObjectTag::CutterTrap),
	m_rotationAngle(0.0f),
	m_frame(0)
{
	m_pPhysics = physics;

	//ステータスの取得
	status = CsvLoad::GetInstance().TrapStatusLoad(kTrapName);

	//当たり判定の設定
	auto collider = Collidable::AddCollider(MyLib::ColliderData::Kind::Sphere, true);
	auto sphereCol = dynamic_cast<MyLib::ColliderDataSphere*>(collider.get());
	sphereCol->m_radius = status.atkRange;

	//攻撃力の設定
	m_attack = status.atk;
}

CutterTrap::~CutterTrap()
{
	MV1DeleteModel(m_modelHandle);
}

void CutterTrap::Init()
{
	m_modelHandle = ModelManager::GetInstance().GetModelHandle(kModelPath);
	MV1SetScale(m_modelHandle, VECTOR(kModelSizeScale, kModelSizeScale, kModelSizeScale));
	MV1SetPosition(m_modelHandle, m_pos.ConvertToVECTOR());
}

void CutterTrap::Update()
{
	//モデルを回転させる
	m_rotationAngle += kRotationSpeed;
	MV1SetRotationXYZ(m_modelHandle, VECTOR(0.0f, m_rotationAngle, 0.0f));

	//数秒に一回攻撃判定を消してつけなおす
	if (m_frame % status.coolTime == 0)
	{
		Collidable::Init(m_pPhysics);
		rigidbody.Init();
		rigidbody.SetPos(m_pos);
	}

	if (m_frame % status.coolTime == status.coolTime - 1)
	{
		Finalize(m_pPhysics);
	}

	m_frame++;
}

void CutterTrap::Draw()
{
	MV1DrawModel(m_modelHandle);
}
