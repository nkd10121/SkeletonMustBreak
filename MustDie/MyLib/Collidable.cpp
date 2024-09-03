#include "MyLib.h"
#include <cassert> 
#include "DxLib.h"
#include "MyLib.h"

MyLib::Collidable::Collidable(Priority priority, GameObjectTag tag):
	priority(priority),
	tag(tag)
{
}

MyLib::Collidable::~Collidable()
{

}

void MyLib::Collidable::Init(std::shared_ptr<MyLib::Physics> physics)
{
	physics->Entry(shared_from_this());	// •¨—î•ñ‚É©g‚ğ“o˜^
}

void MyLib::Collidable::Finalize(std::shared_ptr<MyLib::Physics> physics)
{
	physics->Exit(shared_from_this());	// •¨—î•ñ“o˜^‰ğœ
}

void MyLib::Collidable::AddThroughTag(GameObjectTag tag)
{
	bool found = (std::find(throughTags.begin(), throughTags.end(), tag) != throughTags.end());
	if (found)
	{
		assert(0 && "w’èƒ^ƒO‚ÍŠù‚É’Ç‰Á‚³‚ê‚Ä‚¢‚Ü‚·");
	}
	else
	{
		throughTags.emplace_back(tag);
	}
}

void MyLib::Collidable::RemoveThroughTag(GameObjectTag tag)
{
	bool found = (std::find(throughTags.begin(), throughTags.end(), tag) != throughTags.end());
	if (!found)
	{
		assert(0 && "w’èƒ^ƒO‚Í‘¶İ‚µ‚Ü‚¹‚ñ");
	}
	else
	{
		throughTags.remove(tag);
	}
}

bool MyLib::Collidable::IsThroughTarget(const std::shared_ptr<Collidable> target) const
{
	bool found = (std::find(throughTags.begin(), throughTags.end(), target->GetTag()) != throughTags.end());
	return found;
}

//MyLib::ColliderData* MyLib::Collidable::CreateColliderData(ColliderData::Kind kind, bool isTrigger)
//{
//	if (m_colliderData != nullptr)
//	{
//		assert(0 && "colliderData‚ÍŠù‚Éì‚ç‚ê‚Ä‚¢‚Ü‚·B");
//		return m_colliderData;
//	}
//	switch (kind)
//	{
//	case ColliderData::Kind::Line:
//		m_colliderData = new ColliderDataLine(isTrigger);
//		break;
//	case ColliderData::Kind::Sphere:
//		m_colliderData = new ColliderDataSphere(isTrigger);
//		break;
//	default:
//		assert(0 && "colliderDataì¬‚É¸”sB");
//		break;
//	}
//	return m_colliderData;
//}

std::shared_ptr<MyLib::ColliderData> MyLib::Collidable::AddCollider(const ColliderData::Kind& kind,bool isTrigger)
{
	std::shared_ptr<ColliderData> collider;

	if (kind == ColliderData::Kind::Sphere)
	{
		collider = std::make_shared<ColliderDataSphere>(isTrigger);
	}
	else if (kind == ColliderData::Kind::Line)
	{
		collider = std::make_shared<ColliderDataLine>(isTrigger);
	}
	else
	{
		assert(false);
		collider = std::make_shared<ColliderDataSphere>(isTrigger);
	}

	m_colliders.emplace_back(collider);

	return collider;
}
