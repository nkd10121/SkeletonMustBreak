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

	// �Փ˂����Ƃ�
	void OnCollideEnter(const std::shared_ptr<Collidable>& colider)override;
	void OnTriggerEnter(const std::shared_ptr<Collidable>& colider)override;

	void SetCameraAngle(MyLib::Vec3 m_dir) { m_cameraDirection = m_dir; }
	void SetCameraAngle(float ang) { m_cameraAngle = ang; }

	const MyLib::Vec3 GetPos()const { return rigidbody.GetPos(); }

	//�v���C���[�������Ă��邩���擾
	const bool IsGetPlayerDown()const { return m_isDown; }
	const bool IsGetPlayerDead()const { return m_isDead; }

	//�v���C���[����
	void PlayerRevival();

	void SetSlotNumMax(int max) { m_slotNumMax = max; }

	const int GetNowSlotNum()const { return m_nowSlotIdx; };

	const int GetDifAngle()const { return m_difAngle; }

	const int GetHp()const { return m_status.hp; }

	void SetGameClear();

private:

	void SetModelPos();

	/// <summary>
	/// �A�j���[�V�����̍X�V����
	/// </summary>
	/// <param name="attachNo">�i�s���������A�j���[�V�����ԍ�</param>
	/// <returns>���[�v�������ǂ���</returns>
	bool UpdateAnim(int attachNo, float startTime = 0.0f);

	/// <summary>
	///	�A�j���[�V�����̕ύX
	/// </summary>
	/// <param name="animIndex">�ύX��̃A�j���[�V�����ԍ�</param>
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

	/*�A�j���[�V�������*/
	int m_equipAnimNo;
	int m_currentAnimNo;	//���݂̃A�j���[�V����
	int m_prevAnimNo;		//�ύX�O�̃A�j���[�V����
	float m_animBlendRate;	//�A�j���[�V�����̍�������
	//0.0f:prev���Đ�
	//1.0f:currnt���Đ�

	float m_animSpeed;		//�A�j���[�V�����Đ����x
	bool m_isAnimationFinish;	//�A�j���[�V�������I��������ǂ���

	/*�J�������*/
	MyLib::Vec3  m_cameraDirection;	//�J�����������Ă������
	float  m_cameraAngle;		//�J�����̊p�x

	//�e�z��
	std::list<std::shared_ptr<Shot>> m_pShots;
	int shotTime;
	float m_shotOffsetPower;
	int m_difAngle;

	bool m_isDown;
	bool m_isDead;

	int m_jumpCount;

	//������A�^�b�`����t���[���ԍ�
	int m_weponAttachFrameNum;

	MATRIX m_weponFrameMat;

	int m_nowSlotIdx;
	int m_slotNumMax;



	//��ԑJ�ڂ̂��߂̃����o�֐�
	using UpdateFunc_t = void (Player::*)(std::shared_ptr<Input> input);
	UpdateFunc_t m_updateFunc;
	/*��Ԋ֐�*/
	//�j���[�g����(�ʏ�)
	void NeutralUpdate(std::shared_ptr<Input> input);
	//�W�����v
	void NormalJumpUpdate(std::shared_ptr<Input> input);
	//�ړ����̃W�����v
	void MovingJumpUpdate(std::shared_ptr<Input> input);
	//�U��
	void AttackUpdate(std::shared_ptr<Input> input);
	//����
	void WalkUpdate(std::shared_ptr<Input> input);

	void DeathUpdate(std::shared_ptr<Input> input);

	void ClearUpdate(std::shared_ptr<Input> input);
};

