#include "Rigidbody.h"

/// <summary>
/// �R���X�g���N�^
/// </summary>
MyLib::Rigidbody::Rigidbody():
	m_pos(),
	m_nextPos(),
	m_dir(),
	m_velocity(),
	m_useGravity(true)
{
	// �����Ȃ�
}

/// <summary>
/// ������
/// </summary>
void MyLib::Rigidbody::Init(bool m_useGravity)
{
	m_pos = Vec3();
	m_dir = Vec3();
	m_velocity = Vec3();
	this->m_useGravity = m_useGravity;
}

/// <summary>
/// velocity�ɗ͂�������
/// </summary>
void MyLib::Rigidbody::AddForce(const Vec3& force)
{
	m_velocity = m_velocity + force;
}

/// <summary>
/// Velocity��Set��Dir�����v�Z
/// </summary>
void MyLib::Rigidbody::SetVelocity(Vec3 set, float mul)
{
	m_velocity = set * mul;
	if (m_velocity.SquareSize() > 0)
	{
		m_dir = m_velocity.Normalize();
	}
}