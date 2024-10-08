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
	//コンストラクタ
	Collidable(Priority priority, GameObjectTag tag);	
	//デストラクタ
	virtual ~Collidable();												
	//初期化
	virtual void Init(std::shared_ptr<MyLib::Physics> physics);
	//終了
	virtual void Finalize(std::shared_ptr<MyLib::Physics> physics);

	//当たり判定関数
	virtual void OnCollideEnter(const std::shared_ptr<Collidable>& colider) {}
	virtual void OnCollideStay(const std::shared_ptr<Collidable>& colider) {}
	virtual void OnCollideExit(const std::shared_ptr<Collidable>& colider) {}
	virtual void OnTriggerEnter(const std::shared_ptr<Collidable>& colider) {}
	virtual void OnTriggerStay(const std::shared_ptr<Collidable>& colider) {}
	virtual void OnTriggerExit(const std::shared_ptr<Collidable>& colider) {}

	//タグを取得
	GameObjectTag GetTag() const { return tag; }					
	//優先度を取得
	Priority GetPriority() const { return priority; }				

	//当たり判定を無視（スルー）するタグの追加/削除
	void AddThroughTag(GameObjectTag tag);
	void RemoveThroughTag(GameObjectTag tag);

	//当たり判定を無視（スルー）する対象かどうか
	bool IsThroughTarget(const std::shared_ptr<Collidable> target) const;

protected:
	//当たり判定を追加
	std::shared_ptr<ColliderData> AddCollider(const ColliderData::Kind& kind, bool isTrigger);

protected:
	// 物理データ
	Rigidbody rigidbody;		
	//当たり判定情報
	std::list<std::shared_ptr<ColliderData>> m_colliders;

private:
	//タグ
	GameObjectTag tag;
	//優先度
	Priority priority;

	// 当たり判定を無視（スルー）するタグのリスト
	std::list<GameObjectTag>	throughTags;
};
}