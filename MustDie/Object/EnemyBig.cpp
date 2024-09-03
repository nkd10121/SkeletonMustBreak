#include "EnemyBig.h"
#include "CsvLoad.h"
#include "WeaponBase.h"

namespace
{

	//モデルサイズ
	constexpr float kModelSize = 6.8f;
	constexpr float kModelOffsetY = 0.8f;

	constexpr float kCollisionRadius = 6.0f;

	constexpr float kWeaponModelSize = 0.01f;

	//アニメーションの切り替えにかかるフレーム数
	constexpr float kAnimChangeFrame = 10.0f;
	constexpr float kAnimChangeRateSpeed = 1.0f / kAnimChangeFrame;

	//アニメーションブレンド率の最大
	constexpr float kAnimBlendRateMax = 1.0f;

	constexpr float kSearchingRange = 30.0f;
}

EnemyBig::EnemyBig(int modelH, int weponH):
	EnemyBase(Collidable::Priority::Middle),
	m_nowAnimIdx(eAnimIdx::Run),
	m_weponHandle(-1),
	m_tempFrameCount(0),
	m_weponAttachFrameNum(-1),
	m_attackWaitFrame(0),
	m_knockCount(0),
	m_currentAnimNo(-1),
	m_prevAnimNo(-1),
	m_animBlendRate(1.0f),
	m_animSpeed(0.5f),
	m_isAnimationFinish(false),
	m_isChasing(false)
{
	//当たり判定の設定
	auto collider = Collidable::AddCollider(MyLib::ColliderData::Kind::Sphere, false);
	auto sphereCol = dynamic_cast<MyLib::ColliderDataSphere*>(collider.get());
	sphereCol->m_radius = kCollisionRadius;

	CsvLoad::GetInstance().StatusLoad(m_status, "BigSkelton");

	m_weapon = std::make_shared<WeaponBase>();
	m_weapon->Init(weponH, modelH, "handslot.r", kWeaponModelSize);

	//モデルの読み込み
	m_modelHandle = modelH;
	//m_weponHandle = weponH;

	m_dropPoint = m_status.point;
}

EnemyBig::~EnemyBig()
{
	//メモリの解放
	MV1DeleteModel(m_modelHandle);
}

void EnemyBig::Init(std::shared_ptr<MyLib::Physics> physics, std::vector<MyLib::Vec3> route)
{
	m_pPhisics = physics;

	m_route = route;

	for (int i = 0; i < m_route.size(); i++)
	{
		m_route[i].y += kModelOffsetY * kModelSize;
	}


	//最初の目的地を設定する
	m_routeNum = 1;
	m_destinationPos = m_route[m_routeNum];

	//存在している状態にする
	m_isExist = true;

	Collidable::Init(m_pPhisics);

	rigidbody.Init(true);
	rigidbody.SetPos(m_route[0]);
	rigidbody.SetNextPos(rigidbody.GetPos());
	m_moveVec = (m_destinationPos - rigidbody.GetPos()).Normalize();

	m_collisionPos = rigidbody.GetPos();
	SetModelPos();
	MV1SetPosition(m_modelHandle, m_modelPos.ConvertToVECTOR());


	//モデルのサイズ設定
	MV1SetScale(m_modelHandle, VGet(kModelSize, kModelSize, kModelSize));
	//m_weponAttachFrameNum = MV1SearchFrame(m_modelHandle, "handslot.r");


	//アニメーションを設定
	m_currentAnimNo = MV1AttachAnim(m_modelHandle, eAnimIdx::Run);


	//通常状態に設定しておく
	m_updateFunc = &EnemyBig::WalkUpdate;
}

void EnemyBig::Finalize(std::shared_ptr<MyLib::Physics> physics)
{
	Collidable::Finalize(physics);
}

void EnemyBig::Update(MyLib::Vec3 playerPos, bool isChase)
{
	//存在していない状態なら何もさせない
	if (!m_isExist)return;

	//アニメーションの更新
	m_isAnimationFinish = UpdateAnim(m_currentAnimNo);

	//状態の更新
	(this->*m_updateFunc)(playerPos, isChase);

	//HPが以下になったら死亡する
	if (m_status.hp <= 0)
	{
		if (!m_isDead)
		{
			m_isDead = true;
			Finalize(m_pPhisics);

			m_updateFunc = &EnemyBig::DeathUpdate;

			auto deathAnimIdx = GetRand(2);
			auto deathAnimSpeed = 0.66f;

			switch (deathAnimIdx)
			{
			case 0:
				m_nowAnimIdx = eAnimIdx::DeathA;
				deathAnimSpeed = 0.33f;
				break;
			case 1:
				m_nowAnimIdx = eAnimIdx::DeathB;
				break;
			case 2:
				m_nowAnimIdx = eAnimIdx::DeathC;
				break;
			}



			ChangeAnim(m_nowAnimIdx, deathAnimSpeed);
		}
	}


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


	m_weapon->Update((m_collisionPos + m_moveVec * 4));
}

void EnemyBig::Draw()
{
	//存在していない状態なら何もさせない
	if (!m_isExist)return;

	rigidbody.SetPos(rigidbody.GetNextPos());
	m_collisionPos = rigidbody.GetPos();
	SetModelPos();
	MV1SetPosition(m_modelHandle, m_modelPos.ConvertToVECTOR());
	//モデルの描画
	MV1DrawModel(m_modelHandle);
	m_weapon->Draw();

#ifdef _DEBUG
	DrawFormatString(800, 60, 0xffffff, "%f,%f,%f", m_modelPos.x, m_modelPos.y, m_modelPos.z);
	DrawFormatString(800, 76, 0xffffff, "%f,%f,%f", m_collisionPos.x, m_collisionPos.y, m_collisionPos.z);
#endif
}

void EnemyBig::SetModelPos()
{
	m_modelPos = m_collisionPos;
	m_modelPos.y -= kModelOffsetY * kModelSize;
}

bool EnemyBig::UpdateAnim(int attachNo, float startTime)
{
	//アニメーションが設定されていなかったら早期リターン
	if (attachNo == -1)	return false;

	//アニメーションを進行させる
	float nowFrame = MV1GetAttachAnimTime(m_modelHandle, attachNo);	//現在の再生カウントを取得
	nowFrame += m_animSpeed;

	//現在再生中のアニメーションの総カウントを取得する
	float totalAnimframe = MV1GetAttachAnimTotalTime(m_modelHandle, attachNo);
	bool isLoop = false;

	//NOTE:もしかしたら総フレーム分引いても総フレームより大きいかもしれないからwhileで大きい間引き続ける
	while (totalAnimframe <= nowFrame)
	{
		//NOTE:nowFrameを0にリセットするとアニメーションフレームの飛びがでるから総フレーム分引く
		nowFrame -= totalAnimframe;
		nowFrame += startTime;
		isLoop = true;
	}

	//進めた時間に設定
	MV1SetAttachAnimTime(m_modelHandle, attachNo, nowFrame);

	return isLoop;
}

void EnemyBig::ChangeAnim(int animIndex, float animSpeed)
{
	//さらに古いアニメーションがアタッチされている場合はこの時点で消しておく
	if (m_prevAnimNo != -1)
	{
		MV1DetachAnim(m_modelHandle, m_prevAnimNo);
	}

	//現在再生中の待機アニメーションは変更目のアニメーションの扱いにする
	m_prevAnimNo = m_currentAnimNo;

	//変更後のアニメーションとして攻撃アニメーションを改めて設定する
	m_currentAnimNo = MV1AttachAnim(m_modelHandle, animIndex);

	//切り替えの瞬間は変更前のアニメーションが再生される状態にする
	m_animBlendRate = 0.0f;

	m_animSpeed = animSpeed;

	//変更前のアニメーション100%
	MV1SetAttachAnimBlendRate(m_modelHandle, m_prevAnimNo, 1.0f - m_animBlendRate);
	//変更後のアニメーション0%
	MV1SetAttachAnimBlendRate(m_modelHandle, m_currentAnimNo, m_animBlendRate);
}

void EnemyBig::AttackUpdate(MyLib::Vec3 playerPos, bool isChase)
{
	rigidbody.SetVelocity(MyLib::Vec3());
	////アニメーションの更新
	//m_isAnimationFinish = UpdateAnim(m_currentAnimNo);

	if (m_attackWaitFrame >= 20)
	{
		if (m_attackWaitFrame == 20)
		{
			m_weapon->CollisionInit(m_pPhisics);

			m_nowAnimIdx = eAnimIdx::Attack;
			ChangeAnim(m_nowAnimIdx, 0.3f);
		}


		//アニメーションが終了したら歩き状態に戻す
		if (m_isAnimationFinish)
		{
			m_attackWaitFrame = 0;

			m_isAttack = false;

			m_weapon->CollisionEnd(m_pPhisics);

			m_updateFunc = &EnemyBig::WalkUpdate;
			ChangeAnim(eAnimIdx::Run);
		}

	}

	m_attackWaitFrame++;
}

void EnemyBig::WalkUpdate(MyLib::Vec3 playerPos, bool isChase)
{
	//攻撃フラグがtrueになっていたら攻撃をする
	if (m_isAttack)
	{
		m_moveVec = (playerPos - m_collisionPos).Normalize();

		m_updateFunc = &EnemyBig::AttackUpdate;

		m_nowAnimIdx = eAnimIdx::Idle;
		ChangeAnim(m_nowAnimIdx);
		return;
	}

	if (m_isKnock)
	{
		if (m_knockCount == 0)
		{
			m_updateFunc = &EnemyBig::HitUpdate;
			m_nowAnimIdx = eAnimIdx::Hit;
			ChangeAnim(m_nowAnimIdx);
		}

		m_knockCount++;

		if (m_knockCount > 90)
		{
			m_isKnock = false;
			m_knockCount = 0;
		}

	}

	auto temp = (rigidbody.GetPos() - m_route[m_routeNum]).Size();
	if (temp < 1.2f)
	{
		m_routeNum++;
		if (m_routeNum == m_route.size())
		{
			Finalize(m_pPhisics);
			m_isExist = false;
			m_isReach = true;
		}
		else
		{
			m_destinationPos = m_route[m_routeNum];
			//m_moveVec = (m_destinationPos - rigidbody.GetPos()).Normalize();
		}
	}

	//バグを治すためにいったんここで様子見
	m_moveVec = (m_destinationPos - rigidbody.GetPos()).Normalize();

	if (!isChase)
	{
		bool isChesed = m_isChasing;
		m_isChasing = false;

		playerPos.y = m_collisionPos.y;
		auto len = playerPos - m_collisionPos;
		auto dis = len.Size();

		//一定範囲より近ければ追跡する
		if (dis < kSearchingRange)
		{
			m_isChasing = true;
			m_moveVec = len.Normalize();
		}
		//
		else
		{
			//追跡を終えたとき
			if (isChesed != m_isChasing)
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

				if (retRouteNum != m_routeNum - 1)
				{
					m_routeNum = retRouteNum;

				}

				//
				m_destinationPos = m_route[m_routeNum];
				m_moveVec = (m_destinationPos - rigidbody.GetPos()).Normalize();
			}
		}
	}


	//移動速度をここで設定できるっぽい？
	rigidbody.SetVelocity(m_moveVec, m_status.speed);

	//atan2を使用して向いている角度を取得
	auto angle = atan2(m_moveVec.x, m_moveVec.z);
	auto rotation = VGet(0.0f, angle + DX_PI_F, 0.0f);
	//移動方向に体を回転させる
	MV1SetRotationXYZ(m_modelHandle, rotation);
}

void EnemyBig::HitUpdate(MyLib::Vec3 playerPos, bool isChase)
{
	rigidbody.SetVelocity(MyLib::Vec3());
	//アニメーションの更新
	m_isAnimationFinish = UpdateAnim(m_currentAnimNo);

	//アニメーションが終了したら歩き状態に戻す
	if (m_isAnimationFinish)
	{

		m_updateFunc = &EnemyBig::WalkUpdate;
		ChangeAnim(eAnimIdx::Run);
	}
}

void EnemyBig::DeathUpdate(MyLib::Vec3 playerPos, bool isChase)
{
	if (m_isAnimationFinish)
	{
		m_isExist = false;
	}
}
