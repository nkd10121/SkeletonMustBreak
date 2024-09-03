#include "Camera.h"
#include "Input.h"

#include <cmath>

namespace
{
	constexpr float kCameraDist = 24.0f;
	constexpr float kCameraHeight = 18.0f;

	constexpr float kCameraNear = 0.5f;
	constexpr float kCameraFar = 180.0f;

	constexpr float kAngleMoveScale = 1.6f;

	constexpr float kAnalogInputMax = 1000.0f;	//�A�i���O�X�e�B�b�N������͂����x�N�g���̍ő�

	// �J�����̐���X�s�[�h
	constexpr float kCameraAngleSpeedX = 0.1f;
	constexpr float kCameraAngleSpeedY = 0.05f;

	//�J�����̊p�x����
	constexpr float kCameraAngleVLimitMin = -DX_PI_F / 2.0f + 0.6f;
	constexpr float kCameraAngleVLimitMax = DX_PI_F / 2.0f - 0.6f;

	// �J��������v���C���[�܂ł̍ő勗��
	constexpr float kCameraToPlayerLenghtMax = 175.0f;
}

Camera::Camera() :
	m_cameraAngleX(0.0f),
	m_cameraAngleY(0.0f),
	m_cameraPos(),
	m_aimPos(),
	m_playerPos(),
	m_angleMoveScale(kAngleMoveScale)
{
}

Camera::~Camera()
{
}

void Camera::Init()
{
	m_cameraAngleX = 0.0f;
	m_cameraAngleY = 12.0f;
	SetCameraNearFar(kCameraNear, kCameraFar);

}

void Camera::Update(std::shared_ptr<Input>& input, int stageHandle)
{

	//�E�X�e�B�b�N�̓��͂����炤
	auto rightStickVec = input->GetRightStick();

	//���͂���p�x���v�Z����
	float inputRateX = rightStickVec.first / kAnalogInputMax;
	float inputRateY = rightStickVec.second / kAnalogInputMax;

	if (inputRateX > 0.001f)
	{
		m_cameraAngleX -= m_angleMoveScale * std::abs(inputRateX);
	}
	else if (inputRateX < -0.001f)
	{
		m_cameraAngleX += m_angleMoveScale * std::abs(inputRateX);
	}

	if (inputRateY > 0.001f)
	{
		m_cameraAngleY += m_angleMoveScale * std::abs(inputRateY);
		if (m_cameraAngleY > 90.0f)
		{
			m_cameraAngleY = 89.99f;
		}
	}
	else if (inputRateY < -0.001f)
	{
		m_cameraAngleY -= m_angleMoveScale * std::abs(inputRateY);
		if (m_cameraAngleY < -90.0f)
		{
			m_cameraAngleY = -89.99f;
		}
	}


	// �J�����̈ʒu�̓J�����̐����p�x�Ɛ����p�x����Z�o
	// �ŏ��ɐ����p�x�𔽉f�����ʒu���Z�o
	MyLib::Vec3 tempPos1;
	float sinParam = sinf(m_cameraAngleY / 180.0f * DX_PI_F);
	float cosParam = cosf(m_cameraAngleY / 180.0f * DX_PI_F);
	tempPos1.x = 0.0f;
	tempPos1.y = sinParam * kCameraDist;
	tempPos1.z = -cosParam * kCameraDist;

	// ���ɐ����p�x�𔽉f�����ʒu���Z�o
	MyLib::Vec3 tempPos2;
	sinParam = sinf(m_cameraAngleX / 180.0f * DX_PI_F);
	cosParam = cosf(m_cameraAngleX / 180.0f * DX_PI_F);
	tempPos2.x = cosParam * tempPos1.x - sinParam * tempPos1.z;
	tempPos2.y = tempPos1.y;
	tempPos2.z = sinParam * tempPos1.x + cosParam * tempPos1.z;

	m_aimPos = MyLib::Vec3(m_playerPos.x, m_playerPos.y + 9.0f, m_playerPos.z);

	// �Z�o�������W�ɒ����_�̈ʒu�����Z�������̂��J�����̈ʒu�ɂȂ�
	m_cameraPos = tempPos2 + m_aimPos;

	auto nextPos = m_cameraPos;

	// �ŏ��̓X�e�[�W���̂Ɣ���
	m_hitDim = MV1CollCheck_Capsule(stageHandle, -1, m_aimPos.ConvertToVECTOR(), m_cameraPos.ConvertToVECTOR(), 2.0f);

	// ���o�������͂̃|���S�������J������
	MV1CollResultPolyDimTerminate(m_hitDim);

	// �X�e�[�W�̃|���S���͎��͂ɖ��������獡�x�̓R���W�����I�u�W�F�N�g�̃|���S�������͂ɂ��邩���ׂ�
	if (m_hitDim.HitNum == 0)
	{
		// ���o�������͂̃|���S�������J������
		MV1CollResultPolyDimTerminate(m_hitDim);
	}
	else if (m_hitDim.HitNum != 0)
	{
		bool doCheck = true;

		while (doCheck)
		{

			doCheck = false;

			//�v���C���[�̍��W����J�����̈ړ��\�����W�̕����x�N�g�����v�Z����
			auto playerToCamera = (m_cameraPos - m_aimPos);

			//�����Ƒ傫���ɕ�����
			auto vec = playerToCamera.Normalize();
			auto length = playerToCamera.Size();

			//�������k�߂�
			length *= 0.998f;

			auto checkPos = m_aimPos + vec * length;

			// �ŏ��̓X�e�[�W���̂Ɣ���
			m_hitDim = MV1CollCheck_Capsule(stageHandle, -1, m_aimPos.ConvertToVECTOR(), checkPos.ConvertToVECTOR(), 1.0f);
			MV1CollResultPolyDimTerminate(m_hitDim);

			if (m_hitDim.HitNum != 0)
			{
				m_cameraPos = checkPos;
				doCheck = true;
			}
			else
			{
				doCheck = false;
			}

			// HitLength �� NoHitLength ���\���ɋ߂Â��Ă��Ȃ������烋�[�v
		}

		nextPos = MyLib::Vec3(TestPosition.x, TestPosition.y, TestPosition.z);
	}


	SetCameraPositionAndTarget_UpVecY(m_cameraPos.ConvertToVECTOR(), m_aimPos.ConvertToVECTOR());



}

void Camera::DebugDraw()
{
#ifdef _DEBUG
	DrawFormatString(640, 460, 0xffffff, "%f,%f,%f", m_cameraPos.x, m_cameraPos.y, m_cameraPos.z);
#endif
}

const MyLib::Vec3 Camera::GetDirection() const
{
	return (m_aimPos - m_cameraPos).Normalize();
}

//const float Camera::GetAngle() const
//{
//	return m_cameraAngleY;
//}
