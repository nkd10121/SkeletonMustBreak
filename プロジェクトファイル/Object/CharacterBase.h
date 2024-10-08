#pragma once
#include "MyLib.h"
#include <map>
#include <string>

class CharacterBase : public MyLib::Collidable
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

	const int GetHp()const { return m_status.hp; }
	const int GetMaxHp()const { return m_maxHp; }

	// 衝突したとき
	virtual void OnCollideEnter(const std::shared_ptr<Collidable>& colider) = 0;

protected:
	//アニメーションの更新
	bool UpdateAnim(int attachNo, float startTime = 0.0f);
	//アニメーションの変更
	void ChangeAnim(int animIndex, float animSpeed = 0.5f);

protected:
	//物理クラスのポインタ
	std::shared_ptr<MyLib::Physics> m_pPhysics;
	//モデルハンドル
	int m_modelHandle;
	//モデル座標
	MyLib::Vec3 m_modelPos;
	//当たり判定座標
	MyLib::Vec3 m_collisionPos;
	//移動ベクトル
	MyLib::Vec3 m_moveVec;
	//角度
	MyLib::Vec3 m_rot;
	//ステータス
	Status m_status;
	int m_maxHp;

	/*アニメーション関係*/
	std::map<std::string, int> m_animIdx;
	int m_nowAnimIdx;
	int m_equipAnimNo;			//1つ前ののアニメーション
	int m_currentAnimNo;		//現在のアニメーション
	int m_prevAnimNo;			//変更前のアニメーション
	float m_animBlendRate;		//アニメーションの合成割合
								//0.0f:prevが再生
								//1.0f:currntが再生

	float m_animSpeed;			//アニメーション再生速度
	bool m_isAnimationFinish;	//アニメーションが終わったかどうか
};

