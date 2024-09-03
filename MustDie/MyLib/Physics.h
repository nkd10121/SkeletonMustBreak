// 2024 Takeru Yui All Rights Reserved.
#pragma once
#include "Vec3.h"

#include <list>
#include <vector>
#include <unordered_map>

namespace MyLib {

class Collidable;

namespace ColInfo
{
	//�ő哖���蔻��|���S����
	constexpr int kMaxColHitPolyNum = 2000;
	//�ǉ����o�������̍ő厎�s��
	constexpr int kMaxColHitTryNum = 16;
	//�ǉ����o�����ɃX���C�h�����鋗��
	constexpr float kColHitSlideLength = 1.0f;
}

/// <summary>
/// �����E�Փ˔��肷��I�u�W�F�N�g��o�^���A�����ړ��E�Փ˂�ʒm����
/// </summary>
class Physics final
{
private:
	//�����蔻��̎��
	enum class eOnCollideInfoKind
	{
		CollideEnter,
		CollideStay,
		CollideExit,
		TriggerEnter,
		TriggerStay,
		TriggerExit
	};

	//�Ȃɂ��Ɠ��������I�u�W�F�N�g�̏��
	struct OnCollideInfoData
	{
		std::shared_ptr<Collidable> own;	//�������g
		std::shared_ptr<Collidable> send;	//������������
		eOnCollideInfoKind kind;			//���
	};

	using SendCollideInfo = std::unordered_map<std::shared_ptr<Collidable>, std::list<std::shared_ptr<Collidable>>>;

public:
	Physics(int stageCollisionHandle);

	// �Փ˕��̓o�^�E�o�^����
	void Entry(std::shared_ptr<Collidable> collidable);
	void Exit(std::shared_ptr<Collidable> collidable);

	void Update();	// �X�V�i�o�^�I�u�W�F�N�g�̕����ړ��A�Փ˒ʒm�j

private:
	// �����蔻��`�F�b�N
	void CheckColide();

	bool IsCollide(const Rigidbody& rigidA, const Rigidbody& rigidB, ColliderData* colliderA, ColliderData* colliderB) const;

	void AddNewCollideInfo(const std::shared_ptr<Collidable>& objA, const std::shared_ptr<Collidable>& objB, SendCollideInfo& info);

	// �ʒu�␳�A����
	void FixNextPosition(const Rigidbody& primaryRigid, Rigidbody& secondaryRigid, ColliderData* primaryCollider, ColliderData* secondaryCollider) const;
	
	void CheckSendOnCollideInfo(SendCollideInfo& preSendInfo, SendCollideInfo& newSendInfo, bool isTrigger);
	void AddOnCollideInfo(const std::shared_ptr<Collidable>& own, const std::shared_ptr<Collidable>& send, eOnCollideInfoKind kind);

	void FixPosition();


	Vec3 GetClosestPtOnSegment(Vec3 pt, Vec3 start, Vec3 end);

private:	//�X�e�[�W�����蔻��e�X�g
	//�`�F�b�N�����|���S�����ǃ|���S�������|���S�����𔻒f���ۑ�����
	void CheckWallAndFloor(std::shared_ptr<Collidable>& col);
	//�ǃ|���S���Ƃ̓����蔻����`�F�b�N���A�ړ�������
	void FixPositionWithWall(std::shared_ptr<Collidable>& col);
	void FixPositionWithWallInternal(std::shared_ptr<Collidable>& col);
	//���|���S���Ƃ̓����蔻����`�F�b�N���A�ړ�������
	void FixNowPositionWithFloor(std::shared_ptr<Collidable>& col);

private:
	// �d�͂ƍő�d�͉����x
	static constexpr float Gravity = -0.01f;
	static constexpr float MaxGravityAccel = -0.30f;

	static const int BeforeFixInfoColor = 0x0000ff;	// �␳�O���F
	static const int AimInfoColor = 0x00ff00;	// �␳�O�\����F
	static const int AfterFixInfoColor = 0xff00ff;	// �␳����F

	std::list<std::shared_ptr<Collidable>> m_collidables;	// �o�^���ꂽCollidable�̃��X�g

	std::list<OnCollideInfoData> m_onCollideInfo;

	SendCollideInfo m_newCollideInfo;
	SendCollideInfo m_preCollideInfo;
	SendCollideInfo m_newTirrigerInfo;
	SendCollideInfo m_preTirrigerInfo;


	int m_stageCollisionHandle;

private:	//�X�e�[�W�����蔻��e�X�g
	bool m_isMoveFlag = false;		//�ړ��������ǂ���
	bool m_isHitFlag = false;		//�|���S���ɓ����������ǂ���
	int m_wallNum = 0;				//�ǃ|���S���Ɣ��f���ꂽ�|���S����
	int m_floorNum = 0;				//���|���S���Ɣ��f���ꂽ�|���S����

	MV1_COLL_RESULT_POLY_DIM m_hitDim{};								//�����蔻�茋�ʍ\����
	MV1_COLL_RESULT_POLY* m_pWallPoly[ColInfo::kMaxColHitPolyNum]{};	// �ǃ|���S���Ɣ��f���ꂽ�|���S���̍\���̂̃A�h���X��ۑ����Ă������߂̃|�C���^�z��
	MV1_COLL_RESULT_POLY* m_pFloorPoly[ColInfo::kMaxColHitPolyNum]{};	// ���|���S���Ɣ��f���ꂽ�|���S���̍\���̂̃A�h���X��ۑ����Ă������߂̃|�C���^�z��
	MV1_COLL_RESULT_POLY* m_pPoly = nullptr;							// �|���S���̍\���̂ɃA�N�Z�X���邽�߂Ɏg�p����|�C���^
	HITRESULT_LINE m_lineRes{};											// �����ƃ|���S���Ƃ̓����蔻��̌��ʂ�������\����

};

}
