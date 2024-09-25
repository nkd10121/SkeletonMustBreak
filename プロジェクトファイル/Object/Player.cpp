#include "Player.h"
#include "Input.h"
#include "Shot.h"
#include "WeaponBase.h"
#include "HealPortion.h"
#include <cmath>
#include "CsvLoad.h"
#include "EffectManager.h"
#include "EffekseerForDXLib.h"

#include "SoundManager.h"

#ifdef _DEBUG
#define IsDebugDrawNowAnimName true
#else
#define IsDebugDrawNowAnimName FALSE
#endif
namespace
{
	//���f���T�C�Y
	constexpr float kModelSize = 4.0f;
	//���f���̒��S�Ɠ����蔻��̒��S�ɍ������������߁A���̋����������Ă���(ModelViewer����)
	constexpr float kModelOffsetY = 1.0f;

	//�A�j���[�V�����̐؂�ւ��ɂ�����t���[����
	constexpr float kAnimChangeFrame = 10.0f;
	constexpr float kAnimChangeRateSpeed = 1.0f / kAnimChangeFrame;

	//�A�j���[�V�����u�����h���̍ő�
	constexpr float kAnimBlendRateMax = 1.0f;

	//�_���A�j���[�V�����̊J�n�t���[����
	constexpr float kAimAnimStartFrame = 9.0f;

	/*�A�i���O�X�e�B�b�N�ɂ��ړ��֘A*/
	constexpr float kMaxSpeed = 0.2f;			//�v���C���[�̍ő呬�x
	constexpr float kAnalogRangeMin = 0.1f;		//�A�i���O�X�e�B�b�N�̓��͔���͈�
	constexpr float kAnalogRangeMax = 0.8f;
	constexpr float kAnalogInputMax = 1000.0f;	//�A�i���O�X�e�B�b�N������͂����x�N�g���̍ő�

	constexpr float kRunVecSize = 0.12f;
	constexpr int kShotIntervalFrame = 20;

	constexpr float kJumpPower = 0.58f;

}

Player::Player() :
	CharacterBase(Collidable::Priority::Low, GameObjectTag::Player),
	m_nowAnimIdx(eAnimIdx::Idle),
	m_moveCount(0),
	m_weponHandle(-1),
	m_angle(0.0f),
	m_isMove(true),
	m_isClear(false),
	m_equipAnimNo(-1),
	m_currentAnimNo(-1),
	m_prevAnimNo(-1),
	m_animBlendRate(1.0f),
	m_animSpeed(0.5f),
	m_isAnimationFinish(false),
	m_cameraDirection(),
	m_cameraAngle(0.0f),
	m_updateFunc(nullptr),
	shotTime(0),
	m_shotOffsetPower(0.0f),
	m_isDown(false),
	m_isDead(false),
	m_jumpCount(0),
	m_weponAttachFrameNum(-1),
	m_weponFrameMat(MGetIdent()),
	m_slotNum(0),
	m_slotNumMax(0)
{
	auto collider = Collidable::AddCollider(MyLib::ColliderData::Kind::Sphere, false);
	auto sphereCol = dynamic_cast<MyLib::ColliderDataSphere*>(collider.get());
	sphereCol->m_radius = 3.4f;

	//���f���ǂݍ���
	m_modelHandle = MV1LoadModel("data/model/player.mv1");
	m_weponHandle = MV1LoadModel("data/model/crossbow.mv1");
	//���f���̃T�C�Y�ݒ�
	MV1SetScale(m_modelHandle, VGet(kModelSize, kModelSize, kModelSize));

	EffectManager::GetInstance().Load("PlayerHit","data/effect/player_hit.efk",120);
	EffectManager::GetInstance().Load("Heal","data/effect/heal.efk",120);
}

Player::~Player()
{
	//�������̉��
	MV1DeleteModel(m_modelHandle);
	MV1DeleteModel(m_weponHandle);

	m_pShots.clear();
}

void Player::Init(std::shared_ptr<MyLib::Physics> physics, int* arrow)
{
	m_pPhysics = physics;

	m_arrowHandle = *arrow;

	Collidable::Init(m_pPhysics);

	CsvLoad::GetInstance().StatusLoad(m_status, "Player");
	m_hpMax = m_status.hp;


	//�v���C���[�̏����ʒu�ݒ�
	rigidbody.Init(true);
	rigidbody.SetPos(MyLib::Vec3(0.0f, 1.0f+kModelOffsetY * kModelSize, 0.0f));
	rigidbody.SetNextPos(rigidbody.GetPos());
	m_collisionPos = rigidbody.GetPos();
	SetModelPos();
	MV1SetPosition(m_modelHandle, m_modelPos.ConvertToVECTOR());
	MV1SetRotationXYZ(m_modelHandle, VGet(0.0f, 0.0f, 0.0f));

	//�_���A�j���[�V������ݒ�
	m_equipAnimNo = MV1AttachAnim(m_modelHandle, eAnimIdx::Aim);
	//�ҋ@�A�j���[�V������ݒ�
	m_currentAnimNo = MV1AttachAnim(m_modelHandle, eAnimIdx::Idle);

	//�ʏ��Ԃɐݒ肵�Ă���
	m_updateFunc = &Player::NeutralUpdate;

	// ���������̏�����
	//rigidbody.Init(true);
	//rigidbody.SetPos(MyLib::Vec3());
}

void Player::Finalize()
{
	Collidable::Finalize(m_pPhysics);
}

void Player::Update(std::shared_ptr<Input>& input)
{
	//�A�j���[�V�����̍X�V
	if (!m_isDead)
	{
		UpdateAnim(m_equipAnimNo, kAimAnimStartFrame);

	}
	m_isAnimationFinish = UpdateAnim(m_currentAnimNo);







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

	if (!m_isClear)
	{
		//��Ԃ̍X�V
		(this->*m_updateFunc)(input);
	}





	if (m_status.hp <= 0)
	{
		if (!m_isDown)
		{
			m_isDown = true;

			m_nowAnimIdx = eAnimIdx::Death;
			ChangeAnim(m_nowAnimIdx);

			Finalize();

			m_updateFunc = &Player::DeathUpdate;

		}
	}

	if (m_shotOffsetPower > 0.0f)
	{
		m_shotOffsetPower -= 0.001f;
	}

	if (m_shotOffsetPower <= 0.0f)
	{
		m_shotOffsetPower = 0.0f;
	}

	if (m_status.hp > 0)
	{
		if (input->IsTriggered("R"))
		{
			m_slotNum += 1;
			if (m_slotNum > m_slotNumMax)
			{
				m_slotNum = 0;
			}
		}

		if (input->IsTriggered("L"))
		{
			m_slotNum -= 1;
			if (m_slotNum < 0)
			{
				m_slotNum = m_slotNumMax;
			}
		}
	}

	//�����x��0�ɂ���
	//m_moveVec = MyLib::Vec3();
	//rigidbody.SetVelocity(MyLib::Vec3());

	//���W���擾����
	m_collisionPos = rigidbody.GetPos();

	std::pair<float, float> stick;

	if (!m_isDown && !m_isClear)
	{	//�R���g���[���[�̍��X�e�B�b�N���͂��󂯎��
		stick = input->GetLeftStick();
	}

	//�ړ�������ݒ肷��
	auto moveVec = MyLib::Vec3(stick.first, 0.0f, -stick.second);
	//�ړ��x�N�g���̒������擾����
	float len = moveVec.Size();

	//�ړ����ɑ�����SE��炷
	if (moveVec.Size() != 0.0f)
	{
		if (m_moveCount % 45 == 0)
		{
			SoundManager::GetInstance().PlaySE("walk");
		}
		m_moveCount++;

	}
	else
	{
		m_moveCount = 0;
	}

	//�x�N�g���̒�����0.0�`1.0�̊����ɕϊ�����
	float rate = len / kAnalogInputMax;

	//�A�i���O�X�e�B�b�N�����Ȕ͈͂����O����(�f�b�h�]�[��)
	rate = (rate - kAnalogRangeMin) / (kAnalogRangeMax - kAnalogRangeMin);
	rate = std::min(rate, 1.0f);
	rate = std::max(rate, 0.0f);

	//���x������ł���̂ňړ��x�N�g���ɔ��f����
	moveVec = moveVec.Normalize();
	float speed = m_status.speed * rate;

	moveVec = moveVec * speed;


	// �ړ����� =  �J�����̉E��  * �E�ւ̈ړ� +   �J�����̐���   * ���ʂւ̈ړ�
	// �@�@�@�@ = R(0.5, 0, 0.5) *    speed   +  F(-0.5, 0, 0.5) * speed

	//�v���C���[�̐��ʕ������v�Z���Đ��ʕ�������Ɉړ�����
	//�J�����̐��ʕ����x�N�g��
	MyLib::Vec3 front(m_cameraDirection.x, 0.0f, m_cameraDirection.z);
	//�����x�N�g��*�ړ���
	front = front * moveVec.z;
	//�J�����̉E�����x�N�g��
	MyLib::Vec3 right(-m_cameraDirection.z, 0.0f, m_cameraDirection.x);
	//�����x�N�g��*�ړ���
	right = right * (-moveVec.x);

	//�ړ��x�N�g���̐���
	m_moveVec = front + right;
	m_moveVec = m_moveVec.Normalize() * speed;
	//�ړ�����
	MV1SetPosition(m_modelHandle, m_collisionPos.ConvertToVECTOR());

	//�J�����̍��W����v���C���[����]������������v�Z����
	m_angle = -atan2f(m_cameraDirection.z, m_cameraDirection.x) - DX_PI_F / 2;
	m_rot = MyLib::Vec3(0.0f, m_angle, 0.0f);

	//�v���C���[����]������
	if (!m_isDown && !m_isClear)
	{
		MV1SetRotationXYZ(m_modelHandle, m_rot.ConvertToVECTOR());
	}
	//m_moveVec.y += kModelOffsetY * kModelSize;

	MyLib::Vec3 prevVelocity = rigidbody.GetVelocity();
	MyLib::Vec3 newVelocity = MyLib::Vec3(m_moveVec.x, prevVelocity.y, m_moveVec.z);
	rigidbody.SetVelocity(newVelocity);


	//�����Ă��镐��̈ړ��A��]���X�V����
	//TODO:�����납�炾�Ƃ悭�����Ȃ����߈�a���Ȃ����A����̉�]����������
	m_weponAttachFrameNum = MV1SearchFrame(m_modelHandle, "handslot.r");
	//auto moveMat = MGetTranslate(VGet(0.0f,-m_pos.y,0.0f));
	m_weponFrameMat = MV1GetFrameLocalWorldMatrix(m_modelHandle, m_weponAttachFrameNum);

	auto offsetMat = MGetTranslate(VGet(0.0f, -kModelOffsetY * kModelSize, 0.0f));

	m_weponFrameMat = MAdd(m_weponFrameMat, offsetMat);
	MATRIX temp = MMult(MGetRotY(-DX_PI_F / 2), m_weponFrameMat);
	MV1SetMatrix(m_weponHandle, temp);

	//if (input->IsPushed("ATTACK"))
	if (input->GetIsPushedZR() && m_slotNum == 0 && m_status.hp > 0)
	{
		if (shotTime % kShotIntervalFrame == 0)
		{
			SoundManager::GetInstance().PlaySE("shot");

			auto add = std::make_shared<Shot>();
			add->Init(m_pPhysics, MV1DuplicateModel(m_arrowHandle));
			MyLib::Vec3 offset = MyLib::Vec3(0.0f, kModelOffsetY * kModelSize / 2, 0.0f);

			auto shotVec = m_cameraDirection;
			int offsetX = GetRand(100) - 50;
			int offsetY = GetRand(100) - 50;
			int offsetZ = GetRand(100) - 50;
			MyLib::Vec3 offsetVec = MyLib::Vec3(static_cast<float>(offsetX), static_cast<float>(offsetY), static_cast<float>(offsetZ));
			offsetVec = offsetVec.Normalize() * m_shotOffsetPower;
			shotVec += offsetVec;

			add->Set(m_collisionPos + offset, shotVec, m_status.atk);
			m_pShots.emplace_back(add);

			m_shotOffsetPower += 0.028f;

			if (m_shotOffsetPower >= 0.1f)
			{
				m_shotOffsetPower = 0.1f;
			}
		}
		shotTime++;
	}
	else
	{
		shotTime = 0;
	}

	m_difAngle = static_cast<int>(m_shotOffsetPower * 100);
	if (m_difAngle < 2)
	{
		m_difAngle = 2;
	}

	for (auto& shot : m_pShots)
	{
		shot->Update();
		if (!shot->GetIsExist())
		{
			//���������������
			shot->Finalize(m_pPhysics);
			shot.reset();
			shot = nullptr;
		}
	}

	//�s�v�ɂȂ����G�������ō폜��������
	auto lIt = remove_if(m_pShots.begin(), m_pShots.end(), [](auto& v) {
		return v == nullptr;
		});
	m_pShots.erase(lIt, m_pShots.end());
}

void Player::Draw()
{
	//FIX:Draw�̂Ȃ��ō��W��ύX���Ă���̂͂ǂ��Ȃ́H
	rigidbody.SetPos(rigidbody.GetNextPos());
	m_collisionPos = rigidbody.GetPos();
	SetModelPos();
	MV1SetPosition(m_modelHandle, m_modelPos.ConvertToVECTOR());
	//���f���̕`��

	MV1DrawModel(m_modelHandle);
	MV1DrawModel(m_weponHandle);

	for (const auto& shot : m_pShots)
	{
		shot->Draw();
	}

	int rad = static_cast<int>(m_shotOffsetPower * 100);
	if (rad == 0)
	{
		rad = 2;
	}

#ifdef _DEBUG
	auto pos = rigidbody.GetPos();
	auto vel = rigidbody.GetVelocity();
	DrawFormatString(420, 112, 0xffffff, "%f", m_shotOffsetPower);
	DrawFormatString(0, 144, 0xffffff, "�v���C���[���W:%f,%f,%f", pos.x, pos.y, pos.z);
	DrawFormatString(0, 128, 0xffffff, "�ړ��x�N�g��:%f,%f,%f", vel.x, vel.y, vel.z);
	DrawFormatString(0, 160, 0xffffff, "�v���C���[�X�e�[�^�X:%d,%d,%d", m_status.hp, m_status.atk, m_status.def);
#endif
}

void Player::OnCollideEnter(const std::shared_ptr<Collidable>& colider)
{
#ifdef _DEBUG
	std::string message = "�v���C���[��";
#endif
	auto tag = colider->GetTag();
	switch (tag)
	{
	case GameObjectTag::Enemy:
#ifdef _DEBUG
		message += "�G";
#endif
		break;
	case GameObjectTag::Shot:
#ifdef _DEBUG
		message += "�e";
#endif
		break;
	}
#ifdef _DEBUG
	message += "�Ɠ��������I\n";
	printfDx(message.c_str());
#endif
}

void Player::OnTriggerEnter(const std::shared_ptr<Collidable>& colider)
{
#ifdef _DEBUG
	std::string message = "�v���C���[��";
#endif
	auto tag = colider->GetTag();
	switch (tag)
	{
	case GameObjectTag::Enemy:
#ifdef _DEBUG
		message += "�G";
#endif
		break;
	case GameObjectTag::Shot:
#ifdef _DEBUG
		message += "�e";
#endif
		break;
	case GameObjectTag::Sword:
	{//�Ȃ�{}���Ȃ��ƃG���[�f��
		//���ɂ���������HP�����炷
		WeaponBase* col = dynamic_cast<WeaponBase*>(colider.get());
		m_status.hp -= col->GetAtk() - m_status.def;

		//��_���[�W�G�t�F�N�g���o��
		auto pos = rigidbody.GetPos();
		//m_hitEffect->effects.push_back(CreateEffect(m_hitEffect->emitterHandle));
		MyLib::Vec3 offset = MyLib::Vec3(static_cast<float>(GetRand(4) - 2), static_cast<float>(GetRand(4) - 2), static_cast<float>(GetRand(4) - 2));
		pos += offset;

		EffectManager::GetInstance().CreateEffect("PlayerHit",pos);

		SoundManager::GetInstance().PlaySE("playerHit");
	}
	break;
	case GameObjectTag::Portion:

		if (m_hpMax > m_status.hp)
		{
			m_status.hp = m_hpMax;

			auto pos = rigidbody.GetPos();
			EffectManager::GetInstance().CreateEffect("Heal", pos);

			HealPortion* col = dynamic_cast<HealPortion*>(colider.get());
			col->End();

			SoundManager::GetInstance().PlaySE("heal");

		}
		break;


	}
#ifdef _DEBUG
	message += "�Ɠ��������I\n";
	printfDx(message.c_str());
#endif
}

/// <summary>
/// �v���C���[�𕜊�������
/// </summary>
void Player::PlayerRevival()
{
	CsvLoad::GetInstance().StatusLoad(m_status, "Player");

	//�v���C���[�̏����ʒu�ݒ�
	rigidbody.Init(true);
	rigidbody.SetPos(MyLib::Vec3(0.0f, kModelOffsetY * kModelSize, 0.0f));
	rigidbody.SetNextPos(rigidbody.GetPos());
	m_collisionPos = rigidbody.GetPos();
	SetModelPos();
	MV1SetPosition(m_modelHandle, m_modelPos.ConvertToVECTOR());
	MV1SetRotationXYZ(m_modelHandle, VGet(0.0f, 0.0f, 0.0f));

	//�_���A�j���[�V������ݒ�
	m_equipAnimNo = MV1AttachAnim(m_modelHandle, eAnimIdx::Aim);
	//�ҋ@�A�j���[�V������ݒ�
	m_currentAnimNo = MV1AttachAnim(m_modelHandle, eAnimIdx::Idle);

	//�ʏ��Ԃɐݒ肵�Ă���
	m_updateFunc = &Player::NeutralUpdate;
}

void Player::SetGameClear()
{
	if (!m_isClear)
	{
		m_isClear = true;
		m_nowAnimIdx = eAnimIdx::Cheer;
		ChangeAnim(m_nowAnimIdx);
	}

}

void Player::SetModelPos()
{
	m_modelPos = m_collisionPos;
	m_modelPos.y -= kModelOffsetY * kModelSize*0.8f;
}

bool Player::UpdateAnim(int attachNo, float startTime)
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

void Player::ChangeAnim(int animIndex, float animSpeed)
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

/// <summary>
/// �ʏ�(���͂Ȃ�)���
/// </summary>
void Player::NeutralUpdate(std::shared_ptr<Input> input)
{
#if IsDebugDrawNowAnimName
	DrawFormatString(0, 32, 0xffffff, "Neutral");
#endif
	m_isMove = true;

	//�X�e�B�b�N���͂��������������ԂɑJ�ڂ���
	if (m_moveVec.Size() != 0.0f)
	{
		m_updateFunc = &Player::WalkUpdate;
		m_nowAnimIdx = eAnimIdx::Walk;
		ChangeAnim(m_nowAnimIdx);

		return;
	}

	////�U���{�^������������U����ԂɑJ�ڂ���
	//if (input->IsTriggered("ATTACK"))
	//{
	//	m_updateFunc = &Player::AttackUpdate;
	//	m_nowAnimIdx = eAnimIdx::shoot;
	//	ChangeAnim(m_nowAnimIdx);

	//	return;
	//}

	//�W�����v�{�^������������ʏ�W�����v��ԂɑJ�ڂ���
	if (input->IsTriggered("A"))
	{
		//auto vel = rigidbody.GetVelocity();
		//vel.y += kJumpPower;
		//rigidbody.SetVelocity(vel);
		m_updateFunc = &Player::NormalJumpUpdate;
		m_nowAnimIdx = eAnimIdx::Jump;
		ChangeAnim(m_nowAnimIdx,0.7f);

		return;
	}
}

/// <summary>
/// �ʏ�W�����v���
/// </summary>
void Player::NormalJumpUpdate(std::shared_ptr<Input> input)
{
#if IsDebugDrawNowAnimName
	DrawFormatString(0, 32, 0xffffff, "NormalJump");
#endif
	m_isMove = false;

	m_jumpCount++;

	if (m_jumpCount == 10)
	{
		auto vel = rigidbody.GetVelocity();
		vel.y += kJumpPower;
		rigidbody.SetVelocity(vel);
	}

	if (m_isAnimationFinish)
	{
		m_jumpCount = 0;

		//�X�e�B�b�N�̓��͂��Ȃ�������ʏ��ԂɑJ�ڂ���
		if (m_moveVec.Size() == 0.0f)
		{
			m_updateFunc = &Player::NeutralUpdate;
			m_nowAnimIdx = eAnimIdx::Idle;
			ChangeAnim(m_nowAnimIdx);

			return;
		}

		//�X�e�B�b�N�̓��͂������������������ԂɑJ�ڂ���
		//if (m_moveVec.Size() < kRunVecSize)
		{
			m_updateFunc = &Player::WalkUpdate;
			m_nowAnimIdx = eAnimIdx::Walk;
			ChangeAnim(m_nowAnimIdx);

			return;
		}
	}
}

/// <summary>
/// �ړ����W�����v���
/// </summary>
void Player::MovingJumpUpdate(std::shared_ptr<Input> input)
{
#if IsDebugDrawNowAnimName
	DrawFormatString(0, 32, 0xffffff, "MovingJump");
#endif
	m_isMove = true;

	if (m_isAnimationFinish)
	{
		//�X�e�B�b�N�̓��͂��Ȃ�������ʏ��ԂɑJ�ڂ���
		if (m_moveVec.Size() == 0.0f)
		{
			m_updateFunc = &Player::NeutralUpdate;
			m_nowAnimIdx = eAnimIdx::Idle;
			ChangeAnim(m_nowAnimIdx);

			return;
		}

		//�X�e�B�b�N�̓��͂������������������ԂɑJ�ڂ���
		//if (m_moveVec.Size() < kRunVecSize)
		{
			m_updateFunc = &Player::WalkUpdate;
			m_nowAnimIdx = eAnimIdx::Walk;
			ChangeAnim(m_nowAnimIdx);

			return;
		}
	}
}

/// <summary>
/// �U�����
/// </summary>
void Player::AttackUpdate(std::shared_ptr<Input> input)
{
#if IsDebugDrawNowAnimName
	DrawFormatString(0, 32, 0xffffff, "Attack");
#endif
	m_isMove = false;

	//�U���A�j���[�V�������I�������
	if (m_isAnimationFinish)
	{
		//�U���{�^���������ꑱ���Ă����������x�U����ԂɑJ�ڂ���
		if (input->IsPushed("ATTACK"))
		{
			m_updateFunc = &Player::AttackUpdate;
			m_nowAnimIdx = eAnimIdx::Shoot;
			ChangeAnim(m_nowAnimIdx);

			return;
		}

		//�X�e�B�b�N�̓��͂��Ȃ�������ʏ��ԂɑJ�ڂ���
		if (m_moveVec.Size() == 0.0f)
		{
			m_updateFunc = &Player::NeutralUpdate;
			m_nowAnimIdx = eAnimIdx::Idle;
			ChangeAnim(m_nowAnimIdx);

			return;
		}

		//�X�e�B�b�N�̓��͂������������������ԂɑJ�ڂ���
		if (m_moveVec.Size() < kRunVecSize)
		{
			m_updateFunc = &Player::WalkUpdate;
			m_nowAnimIdx = eAnimIdx::Walk;
			ChangeAnim(m_nowAnimIdx);

			return;
		}
	}
}

/// <summary>
/// �������
/// </summary>
void Player::WalkUpdate(std::shared_ptr<Input> input)
{
#if IsDebugDrawNowAnimName
	DrawFormatString(0, 32, 0xffffff, "Walk");
#endif
	m_isMove = true;

	//�X�e�B�b�N�̓��͂��Ȃ�������ʏ��ԂɑJ�ڂ���
	if (m_moveVec.Size() == 0.0f)
	{
		m_updateFunc = &Player::NeutralUpdate;
		m_nowAnimIdx = eAnimIdx::Idle;
		ChangeAnim(m_nowAnimIdx);

		return;
	}

	//�W�����v�{�^������������ړ����W�����v��ԂɑJ�ڂ���
	if (input->IsTriggered("A"))
	{

		m_updateFunc = &Player::NormalJumpUpdate;
		m_nowAnimIdx = eAnimIdx::Jump;
		ChangeAnim(m_nowAnimIdx, 0.7f);

		SoundManager::GetInstance().PlaySE("jump");
		return;
	}
}

void Player::DeathUpdate(std::shared_ptr<Input> input)
{
	rigidbody.SetVelocity(MyLib::Vec3());

	if (m_isAnimationFinish)
	{
		if (!m_isDead)
		{
			m_isDead = true;
			m_nowAnimIdx = eAnimIdx::DeathPose;
			ChangeAnim(m_nowAnimIdx, 1.0f);
			return;
		}

	}
}

void Player::ClearUpdate(std::shared_ptr<Input> input)
{
}
