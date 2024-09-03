#pragma once
#include "GameObjectTag.h"
#include <List>
#include <memory>

namespace MyLib
{

class Rigidbody;
class Physics;
class ColliderData;

class Collidable : public std::enable_shared_from_this<Collidable>
{
	friend Physics;
public:

	// 位置補正の優先度の判別に使う
	enum class Priority : int
	{
		Low,		// 低
		Middle,		// 中
		High,		// 高
		Static,		// 動かない（最高）
	};

public:
	Collidable(Priority priority, GameObjectTag tag);	// コンストラクタ
	virtual ~Collidable();													// デストラクタ
	virtual void Init(std::shared_ptr<MyLib::Physics> physics);
	virtual void Finalize(std::shared_ptr<MyLib::Physics> physics);

	//virtual void OnCollide(const Collidable& colider) abstract;				// 衝突したとき

	virtual void OnCollideEnter(const std::shared_ptr<Collidable>& colider) {}
	virtual void OnCollideStay(const std::shared_ptr<Collidable>& colider) {}
	virtual void OnCollideExit(const std::shared_ptr<Collidable>& colider) {}
	virtual void OnTriggerEnter(const std::shared_ptr<Collidable>& colider) {}
	virtual void OnTriggerStay(const std::shared_ptr<Collidable>& colider) {}
	virtual void OnTriggerExit(const std::shared_ptr<Collidable>& colider) {}

	GameObjectTag	GetTag()		const { return tag; }					// タグ情報
	Priority		GetPriority()	const { return priority; }				// 優先度

	// 当たり判定を無視（スルー）するタグの追加/削除
	void AddThroughTag(GameObjectTag tag);
	void RemoveThroughTag(GameObjectTag tag);

	// 当たり判定を無視（スルー）する対象かどうか
	bool IsThroughTarget(const std::shared_ptr<Collidable> target) const;

protected:
	Rigidbody		rigidbody;		// 物理データ
	std::list<std::shared_ptr<ColliderData>> m_colliders;

	std::shared_ptr<ColliderData> AddCollider(const ColliderData::Kind& kind, bool isTrigger);
private:
	//ColliderData* CreateColliderData(ColliderData::Kind kind, bool isTrigger);



	GameObjectTag	tag;
	Priority		priority;



	// 当たり判定を無視（スルー）するタグのリスト
	// HACK: Unityみたいにレイヤーの追加の方がスマート
	// FIXME: 本来ColliderData側じゃない？
	std::list<GameObjectTag>	throughTags;

	// PhysicsがCollidableを自由に管理するためにフレンド化

	// 以下、フレンドであるPhisicsのみが扱う型や変数
private:
	// FIXME: これはPhysicsが持つ方が望ましい
};
}


