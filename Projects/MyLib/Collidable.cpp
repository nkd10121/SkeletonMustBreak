#include "MyLib.h"
#include <cassert> 
#include "DxLib.h"
#include "MyLib.h"

/// <summary>
/// コンストラクタ
/// </summary>
/// <param name="priority">優先度</param>
/// <param name="tag">タグ</param>
MyLib::Collidable::Collidable(Priority priority, GameObjectTag tag):
	priority(priority),
	tag(tag)
{
}

/// <summary>
/// デストラクタ
/// </summary>
MyLib::Collidable::~Collidable()
{
}

/// <summary>
/// 初期化(自身に当たり判定を追加)
/// </summary>
/// <param name="physics">物理クラス</param>
void MyLib::Collidable::Init(std::shared_ptr<MyLib::Physics> physics)
{
	physics->Entry(shared_from_this());	// 物理情報に自身を登録
}

/// <summary>
/// 終了(自身の当たり判定を削除)
/// </summary>
/// <param name="physics"></param>
void MyLib::Collidable::Finalize(std::shared_ptr<MyLib::Physics> physics)
{
	physics->Exit(shared_from_this());	// 物理情報登録解除
}

/// <summary>
/// 当たり判定を無視（スルー）するタグの追加
/// </summary>
/// <param name="tag">追加するタグ</param>
void MyLib::Collidable::AddThroughTag(GameObjectTag tag)
{
	//当たり判定を無視するタグのリストに追加予定のタグが存在するかを確認
	bool found = (std::find(throughTags.begin(), throughTags.end(), tag) != throughTags.end());
	//リストの中に既に存在していたら警告を出す
	if (found)
	{
		assert(0 && "指定タグは既に追加されています");
	}
	else //存在していなかったら追加する
	{
		throughTags.emplace_back(tag);
	}
}

/// <summary>
/// 当たり判定を無視（スルー）するタグの削除
/// </summary>
/// <param name="tag">削除するタグ</param>
void MyLib::Collidable::RemoveThroughTag(GameObjectTag tag)
{
	//当たり判定を無視するタグのリストに追加予定のタグが存在するかを確認
	bool found = (std::find(throughTags.begin(), throughTags.end(), tag) != throughTags.end());
	//存在していなかったら警告を出す
	if (!found)
	{
		assert(0 && "指定タグは存在しません");
	}
	else //存在していたら削除する
	{
		throughTags.remove(tag);
	}
}

/// <summary>
/// 当たり判定を無視（スルー）する対象かどうか
/// </summary>
/// <param name="target">無視する対象か確認したい当たり判定</param>
/// <returns></returns>
bool MyLib::Collidable::IsThroughTarget(const std::shared_ptr<Collidable> target) const
{
	//確認したい当たり判定のタグが無視するタグのリストに含まれているかどうか調べる
	bool found = (std::find(throughTags.begin(), throughTags.end(), target->GetTag()) != throughTags.end());
	return found;
}

/// <summary>
/// 当たり判定を追加
/// </summary>
/// <param name="kind">当たり判定の種類</param>
/// <param name="isTrigger">押し出し判定をするかどうか</param>
/// <returns></returns>
std::shared_ptr<MyLib::ColliderData> MyLib::Collidable::AddCollider(const ColliderData::Kind& kind,bool isTrigger)
{
	std::shared_ptr<ColliderData> add;

	//種類によって追加する当たり判定の派生先を変更する
	if (kind == ColliderData::Kind::Sphere)
	{
		add = std::make_shared<ColliderDataSphere>(isTrigger);
	}
	else if (kind == ColliderData::Kind::Line)
	{
		add = std::make_shared<ColliderDataLine>(isTrigger);
	}
	else
	{
		assert(0 && "指定された種類の当たり判定を追加できませんでした");
	}

	//当たり判定を追加する
	m_colliders.emplace_back(add);

	return add;
}
