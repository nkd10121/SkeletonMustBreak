#include "HitBox.h"
#include "Shot.h"
#include "SpikeTrap.h"
#include "CutterTrap.h"

/// <summary>
/// コンストラクタ
/// </summary>
/// <param name="r">半径</param>
HitBox::HitBox(float r) :
	ObjectBase(Priority::Low, GameObjectTag::HitBox),
	m_pPhysics(),
	m_lastHitObjectAttack(0),
	m_hitObjectTag(),
	m_isEnemy(false),
	m_isHit(false)
{
	//当たり判定の設定
	auto collider = Collidable::AddCollider(MyLib::ColliderData::Kind::Sphere, true);
	auto sphereCol = dynamic_cast<MyLib::ColliderDataSphere*>(collider.get());
	sphereCol->m_radius = r;
}

/// <summary>
/// デストラクタ
/// </summary>
HitBox::~HitBox()
{
}

/// <summary>
/// 初期化
/// </summary>
/// <param name="physics">物理クラスのポインタ</param>
/// <param name="pos">設定座標</param>
/// <param name="isEnemy">持ち主がEnemyかどうか</param>
void HitBox::Init(std::shared_ptr<MyLib::Physics> physics, MyLib::Vec3 pos, bool isEnemy)
{
	m_pPhysics = physics;
	m_isEnemy = isEnemy;

	Collidable::Init(m_pPhysics);

	rigidbody.Init();
	rigidbody.SetPos(pos);
}

/// <summary>
/// 更新
/// </summary>
/// <param name="pos">設定座標</param>
void HitBox::Update(MyLib::Vec3 pos)
{
	rigidbody.SetPos(pos);
}

/// <summary>
/// 当たり判定を削除
/// </summary>
/// <param name="physics"></param>
void HitBox::Finalize(std::shared_ptr<MyLib::Physics> physics)
{
	Collidable::Finalize(physics);
}

/// <summary>
/// ほかのオブジェクトに当たった時
/// </summary>
/// <param name="colider"></param>
void HitBox::OnTriggerEnter(const std::shared_ptr<Collidable>& colider)
{
	if (m_isEnemy)
	{
		//矢、トラップ
		m_hitObjectTag = colider->GetTag();
		if (m_hitObjectTag == GameObjectTag::Shot)
		{
			{	//なんか{}がないとエラー吐く
				Shot* col = dynamic_cast<Shot*>(colider.get());
				m_lastHitObjectAttack = col->GetAtk();
			}
			m_isHit = true;
		}

		if (m_hitObjectTag == GameObjectTag::SpikeTrap)
		{
			{	//なんか{}がないとエラー吐く
				SpikeTrap* col = dynamic_cast<SpikeTrap*>(colider.get());
				m_lastHitObjectAttack = col->GetAtk();
			}
			m_isHit = true;
		}

		if (m_hitObjectTag == GameObjectTag::CutterTrap)
		{
			{	//なんか{}がないとエラー吐く
				CutterTrap* col = dynamic_cast<CutterTrap*>(colider.get());
				m_lastHitObjectAttack = col->GetAtk();
			}
			m_isHit = true;
		}
	}

}

/// <summary>
/// 当たったオブジェクトの攻撃力を取得
/// </summary>
/// <returns></returns>
int HitBox::GetIsAttackNum()
{
	return m_lastHitObjectAttack;
}

/// <summary>
/// ほかのオブジェクトに当たったかどうか
/// </summary>
/// <returns></returns>
bool HitBox::GetIsHit()
{
	//trueを返したときだけfalseにする
	if (m_isHit)
	{
		m_isHit = false;
		return true;
	}

	return false;
}
