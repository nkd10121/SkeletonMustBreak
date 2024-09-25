#pragma once
#include "ObjectBase.h"

class CharacterBase : public ObjectBase
{
public:
	//ステータス構造体
	struct Status
	{
		int hp;		//体力
		int atk;	//攻撃力
		int def;	//防御力
		float speed;	//移動速度
		int point;	//死亡時のポイント
	};
public:
	CharacterBase(Priority priority, GameObjectTag tag);
	virtual ~CharacterBase();

	// 衝突したとき
	virtual void OnCollideEnter(const std::shared_ptr<Collidable>& colider) = 0;

protected:

protected:
	//移動ベクトル
	MyLib::Vec3 m_modelPos;
	MyLib::Vec3 m_collisionPos;
	MyLib::Vec3 m_moveVec;
	Status m_status;
};

