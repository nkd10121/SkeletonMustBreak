#pragma once
#include "ObjectBase.h"

/// <summary>
/// 攻撃を受ける当たり判定クラス
/// </summary>
class HitBox : public ObjectBase
{
public:
	//コンストラクタ
	HitBox(float r);
	//デストラクタ
	~HitBox();

	//初期化
	void Init(std::shared_ptr<MyLib::Physics> physics, MyLib::Vec3 pos,bool isHead);
	//更新
	void Update(MyLib::Vec3 pos);

	//当たり判定を削除
	void Finalize(std::shared_ptr<MyLib::Physics> physics) override;

	//ほかのオブジェクトに当たった時
	void OnTriggerEnter(const std::shared_ptr<Collidable>& colider)override;

	//当たったオブジェクトの攻撃力を取得
	int GetIsAttackNum();
	//ほかのオブジェクトに当たったかどうか
	bool GetIsHit();
	//最後に当たったオブジェクトのタグを取得
	const GameObjectTag GetHitObjectTag()const { return m_hitObjectTag; };

private:
	//物理クラスのポインタ
	std::shared_ptr<MyLib::Physics> m_pPhysics;

	//最後に当たったオブジェクトの攻撃力
	int m_lastHitObjectAttack;
	//最後に当たったオブジェクトのタグ
	GameObjectTag m_hitObjectTag;
	//このクラスの持ち主が敵かどうか
	bool m_isHead;
	//ほかのオブジェクトに当たったかどうか
	bool m_isHit;
};

