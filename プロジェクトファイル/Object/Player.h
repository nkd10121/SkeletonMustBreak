#pragma once
#include "DxLib.h"
#include "MyLib.h"
#include "Vec3.h"
#include "CharacterBase.h"
#include <list>
#include <memory>

class Input;
class Shot;

class Player : public CharacterBase
{
public:
	Player();
	virtual ~Player();

	void Init(std::shared_ptr<MyLib::Physics> physics, int* arrow);
	void Finalize();
	void Update(std::shared_ptr<Input>& input);
	void Draw();

	// 衝突したとき
	void OnCollideEnter(const std::shared_ptr<Collidable>& colider)override;
	void OnTriggerEnter(const std::shared_ptr<Collidable>& colider)override;

	void SetCameraAngle(MyLib::Vec3 m_dir) { m_cameraDirection = m_dir; }
	void SetCameraAngle(float ang) { m_cameraAngle = ang; }

	const MyLib::Vec3 GetPos()const { return rigidbody.GetPos(); }

	//プレイヤーが生きているかを取得
	const bool IsGetPlayerDown()const { return m_isDown; }
	const bool IsGetPlayerDead()const { return m_isDead; }

	//プレイヤー復活
	void PlayerRevival();

	void SetSlotNumMax(int max) { m_slotNumMax = max; }

	const int GetNowSlotNum()const { return m_nowSlotIdx; };

	const int GetDifAngle()const { return m_difAngle; }

	const int GetHp()const { return m_status.hp; }

	void SetGameClear();

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
	std::shared_ptr<MyLib::Physics> m_pPhysics;

	const enum eAnimIdx : int
	{
		test = 73,
		Idle = 1,
		Walk = 4,
		RunRight = 8,
		RunLeft = 9,
		Jump = 11,
		Cheer = 23,
		Aim = 56,
		Shoot = 57,
		Shooting = 58,
		Death = 28,
		DeathPose = 29
	};
	eAnimIdx m_nowAnimIdx;

	int m_hpMax;

	int m_moveCount;

	int m_weponHandle;
	float m_angle;

	int m_arrowHandle;

	bool m_isMove;

	bool m_isClear;

	/*アニメーション情報*/
	int m_equipAnimNo;
	int m_currentAnimNo;	//現在のアニメーション
	int m_prevAnimNo;		//変更前のアニメーション
	float m_animBlendRate;	//アニメーションの合成割合
	//0.0f:prevが再生
	//1.0f:currntが再生

	float m_animSpeed;		//アニメーション再生速度
	bool m_isAnimationFinish;	//アニメーションが終わったかどうか

	/*カメラ情報*/
	MyLib::Vec3  m_cameraDirection;	//カメラが向いている向き
	float  m_cameraAngle;		//カメラの角度

	//弾配列
	std::list<std::shared_ptr<Shot>> m_pShots;
	int shotTime;
	float m_shotOffsetPower;
	int m_difAngle;

	bool m_isDown;
	bool m_isDead;

	int m_jumpCount;

	//武器をアタッチするフレーム番号
	int m_weponAttachFrameNum;

	MATRIX m_weponFrameMat;

	int m_nowSlotIdx;
	int m_slotNumMax;



	//状態遷移のためのメンバ関数
	using UpdateFunc_t = void (Player::*)(std::shared_ptr<Input> input);
	UpdateFunc_t m_updateFunc;
	/*状態関数*/
	//ニュートラル(通常)
	void NeutralUpdate(std::shared_ptr<Input> input);
	//ジャンプ
	void NormalJumpUpdate(std::shared_ptr<Input> input);
	//移動中のジャンプ
	void MovingJumpUpdate(std::shared_ptr<Input> input);
	//攻撃
	void AttackUpdate(std::shared_ptr<Input> input);
	//歩き
	void WalkUpdate(std::shared_ptr<Input> input);

	void DeathUpdate(std::shared_ptr<Input> input);

	void ClearUpdate(std::shared_ptr<Input> input);
};

