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

	void Init(std::shared_ptr<MyLib::Physics> physics);
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

	void SetSlotNumMax(int max) { m_slotNumMax = max; }

	const int GetNowSlotNum()const { return m_nowSlotIdx; };

	const int GetDifAngle()const { return m_difAngle; }


	void SetGameClear();

private:
	void SetModelPos();
private:
	int m_hpMax;

	int m_moveCount;

	int m_weponHandle;
	float m_angle;

	bool m_isMove;

	bool m_isClear;

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
	//歩き
	void WalkUpdate(std::shared_ptr<Input> input);
	//死亡
	void DeathUpdate(std::shared_ptr<Input> input);
};

