#pragma once
#include <DxLib.h>
#include "Vec3.h"

namespace MyLib
{

/// <summary>
/// 物理、衝突判定に必要なデータの塊
/// </summary>
class Rigidbody
{
public:
	Rigidbody();
	void Init(bool useGravity = false);	// 初期化

	// velocityに力を加える
	void AddForce(const Vec3& force);

	// Getter/Setter
	const Vec3& GetPos() const { return m_pos; }
	const Vec3& GetNextPos() const { return m_nextPos; }
	const Vec3& GetOffsetPos() const { return m_offsetPos; }
	const Vec3& GetDir() const { return dir; }
	const Vec3& GetVelocity() const { return velocity; }

	const VECTOR GetPosVECTOR() { return m_pos.ConvertToVECTOR(); }
	const VECTOR GetNextPosVECTOR() { return m_nextPos.ConvertToVECTOR(); }
	const VECTOR GetOffsetPosVECTOR() { return m_offsetPos.ConvertToVECTOR(); }
	const VECTOR GetDirVECTOR() { return dir.ConvertToVECTOR(); }
	const VECTOR GetVelocityVECTOR(){ return velocity.ConvertToVECTOR(); }
	bool UseGravity() const { return useGravity; }

	void SetPos(const Vec3& set);
	void SetNextPos(const Vec3& set) { m_nextPos = set; }
	void SetOffsetPos(const Vec3& set) { m_offsetPos = set; }
	void MV1SetPos(const int& handle);
	void SetVelocity(Vec3 set,float mul = 1.0f);
	void SetUseGravity(bool set) { useGravity = set; }

private:
	Vec3	m_pos;
	Vec3	m_nextPos;
	//モデルの中心座標と当たり判定がずれているとき用
	Vec3 m_offsetPos;

	Vec3	dir;
	Vec3	velocity;
	bool	useGravity;
};

}