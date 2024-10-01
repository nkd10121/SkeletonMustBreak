#include <cassert>
#include "MyLib.h"
#include "DxLib.h"

//using namespace MyLib;

namespace
{
	//�ő哖���蔻��|���S����
	constexpr int kMaxColHitPolyNum = 2000;
	//�ǉ����o�������̍ő厎�s��
	constexpr int kMaxColHitTryNum = 16;
	//�ǉ����o�����ɃX���C�h�����鋗��
	constexpr float kColHitSlideLength = 0.2f;
	//�ǃ|���S�������|���S�����𔻒f���邽�߂̕ϐ�
	constexpr float kWallPolyBorder = 0.4f;
	//�ǃ|���S���Ɣ��f���邽�߂̍����ϐ�
	constexpr float kWallPolyHeight = 5.0f;

	//�d��
	constexpr float kGravity = -0.018f;
	//�ő�d�͉����x
	constexpr float kMaxGravityAccel = -2.00f;

	//�␳�O���F
	const int kBeforeFixInfoColor = 0x0000ff;
	//�␳�O�\����F
	const int kAimInfoColor = 0x00ff00;
	//�␳����F
	const int kAfterFixInfoColor = 0xff00ff;
}

/// <summary>
/// �R���X�g���N�^
/// </summary>
/// <param name="normalStageCollisionHandle">�v���C���[�Ȃǂ̃X�e�[�W�����蔻��n���h��</param>
/// <param name="enemyStageCollisionHandle">�G�̃X�e�[�W�����蔻��n���h��</param>
MyLib::Physics::Physics(int normalStageCollisionHandle,int enemyStageCollisionHandle)
{
	m_stageCollisionHandle = normalStageCollisionHandle;
	m_enemyCollisionHandle = enemyStageCollisionHandle;
}

/// <summary>
/// �Փ˕��̓o�^
/// </summary>
/// <param name="collidable">�ǉ����铖���蔻��</param>
void MyLib::Physics::Entry(std::shared_ptr<Collidable> collidable)
{
	// �o�^
	bool found = (std::find(m_collidables.begin(), m_collidables.end(), collidable) != m_collidables.end());
	if (!found)
	{
		m_collidables.emplace_back(collidable);
		//collidables.push_back(collidable);
	}
	// ���ɓo�^����Ă���G���[
	else
	{
		assert(0 && "�w���collidable�͓o�^�ςł��B");
	}
}

/// <summary>
/// �Փ˕��̓o�^�폜
/// </summary>
/// <param name="collidable">�폜���铖���蔻��</param>
void MyLib::Physics::Exit(std::shared_ptr<Collidable> collidable)
{
	bool found = (std::find(m_collidables.begin(), m_collidables.end(), collidable) != m_collidables.end());
	if (found)
	{
		m_collidables.remove(collidable);
	}
	// �o�^����ĂȂ�������G���[
	else
	{
		assert(0 && "�w���collidable���o�^����Ă��܂���B");
	}
}

/// <summary>
/// �X�V�i�o�^�I�u�W�F�N�g�̕����ړ��A�Փ˒ʒm�j
/// </summary>
void MyLib::Physics::Update()
{
	//�����蔻��̃��O���c��
	m_preCollideInfo = m_newCollideInfo;
	m_newCollideInfo.clear();
	m_preTirrigerInfo = m_newTirrigerInfo;
	m_newTirrigerInfo.clear();
	m_onCollideInfo.clear();

	// �ړ�
	for (auto& item : m_collidables)
	{
		// �|�W�V�����Ɉړ��͂𑫂�
		auto pos = item->rigidbody.GetPos();
		auto m_velocity = item->rigidbody.GetVelocity();

		// �d�͂𗘗p����ݒ�Ȃ�A�d�͂�ǉ�����
		if (item->rigidbody.GetUseGravity())
		{
			m_velocity = m_velocity + Vec3(0.0f, kGravity, 0.0f);

			// �ő�d�͉����x���傫��������N�����v
			if (m_velocity.y < kMaxGravityAccel)
			{
				m_velocity = MyLib::Vec3(m_velocity.x, kMaxGravityAccel, m_velocity.z);
			}
		}

		auto nextPos = pos + m_velocity;

		item->rigidbody.SetVelocity(m_velocity);

		// ���Ƃ��Ƃ̏��A�\������f�o�b�O�\��
#if _DEBUG

		for (const auto& collider : item->m_colliders)
		{
			auto kind = collider->GetKind();
			if (kind == ColliderData::Kind::Sphere)
			{
				auto sphereData = dynamic_cast<MyLib::ColliderDataSphere*> (collider.get());
				auto radius = sphereData->m_radius;
				MyLib::DebugDraw::AddDrawSphere(pos, radius, kBeforeFixInfoColor);
				MyLib::DebugDraw::AddDrawSphere(nextPos, radius, kAimInfoColor);
			}
		}

#endif
		// �\��|�W�V�����ݒ�
		item->rigidbody.SetNextPos(nextPos);
	}

	// �����蔻��`�F�b�N�inextPos�w��j
	CheckColide();

	for (auto& item : m_collidables)
	{
		if (item->GetTag() == GameObjectTag::Player || item->GetTag() == GameObjectTag::Enemy)
		{
			float rad = 0;
			int modelHandle = -1;

			for (auto& col : item->m_colliders)
			{
				rad = dynamic_cast<MyLib::ColliderDataSphere*> (col.get())->m_radius;

				if (item->GetTag() == GameObjectTag::Player)
				{
					modelHandle = m_stageCollisionHandle;
				}
				else
				{
					modelHandle = m_enemyCollisionHandle;
				}
			}
			m_hitDim = MV1CollCheck_Sphere(modelHandle, -1, item->rigidbody.GetNextPosVECTOR(), rad);
		}
		else
		{
			continue;
		}

		//�ǂƏ��̓����蔻����s��
		CheckWallAndFloor(item);
		//�ǂƂ̓����蔻�菈��
		FixPositionWithWall(item);
		//���Ƃ̓����蔻�菈��
		FixNowPositionWithFloor(item);

		// ���o�����v���C���[�̎��͂̃|���S�������J������
		MV1CollResultPolyDimTerminate(m_hitDim);
	}

	CheckSendOnCollideInfo(m_preCollideInfo, m_newCollideInfo, false);
	CheckSendOnCollideInfo(m_preTirrigerInfo, m_newTirrigerInfo, true);

	// �ʒu�m��
	FixPosition();

	for (const auto& info : m_onCollideInfo)
	{
		if (info.kind == eOnCollideInfoKind::CollideEnter)
		{
			info.own->OnCollideEnter(info.send);
		}
		else if (info.kind == eOnCollideInfoKind::CollideStay)
		{
			info.own->OnCollideStay(info.send);
		}
		else if (info.kind == eOnCollideInfoKind::CollideExit)
		{
			info.own->OnCollideExit(info.send);
		}
		else if (info.kind == eOnCollideInfoKind::TriggerEnter)
		{
			info.own->OnTriggerEnter(info.send);
		}
		else if (info.kind == eOnCollideInfoKind::TriggerStay)
		{
			info.own->OnTriggerStay(info.send);
		}
		else if (info.kind == eOnCollideInfoKind::TriggerExit)
		{
			info.own->OnTriggerExit(info.send);
		}
	}
}

/// <summary>
/// �����蔻��`�F�b�N
/// </summary>
void MyLib::Physics::CheckColide()
{
	std::vector<OnCollideInfoData> onCollideInfo;
	// �Փ˒ʒm�A�|�W�V�����␳
	bool	doCheck = true;
	int		checkCount = 0;	// �`�F�b�N��
	while (doCheck)
	{
		doCheck = false;
		++checkCount;

		// 2�d���[�v�őS�I�u�W�F�N�g�����蔻��
		// FIXME: �d���̂ŋ߂��I�u�W�F�N�g���m�̂ݓ����蔻�肷��ȂǍH�v������
		for (const auto& objA : m_collidables)
		{
			for (const auto& objB : m_collidables)
			{
				//����I�u�W�F�N�g�Ȃ瑁�����^�[��
				if (objA == objB)
					continue;

				for (const auto& colA : objA->m_colliders)
				{
					for (const auto& colB : objB->m_colliders)
					{
						if (!IsCollide(objA->rigidbody, objB->rigidbody, colA.get(), colB.get())) continue;

						bool isTrigger = colA->IsTrigger() || colB->IsTrigger();

						if (isTrigger)
						{
							AddNewCollideInfo(objA, objB, m_newTirrigerInfo);
						}
						else
						{
							AddNewCollideInfo(objA, objB, m_newCollideInfo);
						}

						// Trigger�̏ꍇ�͈ʒu�␳�͂��Ȃ�
						if (isTrigger) continue;

						auto primary = objA;
						auto secondary = objB;

						if (primary == secondary)
						{
							break;
						}

						auto primaryCollider = colA;
						auto secondaryCollider = colB;
						if (objA->priority < objB->priority)
						{
							primary = objB;
							secondary = objA;
							primaryCollider = colB;
							secondaryCollider = colA;
						}

						FixNextPosition(primary->rigidbody, secondary->rigidbody, primaryCollider.get(), secondaryCollider.get());
						// �ʒu�␳�������������x���߂���s��
						doCheck = true;
						break;
					}
					if (doCheck) break;
				}
				if (doCheck) break;
			}
			if (doCheck) break;
		}
		if (doCheck && checkCount > 1000)
		{
#if _DEBUG
			printfDx("�K��񐔂𒴂��܂���");
#endif
			break;
		}
	}
}

/// <summary>
/// /��̃I�u�W�F�N�g���ڐG���Ă��邩�ǂ���
/// </summary>
/// <param name="rigidA">�I�u�W�F�N�gA�̕����f�[�^</param>
/// <param name="rigidB">�I�u�W�F�N�gB�̕����f�[�^</param>
/// <param name="colliderA">�I�u�W�F�N�gA�̓����蔻��f�[�^</param>
/// <param name="colliderB">�I�u�W�F�N�gB�̓����蔻��f�[�^</param>
/// <returns></returns>
bool MyLib::Physics::IsCollide(const Rigidbody& rigidA, const Rigidbody& rigidB, ColliderData* colliderA, ColliderData* colliderB) const
{
	bool isCollide = false;

	auto kindA = colliderA->GetKind();
	auto kindB = colliderB->GetKind();

	if (kindA == MyLib::ColliderData::Kind::Sphere && kindB == MyLib::ColliderData::Kind::Sphere)
	{
		auto colA = dynamic_cast<MyLib::ColliderDataSphere*>(colliderA);
		auto colB = dynamic_cast<MyLib::ColliderDataSphere*>(colliderB);

		auto atob = rigidA.GetNextPos() - rigidB.GetNextPos();
		auto atobLength = atob.Size();

		// ���݂��̋������A���ꂼ��̔��a�𑫂������̂�菬������Γ�����
		isCollide = (atobLength < colA->m_radius + colB->m_radius);
	}
	return isCollide;
}

/// <summary>
/// ���������I�u�W�F�N�g�̃y�A��o�^����
/// </summary>
/// <param name="objA">�I�u�W�F�N�gA</param>
/// <param name="objB">�I�u�W�F�N�gB</param>
/// <param name="info">�o�^����z��</param>
void MyLib::Physics::AddNewCollideInfo(const std::shared_ptr<Collidable>& objA, const std::shared_ptr<Collidable>& objB, SendCollideInfo& info)
{
	bool isParentA = info.find(objA) != info.end();
	bool isParentB = info.find(objB) != info.end();
	if (isParentA || isParentB)
	{
		std::shared_ptr<Collidable> parent = objA;
		std::shared_ptr<Collidable> child = objB;
		if (isParentB)
		{
			parent = objB;
			child = objA;
		}
		bool isChild = std::find(info[parent].begin(), info[parent].end(), child) != info[parent].end();
		if (!isChild)
		{
			info[parent].emplace_back(child);
		}
	}
	else
	{
		info[objA].emplace_back(objB);
	}
}

/// <summary>
/// �ړ��\��̍��W���C������
/// </summary>
/// <param name="primaryRigid">�D��x���I�u�W�F�N�g�̕����f�[�^</param>
/// <param name="secondaryRigid">�D��x��I�u�W�F�N�g�̕����f�[�^</param>
/// <param name="primaryCollider">�D��x���I�u�W�F�N�g�̓����蔻��f�[�^</param>
/// <param name="secondaryCollider">�D��x��I�u�W�F�N�g�̓����蔻��f�[�^</param>
void MyLib::Physics::FixNextPosition(const Rigidbody& primaryRigid, Rigidbody& secondaryRigid, ColliderData* primaryCollider, ColliderData* secondaryCollider) const
{
	// �����蔻��̎�ʂ��Ƃɕ␳���@��ς���
	auto primaryKind = primaryCollider->GetKind();
	auto secondaryKind = secondaryCollider->GetKind();

	if (primaryKind == MyLib::ColliderData::Kind::Sphere && secondaryKind == MyLib::ColliderData::Kind::Sphere)
	{
		auto primaryToSecondary = secondaryRigid.GetNextPos() - primaryRigid.GetNextPos();
		auto primaryToSecondaryN = primaryToSecondary.Normalize();

		auto primaryColliderData = dynamic_cast<MyLib::ColliderDataSphere*>(primaryCollider);
		auto secondaryColliderData = dynamic_cast<MyLib::ColliderDataSphere*>(secondaryCollider);
		auto awayDist = primaryColliderData->m_radius + secondaryColliderData->m_radius + 0.0001f;	// ���̂܂܂��Ƃ��傤�Ǔ�����ʒu�ɂȂ�̂ŏ����]���ɗ���
		auto primaryToNewSecondaryPos = primaryToSecondaryN * awayDist;
		auto fixedPos = primaryRigid.GetNextPos() + primaryToNewSecondaryPos;
		fixedPos.y = secondaryRigid.GetPos().y;
		secondaryRigid.SetNextPos(fixedPos);
	}
}

/// <summary>
/// ��ނ��ƂɏՓ˒ʒm���΂��z��ɒǉ�����
/// </summary>
/// <param name="preSendInfo">�Փ˂����I�u�W�F�N�g�̃��X�g�̃��O</param>
/// <param name="newSendInfo">�Փ˂����I�u�W�F�N�g�̃��X�g</param>
/// <param name="isTrigger">�������邩�ǂ���</param>
void MyLib::Physics::CheckSendOnCollideInfo(SendCollideInfo& preSendInfo, SendCollideInfo& newSendInfo, bool isTrigger)
{
	for (auto& parent : newSendInfo)
	{
		// �ȑO�̏��ɐe�Ƃ��ēo�^����Ă��邩
		bool isPreParent = preSendInfo.find(parent.first) != preSendInfo.end();
		bool isAllEnter = true;

		for (auto& child : parent.second)
		{
			bool isPreChild = false;
			if (isPreParent)
			{
				// �ȑO�̏��Ɏq�Ƃ��ēo�^����Ă��邩
				auto& preParent = preSendInfo[parent.first];
				isPreChild = std::find(preParent.begin(), preParent.end(), child) != preParent.end();
			}

			// ��������Ă����ꍇ��Enter���Ă�(�q�Ƃ��ēo�^����Ă��Ȃ�)
			if (!isPreChild)
			{
				if (isTrigger)
				{
					AddOnCollideInfo(parent.first, child, eOnCollideInfoKind::TriggerEnter);
					AddOnCollideInfo(child, parent.first, eOnCollideInfoKind::TriggerEnter);
				}
				else
				{
					AddOnCollideInfo(parent.first, child, eOnCollideInfoKind::CollideEnter);
					AddOnCollideInfo(child, parent.first, eOnCollideInfoKind::CollideEnter);
				}
			}

			// Stary�͖��x�Ă�
			if (isTrigger)
			{
				AddOnCollideInfo(parent.first, child, eOnCollideInfoKind::TriggerStay);
				AddOnCollideInfo(child, parent.first, eOnCollideInfoKind::TriggerStay);
			}
			else
			{
				AddOnCollideInfo(parent.first, child, eOnCollideInfoKind::CollideStay);
				AddOnCollideInfo(child, parent.first, eOnCollideInfoKind::CollideStay);
			}

			// �o�^����Ă��������폜
			if (isPreChild)
			{
				preSendInfo[parent.first].remove(child);
			}
			// �S�ēo�^����Ă��Ȃ������Ƃ���
			else
			{
				isAllEnter = false;
			}
		}

		// �S�ēo�^����Ă�����e�̏�������
		if (isAllEnter)
		{
			preSendInfo.erase(parent.first);
		}
	}

	// �c���Ă���O��񂩂�Exist���Ă�(�o�^���c���Ă��遁���񔲂���)
	for (auto& parent : preSendInfo)
	{
		for (auto& child : parent.second)
		{
			if (isTrigger)
			{
				AddOnCollideInfo(parent.first, child, eOnCollideInfoKind::TriggerExit);
				AddOnCollideInfo(child, parent.first, eOnCollideInfoKind::TriggerExit);
			}
			else
			{
				AddOnCollideInfo(parent.first, child, eOnCollideInfoKind::CollideExit);
				AddOnCollideInfo(child, parent.first, eOnCollideInfoKind::CollideExit);
			}
		}
	}
}

/// <summary>
/// �Փ˒ʒm���΂��z��ɒǉ�����
/// </summary>
/// <param name="own">���g</param>
/// <param name="send">�Փ˂�������</param>
/// <param name="kind">�����蔻��̎��</param>
void MyLib::Physics::AddOnCollideInfo(const std::shared_ptr<Collidable>& own, const std::shared_ptr<Collidable>& send, eOnCollideInfoKind kind)
{
	OnCollideInfoData addInfo;
	addInfo.own = own;
	addInfo.send = send;
	addInfo.kind = kind;
	m_onCollideInfo.emplace_back(addInfo);
}

/// <summary>
/// �ŏI�I�Ȉʒu�����肷��
/// </summary>
void MyLib::Physics::FixPosition()
{
	for (auto& item : m_collidables)
	{
#if _DEBUG
		//// �␳��̈ʒu���f�o�b�O�\��
		//DebugDraw::DrawLine(item->rigidbody.GetPos(), item->nextPos, AfterFixInfoColor);

		//if (item->colliderData->GetKind() == ColliderData::Kind::Circle2D)
		//{
		//	auto itemCircleData = dynamic_cast<ColliderDataCircle2D*>(item->colliderData);
		//	DebugDraw::DrawCircle(item->nextPos, itemCircleData->radius, AfterFixInfoColor);
		//}
#endif
		// Pos���X�V����̂ŁAvelocity�������Ɉړ�����velocity�ɏC��
		Vec3 toFixedPos = item->rigidbody.GetNextPos() - item->rigidbody.GetPos();
		item->rigidbody.SetVelocity(toFixedPos);

		// �ʒu�m��
		item->rigidbody.SetPos(item->rigidbody.GetNextPos());
	}
}


//MyLib::Vec3 MyLib::Physics::GetClosestPtOnSegment(Vec3 pt, Vec3 start, Vec3 end)
//{
//	// �ŋߐړ_��start�������O�̈�̏ꍇ
//	auto startToPt = pt - start;
//	auto startToEnd = end - start;
//	auto startToEndN = startToEnd.Normalize();
//	if (startToPt.Dot(startToEndN) < 0)
//	{
//		return start;
//	}
//	auto endToPt = pt - end;
//	auto endToStart = start - end;
//	auto endToStartN = endToStart.Normalize();
//	// �ŋߐړ_��end�������O�̈�̏ꍇ
//	if (endToPt.Dot(endToStartN) < 0)
//	{
//		return end;
//	}
//	// ���ԗ̈�̏ꍇ
//	else
//	{
//		float t = startToEndN.Dot(startToPt);
//		return start + startToEndN * t;
//	}
//}

/// <summary>
/// �`�F�b�N�����|���S�����ǃ|���S�������|���S�����𔻒f���ۑ�����
/// </summary>
/// <param name="col">�`�F�b�N����I�u�W�F�N�g</param>
void MyLib::Physics::CheckWallAndFloor(std::shared_ptr<Collidable>& col)
{
	// �ǃ|���S���Ə��|���S���̐�������������
	m_wallNum = 0;
	m_floorNum = 0;

	// ���o���ꂽ�|���S���̐������J��Ԃ�
	for (int i = 0; i < m_hitDim.HitNum; i++)
	{
		// �|���S���̖@���̂x�������ǃ|���S���{�[�_�[�ɒB�����Ă��邩�ǂ����ŕǃ|���S�������|���S�����𔻒f����
		if (m_hitDim.Dim[i].Normal.y < kWallPolyBorder && m_hitDim.Dim[i].Normal.y > -kWallPolyBorder)
		{
			// �ǃ|���S���Ɣ��f���ꂽ�ꍇ�ł��A�v���C���[�̂x���W��荂���|���S���̂ݓ����蔻����s��
			if (m_hitDim.Dim[i].Position[0].y > col->rigidbody.GetPos().y + kWallPolyHeight ||
				m_hitDim.Dim[i].Position[1].y > col->rigidbody.GetPos().y + kWallPolyHeight ||
				m_hitDim.Dim[i].Position[2].y > col->rigidbody.GetPos().y + kWallPolyHeight)
			{
				// �|���S���̐������E���ɒB���Ă��Ȃ�������|���S����z��ɒǉ�
				if (m_wallNum < ColInfo::kMaxColHitPolyNum)
				{
					// �|���S���̍\���̂̃A�h���X��ǃ|���S���|�C���^�z��ɕۑ�����
					m_pWallPoly[m_wallNum] = &m_hitDim.Dim[i];

					// �ǃ|���S���̐������Z����
					m_wallNum++;
				}
			}
		}
		else
		{
			// �|���S���̐������E���ɒB���Ă��Ȃ�������|���S����z��ɒǉ�
			if (m_floorNum < ColInfo::kMaxColHitPolyNum)
			{
				// �|���S���̍\���̂̃A�h���X�����|���S���|�C���^�z��ɕۑ�����
				m_pFloorPoly[m_floorNum] = &m_hitDim.Dim[i];

				// ���|���S���̐������Z����
				m_floorNum++;
			}
		}
	}
}

/// <summary>
/// �ǃ|���S���Ƃ̓����蔻����`�F�b�N���A�ړ�������
/// </summary>
/// <param name="col">�`�F�b�N����I�u�W�F�N�g</param>
void MyLib::Physics::FixPositionWithWall(std::shared_ptr<Collidable>& col)
{
	float radius = 0.0f;
	for (auto& col : col->m_colliders)
	{
		radius = dynamic_cast<MyLib::ColliderDataSphere*> (col.get())->m_radius;
	}

	// �ǃ|���S�����Ȃ��ꍇ�͉������Ȃ�
	if (m_wallNum == 0) return;

	// �ǃ|���S���Ƃ̓����蔻�菈��
	// �ǂɓ����������ǂ����̃t���O�͏�����Ԃł́u�������Ă��Ȃ��v�ɂ��Ă���
	m_isHitFlag = false;

	// �ړ��������ǂ����ŏ����𕪊�
	if (col->rigidbody.GetDir().Size() != 0.0f)
	{
		// �ǃ|���S���̐������J��Ԃ�
		for (int i = 0; i < m_wallNum; i++)
		{
			// i�Ԗڂ̕ǃ|���S���̃A�h���X��ǃ|���S���|�C���^�z�񂩂�擾
			m_pPoly = m_pWallPoly[i];

			// �|���S���ƃv���C���[���������Ă��Ȃ������玟�̃J�E���g��
			if (!HitCheck_Sphere_Triangle(col->rigidbody.GetNextPosVECTOR(), radius,
				m_pPoly->Position[0], m_pPoly->Position[1], m_pPoly->Position[2])) continue;

			// �����ɂ�����|���S���ƃv���C���[���������Ă���Ƃ������ƂȂ̂ŁA�|���S���ɓ��������t���O�𗧂Ă�
			m_isHitFlag = true;

			//�ǂ��l�������ړ����O�ς��g���ĎZ�o
			MyLib::Vec3 SlideVec;

			VECTOR ret;
			ret = VCross(col->rigidbody.GetVelocityVECTOR(), m_pPoly->Normal);
			// �i�s�����x�N�g���ƕǃ|���S���̖@���x�N�g���ɐ����ȃx�N�g�����Z�o
			SlideVec = MyLib::Vec3(ret.x, ret.y, ret.z);

			// �Z�o�����x�N�g���ƕǃ|���S���̖@���x�N�g���ɐ����ȃx�N�g�����Z�o�A���ꂪ
			// ���̈ړ���������Ǖ����̈ړ������𔲂����x�N�g��
			ret = VCross(m_pPoly->Normal, SlideVec.ConvertToVECTOR());
			SlideVec = MyLib::Vec3(ret.x, ret.y, ret.z);

			// ������ړ��O�̍��W�ɑ��������̂�V���ȍ��W�Ƃ���
			col->rigidbody.SetNextPos(col->rigidbody.GetPos() + SlideVec);

			//�ύX�������ߐV���Ɏ擾
			//tempNextPos = col->rigidbody.GetNextPos();
			//nextPos = tempNextPos.ConvertToVECTOR();

			// �V���Ȉړ����W�ŕǃ|���S���Ɠ������Ă��Ȃ����ǂ����𔻒肷��
			bool isHitWallPolygon = false;
			for (int j = 0; j < m_wallNum; j++)
			{
				// j�Ԗڂ̕ǃ|���S���̃A�h���X��ǃ|���S���|�C���^�z�񂩂�擾
				m_pPoly = m_pWallPoly[j];

				// �������Ă����烋�[�v���甲����
				if (HitCheck_Sphere_Triangle(col->rigidbody.GetNextPosVECTOR(), radius,
					m_pPoly->Position[0], m_pPoly->Position[1], m_pPoly->Position[2]))
				{
					//true�ɂ���
					isHitWallPolygon = true;
					break;
				}
			}

			// �S�Ẵ|���S���Ɠ������Ă��Ȃ������炱���Ń��[�v�I��
			if (!isHitWallPolygon)
			{
				//�q�b�g�t���O��|��
				m_isHitFlag = false;
				break;
			}
		}
	}
	else
	{
		// �ړ����Ă��Ȃ��ꍇ�̏���

		// �ǃ|���S���̐������J��Ԃ�
		for (int i = 0; i < m_wallNum; i++)
		{
			// i�Ԗڂ̕ǃ|���S���̃A�h���X��ǃ|���S���|�C���^�z�񂩂�擾
			m_pPoly = m_pWallPoly[i];

			// �|���S���ɓ������Ă����瓖�������t���O�𗧂Ă���Ń��[�v���甲����
			if (HitCheck_Sphere_Triangle(col->rigidbody.GetNextPosVECTOR(), radius,
				m_pPoly->Position[0], m_pPoly->Position[1], m_pPoly->Position[2]))
			{
				m_isHitFlag = true;
				break;
			}
		}
	}

	// �ǂɓ������Ă�����ǂ��牟���o���������s��
	if (m_isHitFlag)
	{
		FixPositionWithWallInternal(col);
	}
}

/// <summary>
/// �ǂ̒����牟���o��
/// </summary>
/// <param name="col">�`�F�b�N����I�u�W�F�N�g</param>
void MyLib::Physics::FixPositionWithWallInternal(std::shared_ptr<Collidable>& col)
{
	float radius = 0.0f;
	for (auto& col : col->m_colliders)
	{
		radius = dynamic_cast<MyLib::ColliderDataSphere*> (col.get())->m_radius;
	}

	// �ǂ���̉����o�����������݂�ő吔�����J��Ԃ�
	for (int i = 0; i < ColInfo::kMaxColHitTryNum; i++)
	{
		// ������\���̂���ǃ|���S����S�Č���
		bool isHitWall = false;
		// �ǃ|���S���̐������J��Ԃ�
		for (int j = 0; j < m_wallNum; j++)
		{
			// i�Ԗڂ̕ǃ|���S���̃A�h���X��ǃ|���S���|�C���^�z�񂩂�擾
			m_pPoly = m_pWallPoly[j];

			// �|���S���ƃv���C���[���������Ă��Ȃ������玟�̃J�E���g��
			if (!HitCheck_Sphere_Triangle(col->rigidbody.GetNextPosVECTOR(), radius,
				m_pPoly->Position[0], m_pPoly->Position[1], m_pPoly->Position[2])) continue;

			auto ret = VAdd(col->rigidbody.GetNextPosVECTOR(), VScale(m_pPoly->Normal, kColHitSlideLength));

			MyLib::Vec3 set;
			set = MyLib::Vec3(ret.x, ret.y, ret.z);

			// �������Ă�����K�苗�����v���C���[��ǂ̖@�������Ɉړ�������
			col->rigidbody.SetNextPos(set);

			// �ړ�������ŕǃ|���S���ƐڐG���Ă��邩�ǂ����𔻒�
			for (int k = 0; k < m_wallNum; k++)
			{
				// �������Ă����烋�[�v�𔲂���
				m_pPoly = m_pWallPoly[k];
				if (HitCheck_Sphere_Triangle(col->rigidbody.GetNextPosVECTOR(), radius,
					m_pPoly->Position[0], m_pPoly->Position[1], m_pPoly->Position[2]))
				{
					isHitWall = true;
					break;
				}
			}

			// �S�Ẵ|���S���Ɠ������Ă��Ȃ������炱���Ń��[�v�I��
			if (!isHitWall) break;
		}

		//���[�v�I��
		if (!isHitWall) break;
	}
}

/// <summary>
/// ���|���S���Ƃ̓����蔻����`�F�b�N���A�ړ�������
/// </summary>
/// <param name="col">�`�F�b�N����I�u�W�F�N�g</param>
void MyLib::Physics::FixNowPositionWithFloor(std::shared_ptr<Collidable>& col)
{
	//���|���S�����Ȃ��ꍇ�͉������Ȃ�
	if (m_floorNum == 0) return;

	float radius = 0.0f;
	for (auto& col : col->m_colliders)
	{
		radius = dynamic_cast<MyLib::ColliderDataSphere*> (col.get())->m_radius;
	}

	// ���|���S���Ƃ̓����蔻�菈��
	//�����������ǂ����̃t���O������
	bool IsHitFlag = false;

	////�W�����v�����㏸���̏ꍇ
	//if (dynamic_cast<CharacterBase*>(this)->GetJumpState() && dynamic_cast<CharacterBase*>(this)->GetJumpPower() >= 0.0f)
	//{
	//	// �V��ɓ����Ԃ��鏈�����s��
	//	// ��ԒႢ�V��ɂԂ���ׂ̔���p�ϐ���������
	//	float PolyMinPosY = 0.0f;

	//	// ���|���S���̐������J��Ԃ�
	//	for (int i = 0; i < m_floorNum; i++)
	//	{
	//		// i�Ԗڂ̏��|���S���̃A�h���X�����|���S���|�C���^�z�񂩂�擾
	//		m_pPoly = m_pFloorPoly[i];

	//		// ���悩�瓪�̍����܂ł̊ԂŃ|���S���ƐڐG���Ă��邩�ǂ����𔻒�
	//		m_lineRes = HitCheck_Line_Triangle(m_nextPos, VAdd(m_nextPos, VGet(0.0f, kHeadHeight, 0.0f)),
	//			m_pPoly->Position[0], m_pPoly->Position[1], m_pPoly->Position[2]);

	//		// �ڐG���Ă��Ȃ������牽�����Ȃ�
	//		if (!m_lineRes.HitFlag) continue;

	//		// �V��|���S�������܂Ō��o���ꂽ�|���S�����Ⴂ�ꍇ������ʂ�
	//		if (PolyMinPosY < m_lineRes.Position.y)
	//		{
	//			// �|���S���ɓ��������t���O�𗧂Ă�
	//			IsHitFlag = true;

	//			// �ڐG�����x���W��ۑ�����
	//			PolyMinPosY = m_lineRes.Position.y;
	//		}
	//	}

	//	// �ڐG�����|���S���������
	//	if (IsHitFlag)
	//	{
	//		// �ڐG�����ꍇ�̓v���C���[�̂x���W��ڐG���W�����ɍX�V
	//		m_nextPos.y = PolyMinPosY - kHeadHeight;

	//		//�n�ʂɓ����������̏������s��
	//		dynamic_cast<CharacterBase*>(this)->HitGroundUpdate();
	//	}
	//}

	// ���|���S���Ƃ̓����蔻��
	//��ԍ������|���S���ɂԂ���ׂ̔���p�ϐ���������
	float PolyMaxPosY = 0.0f;

	// ���|���S���ɓ����������ǂ����̃t���O��|���Ă���
	m_isHitFlag = false;

	// ���|���S���̐������J��Ԃ�
	for (int i = 0; i < m_floorNum; i++)
	{
		// i�Ԗڂ̏��|���S���̃A�h���X�����|���S���|�C���^�z�񂩂�擾
		m_pPoly = m_pFloorPoly[i];

		// �|���S���ƃv���C���[���������Ă��Ȃ������玟�̃J�E���g��
		if (!HitCheck_Sphere_Triangle(col->rigidbody.GetNextPosVECTOR(), radius,
			m_pPoly->Position[0], m_pPoly->Position[1], m_pPoly->Position[2])) continue;

		float mostHeightY = m_pPoly->Position[0].y;

		if (mostHeightY < m_pPoly->Position[1].y)
		{
			mostHeightY = m_pPoly->Position[1].y;
		}

		if (mostHeightY < m_pPoly->Position[2].y)
		{
			mostHeightY = m_pPoly->Position[2].y;
		}

		// ���ɓ��������|���S��������A�����܂Ō��o�������|���S�����Ⴂ�ꍇ�͉������Ȃ�
		if (m_isHitFlag && PolyMaxPosY > mostHeightY) continue;

		// �|���S���ɓ��������t���O�𗧂Ă�
		m_isHitFlag = true;

		// �ڐG�����x���W��ۑ�����
		PolyMaxPosY = mostHeightY;
	}

	// ���|���S���̓����蔻�肩�A�W�����v�͂�0����������(���~���̏ꍇ)�ǂ����ŏ����𕪊�
	if (m_isHitFlag)
	{
		// �ڐG�����|���S���ň�ԍ����x���W���v���C���[�̂x���W�ɂ���
		auto set = col->rigidbody.GetNextPos();
		set.y = PolyMaxPosY + radius;
		col->rigidbody.SetNextPos(set);

		//m_nextPos.y = PolyMaxPosY;
		//dynamic_cast<CharacterBase*>(this)->HitGroundUpdate();

		////�W�����v�͂�0����������(���~��)���A�W�����v���ł������ꍇ
		////�W�����v�������I������
		//if (dynamic_cast<CharacterBase*>(this)->GetJumpPower() <= 0.0f &&
		//	dynamic_cast<CharacterBase*>(this)->GetJumpState())
		//{
		//	dynamic_cast<CharacterBase*>(this)->EndJumpState();
		//}
	}

}