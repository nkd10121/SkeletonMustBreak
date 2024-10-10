#include "Enemy.h"
#include "WeaponBase.h"
#include "DxLib.h"

#include "HitBox.h"
#include "SearchObject.h"

#include "SoundManager.h"

namespace
{
	//キャラクター名
	constexpr const char* kCharacterName = "NormalSkelton";
	//モデルパス
	constexpr const char* kModelPath = "data/model/Skeleton_Minion.mv1";
	//モデルの元のサイズ
	constexpr float kModelDefaultSize = 2.166f;
	//モデルサイズの拡大率
	constexpr float kModelSizeScale = 4.4f;
	constexpr float kModelOffsetY = 0.3f;
	//武器のモデルサイズ
	constexpr float kWeaponModelSize = 0.01f;
	//押し出し当たり判定の半径
	constexpr float kCollisionRadius = 1.6f;
	//索敵範囲
	constexpr float kSearchingRadius = 30.0f;
	//当たり判定の半径
	constexpr float kHitBoxRadius = 3.0f;
}

/// <summary>
/// コンストラクタ
/// </summary>
Enemy::Enemy() :
	EnemyBase(Collidable::Priority::Middle)
{
	//当たり判定の設定
	InitCollision(kCollisionRadius);
	//モデルの読み込み
	LoadModel(kModelPath);
	//アニメーションやステータスを取得
	LoadData(kCharacterName);
	//武器を生成
	CreateWeapon(WeaponKind::Blade, kWeaponModelSize);
	//索敵範囲の設定
	m_searchRange = kSearchingRadius;
}

/// <summary>
/// デストラクタ
/// </summary>
Enemy::~Enemy()
{
}

/// <summary>
/// 初期化
/// </summary>
/// <param name="physics">物理クラスのポインタ</param>
/// <param name="route">移動ルート</param>
void Enemy::Init(std::shared_ptr<MyLib::Physics> physics, std::vector<MyLib::Vec3> route)
{
	//代入
	m_pPhysics = physics;
	m_route = route;

	//ルートのy座標を調整
	AdjustmentRoute(kModelOffsetY, kModelSizeScale);

	//最初の目的地を設定する
	m_routeIdx = 1;
	m_destinationPos = m_route[m_routeIdx];

	//存在している状態にする
	m_isExist = true;

	Collidable::Init(m_pPhysics);

	//物理クラスの初期化
	InitRigidbody();

	//中心座標の設定
	CalculationCenterPos(kModelDefaultSize, kModelSizeScale);

	//当たり判定の座標を設定
	m_collisionPos = rigidbody.GetPos();

	//モデルの座標を設定
	SetModelPos(kModelOffsetY * kModelSizeScale);
	MV1SetPosition(m_modelHandle, m_modelPos.ConvertToVECTOR());

	//ダメージ判定をする当たり判定を作成
	InitNormalHitBox(kHitBoxRadius);
	InitHeadHitBox(kHitBoxRadius*0.8f);

	//索敵判定をする当たり判定を作成
	InitSearch(kModelDefaultSize * kModelSizeScale / 2);

	//モデルのサイズ設定
	MV1SetScale(m_modelHandle, VGet(kModelSizeScale, kModelSizeScale, kModelSizeScale));

	//アニメーションを設定
	m_currentAnimNo = MV1AttachAnim(m_modelHandle, m_animIdx["Move"]);
	m_nowAnimIdx = m_animIdx["Move"];

	//通常状態に設定しておく
	m_updateFunc = &Enemy::WalkUpdate;
}

/// <summary>
/// 更新
/// </summary>
/// <param name="playerPos">プレイヤー座標</param>
/// <param name="isChase">プレイヤーが追跡できる状態かどうか</param>
void Enemy::Update(MyLib::Vec3 playerPos, bool isChase)
{
	//存在していない状態なら何もさせない
	if (!m_isExist)return;

	//アニメーションの更新
	m_isAnimationFinish = UpdateAnim(m_currentAnimNo);

	//状態の更新
	(this->*m_updateFunc)(playerPos, isChase);

	//中心座標の設定(ミニマップに表示するために必要)
	CalculationCenterPos(kModelDefaultSize, kModelSizeScale);

	//判定の更新
	MyLib::Vec3 centerPos = rigidbody.GetPos();
	centerPos.y += kModelDefaultSize / 2 * kModelSizeScale * 0.3f;
	m_pNormalHitbox->Update(centerPos);
	m_pSearch->Update(centerPos);

	centerPos.y += kModelDefaultSize / 2 * kModelSizeScale * 0.8f;
	m_pHeadHitbox->Update(centerPos);

	//敵(プレイヤー)の攻撃にあたった時
	if (m_pNormalHitbox->GetIsHit())
	{
		//HPを減らす処理を行う
		OnNormalDamage();
	}

	//敵(プレイヤー)の攻撃にあたった時
	if (m_pHeadHitbox->GetIsHit())
	{
		//HPを減らす処理を行う
		OnHeadDamage();
	}

	//HPが0以下になったら死亡する
	if (m_status.hp <= 0)
	{
		//死亡処理を行う
		Death();
	}

	//アニメーションブレンドの更新をする
	UpdateAnimationBlend();
	//武器の更新をする
	m_pWeapon->Update((m_collisionPos + m_moveVec * 4));
}

/// <summary>
/// 描画
/// </summary>
void Enemy::Draw()
{
	//存在していない状態なら何もさせない
	if (!m_isExist)return;

	//当たり判定座標を取得してモデルの描画座標を設定する
	SetDrawModelPos(kModelOffsetY * kModelSizeScale);
	//モデルの描画
	MV1DrawModel(m_modelHandle);
	//MV1DrawModel(m_weponHandle);
	m_pWeapon->Draw();

#ifdef _DEBUG
	//DrawFormatString(0, 176, 0xffffff, "エネミーステータス:%d,%d,%d", m_status.hp, m_status.atk, m_status.def);
#endif
}