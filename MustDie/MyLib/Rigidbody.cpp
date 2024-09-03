#include "Rigidbody.h"

/// <summary>
/// コンストラクタ
/// </summary>
MyLib::Rigidbody::Rigidbody():
	m_pos(),
	m_nextPos(),
	m_offsetPos(),
	dir(),
	velocity(),
	useGravity(true)
{
	// 処理なし
}

/// <summary>
/// 初期化
/// </summary>
void MyLib::Rigidbody::Init(bool useGravity)
{
	m_pos = Vec3();
	dir = Vec3();
	velocity = Vec3();
	this->useGravity = useGravity;
}

/// <summary>
/// velocityに力を加える
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
/// VelocityのSetとDir自動計算
/// </summary>
void MyLib::Rigidbody::SetVelocity(Vec3 set, float mul)
{
	velocity = set * mul;
	if (velocity.SquareSize() > 0)
	{
		dir = velocity.Normalize();
	}
}