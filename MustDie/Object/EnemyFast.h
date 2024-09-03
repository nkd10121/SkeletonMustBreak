#pragma once
#include "DxLib.h"
#include "MyLib.h"
#include "EnemyBase.h"
#include <map>

class EnemyFast : public EnemyBase
{
public:
	EnemyFast(int modelH);
	virtual ~EnemyFast();

	void Init(std::shared_ptr<MyLib::Physics>physics, std::vector<MyLib::Vec3> route) override;
	void Finalize(std::shared_ptr<MyLib::Physics> physics) override;
	void Update(MyLib::Vec3 playerPos, bool isChase);
	void Draw();

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

	int m_tempFrameCount;	//�����Ɣj�����Ǘ����邽�߂̈ꎞ�I�ϐ�(�p���ς񂾂�����Ă悵)

	int m_weponAttachFrameNum;

	int m_attackWaitFrame;

	int m_knockCount;

	/*�A�j���[�V�����֌W*/
	int m_currentAnimNo;	//���݂̃A�j���[�V����
	int m_prevAnimNo;		//�ύX�O�̃A�j���[�V����
	float m_animBlendRate;	//�A�j���[�V�����̍�������
	//0.0f:prev���Đ�
	//1.0f:currnt���Đ�

	float m_animSpeed;
	bool m_isAnimationFinish;

	std::vector<MyLib::Vec3> m_route;
	int m_routeNum;
	MyLib::Vec3 m_destinationPos;

	bool m_isChasing;

	//��ԑJ�ڂ̂��߂̃����o�֐�
	using UpdateFunc_t = void (EnemyFast::*)(MyLib::Vec3 playerPos, bool isChase);
	UpdateFunc_t m_updateFunc;

	//�U��
	void AttackUpdate(MyLib::Vec3 playerPos, bool isChase);
	//����
	void WalkUpdate(MyLib::Vec3 playerPos, bool isChase);
	//�U���q�b�g
	void HitUpdate(MyLib::Vec3 playerPos, bool isChase);

	void DeathUpdate(MyLib::Vec3 playerPos, bool isChase);
};

