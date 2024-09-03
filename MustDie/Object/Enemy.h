#pragma once
#include "DxLib.h"
#include "MyLib.h"
#include "EnemyBase.h"
#include <map>

class WeaponBase;

class Enemy : public EnemyBase
{
public:
	Enemy(int modelH,int weponH);
	virtual ~Enemy();

	void Init(std::shared_ptr<MyLib::Physics>physics, std::vector<MyLib::Vec3> route) override;
	void Finalize(std::shared_ptr<MyLib::Physics>) override;
	void Update(MyLib::Vec3 playerPos, bool isChase);
	void Draw();

	int GetDropPoint();
	bool GetIsDropedPoint()const { return m_isDroped; };
	bool GetIsDead()const { return m_isDead; };

private:

	void SetModelPos();

	/// <summary>
	/// アニメーションの更新処理
	/// </summary>
	/// <param name="attachNo">進行させたいアニメーション番号</param>
	/// <returns>ループしたかどうか</returns>
	bool UpdateAnim(int attachNo, float startTime = 0.0f);

	/// <summary>
	///	アニメーションの変更
	/// </summary>
	/// <param name="animIndex">変更後のアニメーション番号</param>
	void ChangeAnim(int animIndex, float animSpeed = 0.5f);
private:
	std::shared_ptr<MyLib::Physics> m_pPhisics;

	const enum eAnimIdx : int
	{
		Idle = 42,
		Run = 54,
		Attack = 4,
		Hit = 39,
		DeathA = 25,
		DeathB = 27,
		DeathC = 30,
	};

	eAnimIdx m_nowAnimIdx;

	int m_weponHandle;

	int m_tempFrameCount;	//生成と破棄を管理するための一時的変数(用が済んだら消してよし)

	int m_weponAttachFrameNum;

	std::shared_ptr<WeaponBase> m_weapon;

	int m_attackWaitFrame;

	int m_knockCount;

	bool m_isDead;

	int m_dropPoint;
	bool m_isDroped;

	/*アニメーション関係*/
	int m_currentAnimNo;	//現在のアニメーション
	int m_prevAnimNo;		//変更前のアニメーション
	float m_animBlendRate;	//アニメーションの合成割合
	//0.0f:prevが再生
	//1.0f:currntが再生

	float m_animSpeed;
	bool m_isAnimationFinish;

	std::vector<MyLib::Vec3> m_route;
	int m_routeNum;
	MyLib::Vec3 m_destinationPos;

	bool m_isChasing;

	//状態遷移のためのメンバ関数
	using UpdateFunc_t = void (Enemy::*)(MyLib::Vec3 playerPos, bool isChase);
	UpdateFunc_t m_updateFunc;

	//攻撃
	void AttackUpdate(MyLib::Vec3 playerPos, bool isChase);
	//歩き
	void WalkUpdate(MyLib::Vec3 playerPos, bool isChase);
	//攻撃ヒット
	void HitUpdate(MyLib::Vec3 playerPos, bool isChase);

	void DeathUpdate(MyLib::Vec3 playerPos, bool isChase);
};

