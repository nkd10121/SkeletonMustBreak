#include "EnemyBase.h"
#include "Shot.h"
#include "SpikeTrap.h"
#include "CutterTrap.h"
#include "ModelManager.h"
#include "CsvLoad.h"
#include "WeaponBase.h"
#include "Sword.h"
#include "HitBox.h"
#include "SearchObject.h"
#include "SoundManager.h"
#include "EffectManager.h"
#include <memory>

namespace
{
	//アニメーションの切り替えにかかるフレーム数
	constexpr float kAnimChangeFrame = 10.0f;
	//アニメーションの切り替え速度
	constexpr float kAnimChangeRateSpeed = 1.0f / kAnimChangeFrame;

	//アニメーションブレンド率の最大
	constexpr float kAnimBlendRateMax = 1.0f;

	constexpr float kDistance = 2.6f;

	//ヒットエフェクトのオフセット
	constexpr int kEffectOffsetRand = 6;
}

EnemyBase::EnemyBase(Priority priority)
	:CharacterBase(priority, GameObjectTag::Enemy),
	m_pWeapon(nullptr),
	m_pNormalHitbox(nullptr),
	m_pHeadHitbox(nullptr),
	m_pSearch(nullptr),
	m_dropPoint(0),
	m_attackWaitFrame(0),
	m_knockCount(0),
	m_routeIdx(0),
	m_searchRange(0.0f),
	m_radius(0.0f),
	m_isExist(false),
	m_isReach(false),
	m_isHit(false),
	m_isDead(false),
	m_isDroped(false),
	m_isAttack(false),
	m_isKnock(false),
	m_isChase(true),
	m_isChasing(false),
	m_route(),
	m_destinationPos(),
	m_centerPos(),
	m_lastHitObjectTag()
{
}

EnemyBase::~EnemyBase()
{
	//メモリの解放
	MV1DeleteModel(m_modelHandle);
}

void EnemyBase::Finalize(std::shared_ptr<MyLib::Physics>physics)
{
	Collidable::Finalize(physics);
	m_pNormalHitbox->Finalize(m_pPhysics);
	m_pHeadHitbox->Finalize(m_pPhysics);
	if (m_isChase)
	{
		m_pWeapon->CollisionEnd();
		m_pSearch->Finalize(m_pPhysics);
	}
}

void EnemyBase::OnCollideEnter(const std::shared_ptr<Collidable>& colider)
{
#ifdef _DEBUG
	std::string message = "敵が";
#endif
	auto tag = colider->GetTag();
	switch (tag)
	{
	case GameObjectTag::Player:		//プレイヤーと当たった時
#ifdef _DEBUG
		message += "プレイヤー";
#endif
		break;
	case GameObjectTag::Shot:	//プレイヤーが撃った弾と当たった時
#ifdef _DEBUG
		message += "弾";
#endif
		break;
	}
#ifdef _DEBUG
	message += "と当たった！\n";
	printfDx(message.c_str());
#endif
}

void EnemyBase::OnCollideStay(const std::shared_ptr<Collidable>& colider)
{
#ifdef _DEBUG
	std::string message = "敵が";
#endif
	auto tag = colider->GetTag();
	switch (tag)
	{
	case GameObjectTag::Player:		//プレイヤーと当たった時
		break;
	case GameObjectTag::Shot:	//プレイヤーが撃った弾と当たった時
		break;
	}
}

bool EnemyBase::GetIsHit()
{
	bool log = m_isHit;
	m_isHit = false;
	return log;
}

int EnemyBase::GetDropPoint()
{
	m_isDroped = true;
	return m_dropPoint;
}

const float EnemyBase::GetRadius() const
{
	//もし半径0(索敵範囲なし)ならとりあえずの値を返す
	if (m_radius == 0.0f)
	{
		return 5.0f;
	}
	
	//0でなければ索敵範囲の半径をそのまま返す
	return m_radius;
}

/// <summary>
/// ほかのオブジェクトと押し出し判定をする当たり判定を作成
/// </summary>
/// <param name="radius">半径</param>
void EnemyBase::InitCollision(float radius)
{
	auto collider = Collidable::AddCollider(MyLib::ColliderData::Kind::Sphere, false);
	auto sphereCol = dynamic_cast<MyLib::ColliderDataSphere*>(collider.get());
	sphereCol->m_radius = radius;
}

/// <summary>
/// モデルを読み込む
/// </summary>
/// <param name="path">モデルパス</param>
void EnemyBase::LoadModel(std::string path)
{
	m_modelHandle = ModelManager::GetInstance().GetModelHandle(path);
}

/// <summary>
/// アニメーション情報や、ステータス情報を読み込む
/// </summary>
/// <param name="name">キャラクター名</param>
void EnemyBase::LoadData(std::string name)
{
	CsvLoad::GetInstance().AnimDataLoad(name, m_animIdx);
	CsvLoad::GetInstance().StatusLoad(m_status, name.c_str());
	m_dropPoint = m_status.point;
	m_maxHp = m_status.hp;
}

/// <summary>
/// 武器を生成する
/// </summary>
/// <param name="kind">武器の種類</param>
/// <param name="modelSize">武器モデルの拡大率</param>
void EnemyBase::CreateWeapon(WeaponKind kind, float modelSize)
{
	switch (kind)
	{
	case EnemyBase::Blade:
		m_pWeapon = std::make_shared<Sword>();
		m_pWeapon->Init(m_modelHandle, "handslot.r", modelSize);
		m_pWeapon->SetAtk(m_status.atk);
		break;
	default:
		break;
	}
}

/// <summary>
/// ルートをモデルの高さに合うように調整する
/// </summary>
/// <param name="modelOffesetY">Y軸の差分</param>
/// <param name="modelSize">モデルの拡大率</param>
void EnemyBase::AdjustmentRoute(float modelOffesetY, float modelSize)
{
	for (int i = 0; i < m_route.size(); i++)
	{
		m_route[i].y += modelOffesetY * modelSize;
	}
}

/// <summary>
/// 物理クラスの初期化
/// </summary>
/// <param name="isUseGravity">true:重力を与える,false:重力を与えない</param>
void EnemyBase::InitRigidbody(bool isUseGravity)
{
	rigidbody.Init(isUseGravity);
	rigidbody.SetPos(m_route[0]);
	rigidbody.SetNextPos(rigidbody.GetPos());
	m_moveVec = (m_destinationPos - rigidbody.GetPos()).Normalize();
}

/// <summary>
/// モデルの中心座標を計算
/// </summary>
/// <param name="modeldefaultSize">モデルのもともとのサイズ</param>
/// <param name="modelSize">モデルの拡大率</param>
void EnemyBase::CalculationCenterPos(float modeldefaultSize, float modelSize)
{
	m_centerPos = rigidbody.GetPos();
	m_centerPos.y += (modeldefaultSize * modelSize) / 2;
}

/// <summary>
/// モデル座標を設定
/// </summary>
/// <param name="offset">差分</param>
void EnemyBase::SetModelPos(float offset)
{
	m_modelPos = m_collisionPos;
	m_modelPos.y -= offset;
}

/// <summary>
/// ダメージ判定をする当たり判定を作成
/// </summary>
/// <param name="radius">半径</param>
void EnemyBase::InitNormalHitBox(float radius)
{
	m_pNormalHitbox = std::make_shared<HitBox>(radius);
	m_pNormalHitbox->Init(m_pPhysics, m_centerPos, false);
}

void EnemyBase::InitHeadHitBox(float radius)
{
	m_pHeadHitbox = std::make_shared<HitBox>(radius);
	m_pHeadHitbox->Init(m_pPhysics, m_centerPos, true);
}

/// <summary>
/// 索敵判定をする当たり判定を作成
/// </summary>
/// <param name="radius">半径</param>
void EnemyBase::InitSearch(float radius)
{
	m_pSearch = std::make_shared<SearchObject>(radius);
	m_pSearch->Init(m_pPhysics, m_modelPos, true);

	m_radius = radius;
}

/// <summary>
/// ダメージを受けたとき
/// </summary>
void EnemyBase::OnNormalDamage()
{
	//HPを減らす
	m_status.hp -= m_pNormalHitbox->GetIsAttackNum() - m_status.def;

	m_lastHitObjectTag = m_pNormalHitbox->GetHitObjectTag();

	//EnemyManagerがエフェクトを生成するためのフラグ
	m_isHit = true;

	SoundManager::GetInstance().PlaySE("EnemyHit");

	//敵の近くにヒットエフェクトを表示
	auto pos = m_centerPos;
	MyLib::Vec3 offset = MyLib::Vec3(static_cast<float>(GetRand(kEffectOffsetRand) - kEffectOffsetRand / 2), static_cast<float>(GetRand(kEffectOffsetRand) - kEffectOffsetRand / 2), static_cast<float>(GetRand(kEffectOffsetRand) - kEffectOffsetRand / 2));
	pos += offset;
	EffectManager::GetInstance().CreateEffect("EnemyHit", pos, pos);

	if (!m_isKnock && !m_isAttack)
	{
		m_isKnock = true;
		m_updateFunc = &EnemyBase::HitUpdate;
		m_nowAnimIdx = m_animIdx["Hit"];

		ChangeAnim(m_nowAnimIdx);
	}
}

void EnemyBase::OnHeadDamage()
{
	//HPを減らす
	m_status.hp -= m_pHeadHitbox->GetIsAttackNum() * 4 - m_status.def;

	m_lastHitObjectTag = m_pHeadHitbox->GetHitObjectTag();

	//EnemyManagerがエフェクトを生成するためのフラグ
	m_isHit = true;

	SoundManager::GetInstance().PlaySE("EnemyCriticalHit");
	//敵の近くにヒットエフェクトを表示
	auto pos = m_centerPos;
	MyLib::Vec3 offset = MyLib::Vec3(static_cast<float>(GetRand(kEffectOffsetRand) - kEffectOffsetRand / 2), static_cast<float>(GetRand(kEffectOffsetRand) - kEffectOffsetRand / 2), static_cast<float>(GetRand(kEffectOffsetRand) - kEffectOffsetRand / 2));
	pos += offset;
	EffectManager::GetInstance().CreateEffect("EnemyCriticalHit", pos, pos);

	if (!m_isKnock && !m_isAttack)
	{
		m_isKnock = true;
		m_updateFunc = &EnemyBase::HitUpdate;
		m_nowAnimIdx = m_animIdx["Hit"];

		ChangeAnim(m_nowAnimIdx);
	}
}

/// <summary>
/// 死亡した時
/// </summary>
void EnemyBase::Death()
{
	if (!m_isDead)
	{
		m_isDead = true;
		EnemyBase::Finalize(m_pPhysics);

		m_updateFunc = &EnemyBase::DeathUpdate;

		auto deathAnimIdx = GetRand(2);
		auto deathAnimSpeed = 0.66f;

		switch (deathAnimIdx)
		{
		case 0:
			m_nowAnimIdx = m_animIdx["Death0"];
			deathAnimSpeed = 0.33f;
			break;
		case 1:
			m_nowAnimIdx = m_animIdx["Death1"];
			break;
		case 2:
			m_nowAnimIdx = m_animIdx["Death2"];
			break;
		}
		ChangeAnim(m_nowAnimIdx, deathAnimSpeed);
	}
}

/// <summary>
/// アニメーションブレンドの更新
/// </summary>
void EnemyBase::UpdateAnimationBlend()
{
	//アニメーションの切り替え
	if (m_prevAnimNo != -1)
	{
		//フレームでアニメーションを切り替える
		m_animBlendRate += kAnimChangeRateSpeed;
		if (m_animBlendRate >= kAnimBlendRateMax)
		{
			m_animBlendRate = kAnimBlendRateMax;
		}

		//アニメーションのブレンド率を設定する
		MV1SetAttachAnimBlendRate(m_modelHandle, m_prevAnimNo, kAnimBlendRateMax - m_animBlendRate);
		MV1SetAttachAnimBlendRate(m_modelHandle, m_currentAnimNo, m_animBlendRate);
	}
}

/// <summary>
/// モデルの座標設定
/// </summary>
/// <param name="offset"></param>
void EnemyBase::SetDrawModelPos(float offset)
{
	rigidbody.SetPos(rigidbody.GetNextPos());
	m_collisionPos = rigidbody.GetPos();
	SetModelPos(offset);
	MV1SetPosition(m_modelHandle, m_modelPos.ConvertToVECTOR());
}

void EnemyBase::AttackUpdate(MyLib::Vec3 playerPos, bool isChase)
{
	rigidbody.SetVelocity(MyLib::Vec3());
	//アニメーションの更新

	if (m_attackWaitFrame >= 20)
	{
		if (m_attackWaitFrame == 20)
		{
			m_nowAnimIdx = m_animIdx["Attack"];
			ChangeAnim(m_nowAnimIdx, 0.3f);
		}

		if (m_attackWaitFrame == 40)
		{
			m_pWeapon->InitCollision(m_pPhysics);
		}

		if (m_attackWaitFrame == 60)
		{
			m_pWeapon->CollisionEnd();
		}


		//アニメーションが終了したら歩き状態に戻す
		if (m_isAnimationFinish)
		{
			m_attackWaitFrame = 0;

			m_isAttack = false;

			m_pSearch->IsTriggerReset();

			m_updateFunc = &EnemyBase::WalkUpdate;
			ChangeAnim(m_animIdx["Move"]);
		}

	}

	m_attackWaitFrame++;
}

void EnemyBase::WalkUpdate(MyLib::Vec3 playerPos, bool isChase)
{
	//攻撃フラグがtrueになっていたら攻撃をする
	if (m_isAttack && m_isChase)
	{
		m_moveVec = (playerPos - m_collisionPos).Normalize();

		m_updateFunc = &EnemyBase::AttackUpdate;

		m_nowAnimIdx = m_animIdx["Idle"];
		ChangeAnim(m_nowAnimIdx);
		return;
	}

	if (m_isChase)
	{
		if (m_pSearch->GetIsTrigger())
		{
			m_isAttack = true;
		}

		if (m_pSearch->GetIsStay())
		{
			m_isAttack = true;
		}
	}


	if (m_isKnock)
	{
		m_knockCount++;

		if (m_knockCount > 90)
		{
			m_isKnock = false;
			m_knockCount = 0;
		}
	}

	auto temp = (rigidbody.GetPos() - m_route[m_routeIdx]).Size();
	if (temp < kDistance)
	{
		m_routeIdx++;
		if (m_routeIdx == m_route.size())
		{
			EnemyBase::Finalize(m_pPhysics);
			m_isExist = false;
			m_isReach = true;
		}
		else
		{
			m_destinationPos = m_route[m_routeIdx];
		}
	}

	//バグを治すためにいったんここで様子見
	m_moveVec = (m_destinationPos - rigidbody.GetPos()).Normalize();

	//プレイヤーを追跡する機能持ちなら
	if (m_isChase && isChase)
	{
		//直前まで追っていたかどうか保存
		bool isChesed = m_isChasing;

		//プレイヤーの座標と現在位置から距離を計算する
		auto pPos = playerPos;
		pPos.y = m_collisionPos.y;
		auto len = pPos - m_collisionPos;
		auto dis = len.Size();

		//一定範囲より近ければ追跡する
		if (dis < m_searchRange)
		{
			m_isChasing = true;
			m_moveVec = len.Normalize();
		}
		//
		else
		{
			m_isChasing = false;
			//追跡を終えたとき
			if (isChesed && !m_isChasing)
			{
				//ルートを見て、一番近い座標に向かう
				float distanceMin = 1000.0f;
				int retRouteNum = 0;

				for (int i = 0; i < m_route.size(); i++)
				{
					//現在座標からの距離を見る
					auto dis = (m_route[i] - rigidbody.GetPos()).Size();

					if (distanceMin > dis)
					{
						distanceMin = dis;
						retRouteNum = i;
					}
				}

				if (retRouteNum != m_routeIdx - 1)
				{
					m_routeIdx = retRouteNum;

				}

				//
				m_destinationPos = m_route[m_routeIdx];
				m_moveVec = (m_destinationPos - rigidbody.GetPos()).Normalize();
			}
		}
	}



	//攻撃予定なら移動しない(モデルの向きは変更させたい)
	if (!m_isAttack)
	{
		//移動速度をここで設定できるっぽい？
		rigidbody.SetVelocity(m_moveVec, m_status.speed);
	}



	//atan2を使用して向いている角度を取得
	auto angle = atan2(m_moveVec.x, m_moveVec.z);
	auto rotation = VGet(0.0f, angle + DX_PI_F, 0.0f);
	//移動方向に体を回転させる
	MV1SetRotationXYZ(m_modelHandle, rotation);
}

void EnemyBase::HitUpdate(MyLib::Vec3 playerPos, bool isChase)
{
	rigidbody.SetVelocity(MyLib::Vec3());
	//アニメーションの更新
	m_isAnimationFinish = UpdateAnim(m_currentAnimNo);

	//アニメーションが終了したら歩き状態に戻す
	if (m_isAnimationFinish)
	{

		m_updateFunc = &EnemyBase::WalkUpdate;
		ChangeAnim(m_animIdx["Move"]);
	}
}

void EnemyBase::DeathUpdate(MyLib::Vec3 playerPos, bool isChase)
{
	if (m_isAnimationFinish)
	{
		m_isExist = false;
	}
}
