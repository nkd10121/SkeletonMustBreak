#include "EnemyBig.h"
#include "CsvLoad.h"
#include "WeaponBase.h"
#include "Sword.h"

#include "HitBox.h"
#include "SearchObject.h"

#include "SoundManager.h"
#include "ModelManager.h"

namespace
{
	//キャラクター名
	constexpr const char* kCharacterName = "BigSkelton";
	//モデルパス
	constexpr const char* kModelPath = "data/model/SkeletonWarrior.mv1";
	//モデルの元のサイズ
	constexpr float kModelDefaultSize = 2.590f;
	//モデルサイズの拡大率
	constexpr float kModelSizeScale = 6.8f;
	constexpr float kModelOffsetY = 0.3f;
	//武器のモデルサイズ
	constexpr float kWeaponModelSize = 0.01f;
	//押し出し当たり判定の半径
	constexpr float kCollisionRadius = 2.0f;
	//索敵範囲
	constexpr float kSearchingRadius = 30.0f;
	//当たり判定の半径
	constexpr float kHitBoxRadius = 6.0f;
}

/// <summary>
/// コンストラクタ
/// </summary>
EnemyBig::EnemyBig():
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
EnemyBig::~EnemyBig()
{
}

/// <summary>
/// 初期化
/// </summary>
/// <param name="physics">物理クラスのポインタ</param>
/// <param name="route">移動ルート</param>
void EnemyBig::Init(std::shared_ptr<MyLib::Physics> physics, std::vector<MyLib::Vec3> route)
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
	InitHitBox(kHitBoxRadius);

	//索敵判定をする当たり判定を作成
	InitSearch(kModelDefaultSize * kModelSizeScale / 2);

	//モデルのサイズ設定
	MV1SetScale(m_modelHandle, VGet(kModelSizeScale, kModelSizeScale, kModelSizeScale));

	//アニメーションを設定
	m_currentAnimNo = MV1AttachAnim(m_modelHandle, m_animIdx["Move"]);
	m_nowAnimIdx = m_animIdx["Move"];

	//通常状態に設定しておく
	m_updateFunc = &EnemyBig::WalkUpdate;
}

/// <summary>
/// 更新
/// </summary>
/// <param name="playerPos">プレイヤー座標</param>
/// <param name="isChase">プレイヤーが追跡できる状態かどうか</param>
void EnemyBig::Update(MyLib::Vec3 playerPos, bool isChase)
{
	//存在していない状態なら何もさせない
	if (!m_isExist)return;

	//アニメーションの更新
	m_isAnimationFinish = UpdateAnim(m_currentAnimNo);

	//状態の更新
	(this->*m_updateFunc)(playerPos, isChase);

	//中心座標の設定(ミニマップに表示するために必要)
	CalculationCenterPos(kModelDefaultSize, kModelSizeScale);

	MyLib::Vec3 centerPos = rigidbody.GetPos();
	centerPos.y += kModelDefaultSize / 2 * kModelSizeScale * 0.7f;
	m_pHitbox->Update(centerPos);
	m_pSearch->Update(centerPos);

	//敵(プレイヤー)の攻撃にあたった時
	if (m_pHitbox->GetIsHit())
	{
		//HPを減らす処理を行う
		OnDamage();
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
	m_pWeapon->Update((m_collisionPos + m_moveVec * 6));
}

/// <summary>
/// 描画
/// </summary>
void EnemyBig::Draw()
{
	//存在していない状態なら何もさせない
	if (!m_isExist)return;

	//当たり判定座標を取得してモデルの描画座標を設定する
	SetDrawModelPos(kModelOffsetY * kModelSizeScale);
	//モデルの描画
	MV1DrawModel(m_modelHandle);
	m_pWeapon->Draw();

#ifdef _DEBUG
	DrawFormatString(800, 60, 0xffffff, "%f,%f,%f", m_modelPos.x, m_modelPos.y, m_modelPos.z);
	DrawFormatString(800, 76, 0xffffff, "%f,%f,%f", m_collisionPos.x, m_collisionPos.y, m_collisionPos.z);
#endif
}