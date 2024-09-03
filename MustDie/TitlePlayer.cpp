#include "TitlePlayer.h"
#include "DxLib.h"

namespace
{
	constexpr float kModelSize =80.0f;


	//�A�j���[�V�����̐؂�ւ��ɂ�����t���[����
	constexpr float kAnimChangeFrame = 10.0f;
	constexpr float kAnimChangeRateSpeed = 1.0f / kAnimChangeFrame;

	//�A�j���[�V�����u�����h���̍ő�
	constexpr float kAnimBlendRateMax = 1.0f;
}

TitlePlayer::TitlePlayer():
	m_currentAnimNo(-1),
	m_prevAnimNo(-1),
	m_animBlendRate(1.0f),
	m_animSpeed(0.5f)
{
	m_modelHandle = MV1LoadModel("data/model/player.mv1");
	MV1SetScale(m_modelHandle,VECTOR(kModelSize, kModelSize, kModelSize));
	MV1SetRotationXYZ(m_modelHandle, VECTOR(0.0f, DX_PI_F, 0.0f));

	//�ҋ@�A�j���[�V������ݒ�
	m_currentAnimNo = MV1AttachAnim(m_modelHandle, 69);
}

TitlePlayer::~TitlePlayer()
{
	MV1DeleteModel(m_modelHandle);
}

void TitlePlayer::Init(MyLib::Vec3 pos)
{
	MV1SetPosition(m_modelHandle,pos.ConvertToVECTOR());
}

void TitlePlayer::Update()
{
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

	UpdateAnim(m_currentAnimNo);
}

void TitlePlayer::Draw()
{
	MV1DrawModel(m_modelHandle);
}

bool TitlePlayer::UpdateAnim(int attachNo, float startTime)
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

void TitlePlayer::ChangeAnim(int animIndex, float animSpeed)
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
