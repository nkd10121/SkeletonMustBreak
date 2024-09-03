#include "EnemyBig.h"
#include "CsvLoad.h"
#include "WeaponBase.h"

namespace
{

	//���f���T�C�Y
	constexpr float kModelSize = 6.8f;
	constexpr float kModelOffsetY = 0.8f;

	constexpr float kCollisionRadius = 6.0f;

	constexpr float kWeaponModelSize = 0.01f;

	//�A�j���[�V�����̐؂�ւ��ɂ�����t���[����
	constexpr float kAnimChangeFrame = 10.0f;
	constexpr float kAnimChangeRateSpeed = 1.0f / kAnimChangeFrame;

	//�A�j���[�V�����u�����h���̍ő�
	constexpr float kAnimBlendRateMax = 1.0f;

	constexpr float kSearchingRange = 30.0f;
}

EnemyBig::EnemyBig(int modelH, int weponH):
	EnemyBase(Collidable::Priority::Middle),
	m_nowAnimIdx(eAnimIdx::Run),
	m_weponHandle(-1),
	m_tempFrameCount(0),
	m_weponAttachFrameNum(-1),
	m_attackWaitFrame(0),
	m_knockCount(0),
	m_currentAnimNo(-1),
	m_prevAnimNo(-1),
	m_animBlendRate(1.0f),
	m_animSpeed(0.5f),
	m_isAnimationFinish(false),
	m_isChasing(false)
{
	//�����蔻��̐ݒ�
	auto collider = Collidable::AddCollider(MyLib::ColliderData::Kind::Sphere, false);
	auto sphereCol = dynamic_cast<MyLib::ColliderDataSphere*>(collider.get());
	sphereCol->m_radius = kCollisionRadius;

	CsvLoad::GetInstance().StatusLoad(m_status, "BigSkelton");

	m_weapon = std::make_shared<WeaponBase>();
	m_weapon->Init(weponH, modelH, "handslot.r", kWeaponModelSize);

	//���f���̓ǂݍ���
	m_modelHandle = modelH;
	//m_weponHandle = weponH;

	m_dropPoint = m_status.point;
}

EnemyBig::~EnemyBig()
{
	//�������̉��
	MV1DeleteModel(m_modelHandle);
}

void EnemyBig::Init(std::shared_ptr<MyLib::Physics> physics, std::vector<MyLib::Vec3> route)
{
	m_pPhisics = physics;

	m_route = route;

	for (int i = 0; i < m_route.size(); i++)
	{
		m_route[i].y += kModelOffsetY * kModelSize;
	}


	//�ŏ��̖ړI�n��ݒ肷��
	m_routeNum = 1;
	m_destinationPos = m_route[m_routeNum];

	//���݂��Ă����Ԃɂ���
	m_isExist = true;

	Collidable::Init(m_pPhisics);

	rigidbody.Init(true);
	rigidbody.SetPos(m_route[0]);
	rigidbody.SetNextPos(rigidbody.GetPos());
	m_moveVec = (m_destinationPos - rigidbody.GetPos()).Normalize();

	m_collisionPos = rigidbody.GetPos();
	SetModelPos();
	MV1SetPosition(m_modelHandle, m_modelPos.ConvertToVECTOR());


	//���f���̃T�C�Y�ݒ�
	MV1SetScale(m_modelHandle, VGet(kModelSize, kModelSize, kModelSize));
	//m_weponAttachFrameNum = MV1SearchFrame(m_modelHandle, "handslot.r");


	//�A�j���[�V������ݒ�
	m_currentAnimNo = MV1AttachAnim(m_modelHandle, eAnimIdx::Run);


	//�ʏ��Ԃɐݒ肵�Ă���
	m_updateFunc = &EnemyBig::WalkUpdate;
}

void EnemyBig::Finalize(std::shared_ptr<MyLib::Physics> physics)
{
	Collidable::Finalize(physics);
}

void EnemyBig::Update(MyLib::Vec3 playerPos, bool isChase)
{
	//���݂��Ă��Ȃ���ԂȂ牽�������Ȃ�
	if (!m_isExist)return;

	//�A�j���[�V�����̍X�V
	m_isAnimationFinish = UpdateAnim(m_currentAnimNo);

	//��Ԃ̍X�V
	(this->*m_updateFunc)(playerPos, isChase);

	//HP���ȉ��ɂȂ����玀�S����
	if (m_status.hp <= 0)
	{
		if (!m_isDead)
		{
			m_isDead = true;
			Finalize(m_pPhisics);

			m_updateFunc = &EnemyBig::DeathUpdate;

			auto deathAnimIdx = GetRand(2);
			auto deathAnimSpeed = 0.66f;

			switch (deathAnimIdx)
			{
			case 0:
				m_nowAnimIdx = eAnimIdx::DeathA;
				deathAnimSpeed = 0.33f;
				break;
			case 1:
				m_nowAnimIdx = eAnimIdx::DeathB;
				break;
			case 2:
				m_nowAnimIdx = eAnimIdx::DeathC;
				break;
			}



			ChangeAnim(m_nowAnimIdx, deathAnimSpeed);
		}
	}


	//�A�j���[�V�����̐؂�ւ�
	if (m_prevAnimNo != -1)
	{
		//�t���[���ŃA�j���[�V������؂�ւ���
		m_animBlendRate += kAnimChangeRateSpeed;
		if (m_animBlendRate >= kAnimBlendRateMax)
		{
			m_animBlendRate = kAnimBlendRateMax;
		}

		//�A�j���[�V�����̃u�����h����ݒ肷��
		MV1SetAttachAnimBlendRate(m_modelHandle, m_prevAnimNo, kAnimBlendRateMax - m_animBlendRate);
		MV1SetAttachAnimBlendRate(m_modelHandle, m_currentAnimNo, m_animBlendRate);
	}


	m_weapon->Update((m_collisionPos + m_moveVec * 4));
}

void EnemyBig::Draw()
{
	//���݂��Ă��Ȃ���ԂȂ牽�������Ȃ�
	if (!m_isExist)return;

	rigidbody.SetPos(rigidbody.GetNextPos());
	m_collisionPos = rigidbody.GetPos();
	SetModelPos();
	MV1SetPosition(m_modelHandle, m_modelPos.ConvertToVECTOR());
	//���f���̕`��
	MV1DrawModel(m_modelHandle);
	m_weapon->Draw();

#ifdef _DEBUG
	DrawFormatString(800, 60, 0xffffff, "%f,%f,%f", m_modelPos.x, m_modelPos.y, m_modelPos.z);
	DrawFormatString(800, 76, 0xffffff, "%f,%f,%f", m_collisionPos.x, m_collisionPos.y, m_collisionPos.z);
#endif
}

void EnemyBig::SetModelPos()
{
	m_modelPos = m_collisionPos;
	m_modelPos.y -= kModelOffsetY * kModelSize;
}

bool EnemyBig::UpdateAnim(int attachNo, float startTime)
{
	//�A�j���[�V�������ݒ肳��Ă��Ȃ������瑁�����^�[��
	if (attachNo == -1)	return false;

	//�A�j���[�V������i�s������
	float nowFrame = MV1GetAttachAnimTime(m_modelHandle, attachNo);	//���݂̍Đ��J�E���g���擾
	nowFrame += m_animSpeed;

	//���ݍĐ����̃A�j���[�V�����̑��J�E���g���擾����
	float totalAnimframe = MV1GetAttachAnimTotalTime(m_modelHandle, attachNo);
	bool isLoop = false;

	//NOTE:�����������瑍�t���[���������Ă����t���[�����傫����������Ȃ�����while�ő傫���Ԉ���������
	while (totalAnimframe <= nowFrame)
	{
		//NOTE:nowFrame��0�Ƀ��Z�b�g����ƃA�j���[�V�����t���[���̔�т��ł邩�瑍�t���[��������
		nowFrame -= totalAnimframe;
		nowFrame += startTime;
		isLoop = true;
	}

	//�i�߂����Ԃɐݒ�
	MV1SetAttachAnimTime(m_modelHandle, attachNo, nowFrame);

	return isLoop;
}

void EnemyBig::ChangeAnim(int animIndex, float animSpeed)
{
	//����ɌÂ��A�j���[�V�������A�^�b�`����Ă���ꍇ�͂��̎��_�ŏ����Ă���
	if (m_prevAnimNo != -1)
	{
		MV1DetachAnim(m_modelHandle, m_prevAnimNo);
	}

	//���ݍĐ����̑ҋ@�A�j���[�V�����͕ύX�ڂ̃A�j���[�V�����̈����ɂ���
	m_prevAnimNo = m_currentAnimNo;

	//�ύX��̃A�j���[�V�����Ƃ��čU���A�j���[�V���������߂Đݒ肷��
	m_currentAnimNo = MV1AttachAnim(m_modelHandle, animIndex);

	//�؂�ւ��̏u�Ԃ͕ύX�O�̃A�j���[�V�������Đ�������Ԃɂ���
	m_animBlendRate = 0.0f;

	m_animSpeed = animSpeed;

	//�ύX�O�̃A�j���[�V����100%
	MV1SetAttachAnimBlendRate(m_modelHandle, m_prevAnimNo, 1.0f - m_animBlendRate);
	//�ύX��̃A�j���[�V����0%
	MV1SetAttachAnimBlendRate(m_modelHandle, m_currentAnimNo, m_animBlendRate);
}

void EnemyBig::AttackUpdate(MyLib::Vec3 playerPos, bool isChase)
{
	rigidbody.SetVelocity(MyLib::Vec3());
	////�A�j���[�V�����̍X�V
	//m_isAnimationFinish = UpdateAnim(m_currentAnimNo);

	if (m_attackWaitFrame >= 20)
	{
		if (m_attackWaitFrame == 20)
		{
			m_weapon->CollisionInit(m_pPhisics);

			m_nowAnimIdx = eAnimIdx::Attack;
			ChangeAnim(m_nowAnimIdx, 0.3f);
		}


		//�A�j���[�V�������I�������������Ԃɖ߂�
		if (m_isAnimationFinish)
		{
			m_attackWaitFrame = 0;

			m_isAttack = false;

			m_weapon->CollisionEnd(m_pPhisics);

			m_updateFunc = &EnemyBig::WalkUpdate;
			ChangeAnim(eAnimIdx::Run);
		}

	}

	m_attackWaitFrame++;
}

void EnemyBig::WalkUpdate(MyLib::Vec3 playerPos, bool isChase)
{
	//�U���t���O��true�ɂȂ��Ă�����U��������
	if (m_isAttack)
	{
		m_moveVec = (playerPos - m_collisionPos).Normalize();

		m_updateFunc = &EnemyBig::AttackUpdate;

		m_nowAnimIdx = eAnimIdx::Idle;
		ChangeAnim(m_nowAnimIdx);
		return;
	}

	if (m_isKnock)
	{
		if (m_knockCount == 0)
		{
			m_updateFunc = &EnemyBig::HitUpdate;
			m_nowAnimIdx = eAnimIdx::Hit;
			ChangeAnim(m_nowAnimIdx);
		}

		m_knockCount++;

		if (m_knockCount > 90)
		{
			m_isKnock = false;
			m_knockCount = 0;
		}

	}

	auto temp = (rigidbody.GetPos() - m_route[m_routeNum]).Size();
	if (temp < 1.2f)
	{
		m_routeNum++;
		if (m_routeNum == m_route.size())
		{
			Finalize(m_pPhisics);
			m_isExist = false;
			m_isReach = true;
		}
		else
		{
			m_destinationPos = m_route[m_routeNum];
			//m_moveVec = (m_destinationPos - rigidbody.GetPos()).Normalize();
		}
	}

	//�o�O���������߂ɂ������񂱂��ŗl�q��
	m_moveVec = (m_destinationPos - rigidbody.GetPos()).Normalize();

	if (!isChase)
	{
		bool isChesed = m_isChasing;
		m_isChasing = false;

		playerPos.y = m_collisionPos.y;
		auto len = playerPos - m_collisionPos;
		auto dis = len.Size();

		//���͈͂��߂���ΒǐՂ���
		if (dis < kSearchingRange)
		{
			m_isChasing = true;
			m_moveVec = len.Normalize();
		}
		//
		else
		{
			//�ǐՂ��I�����Ƃ�
			if (isChesed != m_isChasing)
			{
				//���[�g�����āA��ԋ߂����W�Ɍ�����
				float distanceMin = 1000.0f;
				int retRouteNum = 0;

				for (int i = 0; i < m_route.size(); i++)
				{
					//���ݍ��W����̋���������
					auto dis = (m_route[i] - rigidbody.GetPos()).Size();

					if (distanceMin > dis)
					{
						distanceMin = dis;
						retRouteNum = i;
					}
				}

				if (retRouteNum != m_routeNum - 1)
				{
					m_routeNum = retRouteNum;

				}

				//
				m_destinationPos = m_route[m_routeNum];
				m_moveVec = (m_destinationPos - rigidbody.GetPos()).Normalize();
			}
		}
	}


	//�ړ����x�������Őݒ�ł�����ۂ��H
	rigidbody.SetVelocity(m_moveVec, m_status.speed);

	//atan2���g�p���Č����Ă���p�x���擾
	auto angle = atan2(m_moveVec.x, m_moveVec.z);
	auto rotation = VGet(0.0f, angle + DX_PI_F, 0.0f);
	//�ړ������ɑ̂���]������
	MV1SetRotationXYZ(m_modelHandle, rotation);
}

void EnemyBig::HitUpdate(MyLib::Vec3 playerPos, bool isChase)
{
	rigidbody.SetVelocity(MyLib::Vec3());
	//�A�j���[�V�����̍X�V
	m_isAnimationFinish = UpdateAnim(m_currentAnimNo);

	//�A�j���[�V�������I�������������Ԃɖ߂�
	if (m_isAnimationFinish)
	{

		m_updateFunc = &EnemyBig::WalkUpdate;
		ChangeAnim(eAnimIdx::Run);
	}
}

void EnemyBig::DeathUpdate(MyLib::Vec3 playerPos, bool isChase)
{
	if (m_isAnimationFinish)
	{
		m_isExist = false;
	}
}
