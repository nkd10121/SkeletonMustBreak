#pragma once
#include "CharacterBase.h"
#include  <string>

class WeaponBase;
class HitBox;
class SearchObject;

/// <summary>
/// エネミーの基盤となるクラス
/// </summary>
class EnemyBase : public CharacterBase
{
protected:
	//武器の種類列挙型
	enum WeaponKind
	{
		Blade
	};
public:
	//コンストラクタ
	EnemyBase(Priority priority);
	//デストラクタ
	~EnemyBase();

	//初期化
	virtual void Init(std::shared_ptr<MyLib::Physics>physics, std::vector<MyLib::Vec3> route) {};
	//終了
	virtual void Finalize(std::shared_ptr<MyLib::Physics> physics);
	//更新
	virtual void Update(MyLib::Vec3 playerPos, bool isChase) {};
	//描画
	virtual void Draw() {};

	// 衝突したとき
	void OnCollideEnter(const std::shared_ptr<Collidable>& colider)override;
	void OnCollideStay(const std::shared_ptr<Collidable>& colider)override;

	//中心座標を返す
	virtual  MyLib::Vec3 GetCenterPos()const { return m_centerPos; }
	//足元座標を返す
	virtual  MyLib::Vec3 GetPos()const { return rigidbody.GetPos(); }
	//ダメージを食らったかどうかを取得
	bool GetIsHit();
	//死亡時に落とす罠ポイントを取得
	int GetDropPoint();
	//死亡して罠を落としたかどうかを取得
	bool GetIsDropedPoint()const { return m_isDroped; }
	//死亡しているかどうかを取得
	bool GetIsDead()const { return m_isDead; }
	//存在しているかどうかを取得
	const bool GetIsExist()const { return m_isExist; }
	//目的地に到達しているかどうかを取得
	const bool GetIsReach()const { return m_isReach; }
	//最後に攻撃してきたオブジェクトのタグを取得
	const GameObjectTag GetLastHitObjectTag()const { return m_lastHitObjectTag; }
	//HPを表示するための当たり判定の半径を取得
	const float GetRadius()const;

	//レイキャストをするためにモデルハンドルを取得
	const int GetModelHandle()const { return CharacterBase::m_modelHandle; }
	//現在のHPを取得
	const int GetHp()const { return CharacterBase::m_status.hp; }

protected:
	//ほかのオブジェクトと押し出し判定をする当たり判定を作成
	void InitCollision(float radius);
	//モデルを読み込む
	void LoadModel(std::string path);
	//アニメーション情報や、ステータス情報を読み込む
	void LoadData(std::string name);
	//武器を生成する
	void CreateWeapon(WeaponKind kind, float modelSize);
	//ルートをモデルの高さに合うように調整する
	void AdjustmentRoute(float modelOffesetY,float modelSize);
	//物理クラスの初期化
	void InitRigidbody(bool isUseGravity = true);
	//モデルの中心座標を計算
	void CalculationCenterPos(float modeldefaultSize,float modelSize);
	//モデル座標を設定
	void SetModelPos(float offset);
	//ダメージ判定をする当たり判定を作成
	void InitNormalHitBox(float radius);
	//ダメージ判定をする当たり判定を作成
	void InitHeadHitBox(float radius);
	//索敵判定をする当たり判定を作成
	void InitSearch(float radius);
	//ダメージを受けたとき
	void OnNormalDamage();
	//ダメージを受けたとき
	void OnHeadDamage();
	//死亡した時
	void Death();
	//アニメーションブレンドの更新
	void UpdateAnimationBlend();

	//モデルの座標設定
	void SetDrawModelPos(float offset);
protected:
	std::shared_ptr<WeaponBase> m_pWeapon;		//武器ポインタ
	std::shared_ptr<HitBox> m_pNormalHitbox;	//ダメージ判定をする当たり判定
	std::shared_ptr<HitBox> m_pHeadHitbox;		//ダメージ判定をする当たり判定
	std::shared_ptr<SearchObject> m_pSearch;	//索敵判定をする当たり判定
	
	int m_dropPoint;		//死亡時にドロップするポイント
	int m_attackWaitFrame;	//攻撃までの待機フレーム
	int m_knockCount;		//のけぞりフレーム
	int m_routeIdx;			//現在の目的地

	float m_searchRange;	//索敵範囲
	float m_radius;			//HPバーを表示する当たり判定の半径

	bool m_isExist;		//存在するかどうか
	bool m_isReach;		//最終目的地に着いたかどうか
	bool m_isHit;		//ダメージを受けたかどうか
	bool m_isDead;		//死亡したかどうか
	bool m_isDroped;	//ポイントをドロップしたかどうか
	bool m_isAttack;	//攻撃しているかどうか
	bool m_isKnock;		//のけぞりしているかどうか
	bool m_isChase;		//プレイヤーを追うかどうか
	bool m_isChasing;	//現在プレイヤーを追っているかどうか

	std::vector<MyLib::Vec3> m_route;		//移動ルート
	
	MyLib::Vec3 m_destinationPos;	//次の目的座標
	MyLib::Vec3 m_centerPos;		//中心座標

	GameObjectTag m_lastHitObjectTag;	//最後に攻撃してきたオブジェクトのタグ

	//状態遷移のためのメンバ関数
	using UpdateFunc_t = void (EnemyBase::*)(MyLib::Vec3 playerPos, bool isChase);
	UpdateFunc_t m_updateFunc;

	//攻撃
	void AttackUpdate(MyLib::Vec3 playerPos, bool isChase);
	//歩き
	void WalkUpdate(MyLib::Vec3 playerPos, bool isChase);
	//攻撃ヒット
	void HitUpdate(MyLib::Vec3 playerPos, bool isChase);

	void DeathUpdate(MyLib::Vec3 playerPos, bool isChase);
};

