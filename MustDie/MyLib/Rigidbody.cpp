#include "Rigidbody.h"

/// <summary>
/// �R���X�g���N�^
/// </summary>
MyLib::Rigidbody::Rigidbody():
	m_pos(),
	m_nextPos(),
	m_offsetPos(),
	dir(),
	velocity(),
	useGravity(true)
{
	// �����Ȃ�
}

/// <summary>
/// ������
/// </summary>
void MyLib::Rigidbody::Init(bool useGravity)
{
	m_pos = Vec3();
	dir = Vec3();
	velocity = Vec3();
	this->useGravity = useGravity;
}

/// <summary>
/// velocity�ɗ͂�������
/// </summary>
void MyLib::Rigidbody::AddForce(const Vec3& force)
{
	velocity = velocity + force;
}

void MyLib::Rigidbody::SetPos(const Vec3& set)
{
	m_pos = set;
}

void MyLib::Rigidbody::MV1SetPos(const int& handle)
{
	MV1SetPosition(handle, m_pos.ConvertToVECTOR());
}

/// <summary>
/// Velocity��Set��Dir�����v�Z
/// </summary>
void MyLib::Rigidbody::SetVelocity(Vec3 set, float mul)
{
	velocity = set * mul;
	if (velocity.SquareSize() > 0)
	{
		dir = velocity.Normalize();
	}
}