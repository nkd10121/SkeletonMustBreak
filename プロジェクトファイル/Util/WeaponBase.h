#pragma once
#include "ObjectBase.h"
class WeaponBase : public ObjectBase
{
public:
	WeaponBase();
	virtual ~WeaponBase();

	void Init(int ownerModelHandle, const TCHAR* franeName, float modelSize = 1.0f);
	void Update(MyLib::Vec3 moveVec);
	void Draw();

	void SetAtk(int atk) { m_atk = atk; }
	const int GetAtk()const { return m_atk; }

	void InitCollision(std::shared_ptr<MyLib::Physics> physics);
	void CollisionEnd();

	const bool GetIsCollisionOn()const { return m_isCollisionOn; }

private:
	void SetAttachFrame(const TCHAR* franeName);

	void ConvertMatToVec();

protected:
	//モデルサイズの変更
	void SetModelSize(float modelSize);

	// 衝突したとき
	void OnCollideEnter(const std::shared_ptr<Collidable>& colider);
	void OnTriggerEnter(const std::shared_ptr<Collidable>& colider)override;

protected:
private:
	int m_ownerModelHandle;

	std::shared_ptr<MyLib::Physics> m_pPhysics;

	//武器を持たせる場所
	int m_weaponAttachFrame;
	//モデルサイズ
	float m_modelScale;

	MATRIX m_mat;

	int m_atk;

	//当たり判定の切り替え
	bool m_isCollisionOn;

};

