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
	//コンストラクタ
	Rigidbody();
	// 初期化
	void Init(bool m_useGravity = false);	

	// velocityに力を加える
	void AddForce(const Vec3& force);

	/*Getter*/
	//現在座標を取得
	const Vec3& GetPos() const { return m_pos; }
	//移動予定座標
	const Vec3& GetNextPos() const { return m_nextPos; }
	//向きを取得
	const Vec3& GetDir() const { return m_dir; }
	//移動速度を取得
	const Vec3& GetVelocity() const { return m_velocity; }

	//現在座標を取得
	const VECTOR GetPosVECTOR() { return m_pos.ConvertToVECTOR(); }
	//移動予定座標を取得
	const VECTOR GetNextPosVECTOR() { return m_nextPos.ConvertToVECTOR(); }
	//向きを取得
	const VECTOR GetDirVECTOR() { return m_dir.ConvertToVECTOR(); }
	//移動速度を取得
	const VECTOR GetVelocityVECTOR(){ return m_velocity.ConvertToVECTOR(); }

	//重力を与えるかどうか取得
	bool GetUseGravity() const { return m_useGravity; }		

	/*Setter*/
	//現在座標を設定
	void SetPos(const Vec3& set) { m_pos = set; }
	//移動予定座標を設定
	void SetNextPos(const Vec3& set) { m_nextPos = set; }
	//向きを設定
	void SetVelocity(Vec3 set,float mul = 1.0f);
	//移動速度を設定
	void SetUseGravity(bool set) { m_useGravity = set; }

private:
	//現在座標
	Vec3 m_pos;
	//移動予定座標
	Vec3 m_nextPos;

	//向き
	Vec3 m_dir;
	//移動速度
	Vec3 m_velocity;
	//重力を与えるかどうか
	bool m_useGravity;
};
}