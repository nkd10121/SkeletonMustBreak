// 2024 Takeru Yui All Rights Reserved.
#pragma once
#include "Vec3.h"

#include <list>
#include <vector>
#include <unordered_map>

namespace MyLib {

class Collidable;

namespace ColInfo
{
	//最大当たり判定ポリゴン数
	constexpr int kMaxColHitPolyNum = 2000;
	//壁押し出し処理の最大試行回数
	constexpr int kMaxColHitTryNum = 16;
	//壁押し出し時にスライドさせる距離
	constexpr float kColHitSlideLength = 1.0f;
}

/// <summary>
/// 物理・衝突判定するオブジェクトを登録し、物理移動・衝突を通知する
/// </summary>
class Physics final
{
private:
	//当たり判定の種類
	enum class eOnCollideInfoKind
	{
		CollideEnter,
		CollideStay,
		CollideExit,
		TriggerEnter,
		TriggerStay,
		TriggerExit
	};

	//なにかと当たったオブジェクトの情報
	struct OnCollideInfoData
	{
		std::shared_ptr<Collidable> own;	//自分自身
		std::shared_ptr<Collidable> send;	//当たった相手
		eOnCollideInfoKind kind;			//種類
	};

	using SendCollideInfo = std::unordered_map<std::shared_ptr<Collidable>, std::list<std::shared_ptr<Collidable>>>;

public:
	//コンストラクタ
	Physics(int normalStageCollisionHandle, int enemyStageCollisionHandle);

	//衝突物の登録・登録解除
	void Entry(std::shared_ptr<Collidable> collidable);
	void Exit(std::shared_ptr<Collidable> collidable);

	//更新（登録オブジェクトの物理移動、衝突通知）
	void Update();	

private:
	//当たり判定チェック
	void CheckColide();
	//二つのオブジェクトが接触しているかどうか
	bool IsCollide(const Rigidbody& rigidA, const Rigidbody& rigidB, ColliderData* colliderA, ColliderData* colliderB) const;
	//当たったオブジェクトのペアを登録する
	void AddNewCollideInfo(const std::shared_ptr<Collidable>& objA, const std::shared_ptr<Collidable>& objB, SendCollideInfo& info);
	//移動予定の座標を修正する
	void FixNextPosition(const Rigidbody& primaryRigid, Rigidbody& secondaryRigid, ColliderData* primaryCollider, ColliderData* secondaryCollider) const;
	//種類ごとに衝突通知を飛ばす配列に追加する
	void CheckSendOnCollideInfo(SendCollideInfo& preSendInfo, SendCollideInfo& newSendInfo, bool isTrigger);
	//衝突通知を飛ばす配列に追加する
	void AddOnCollideInfo(const std::shared_ptr<Collidable>& own, const std::shared_ptr<Collidable>& send, eOnCollideInfoKind kind);
	//最終的な位置を決定する
	void FixPosition();

	//Vec3 GetClosestPtOnSegment(Vec3 pt, Vec3 start, Vec3 end);

private:
	//チェックしたポリゴンが壁ポリゴンか床ポリゴンかを判断し保存する
	void CheckWallAndFloor(std::shared_ptr<Collidable>& col);
	//壁ポリゴンとの当たり判定をチェックし、移動させる
	void FixPositionWithWall(std::shared_ptr<Collidable>& col);
	//壁の中から押し出す
	void FixPositionWithWallInternal(std::shared_ptr<Collidable>& col);
	//床ポリゴンとの当たり判定をチェックし、移動させる
	void FixNowPositionWithFloor(std::shared_ptr<Collidable>& col);

private:
	//登録されたCollidableのリスト
	std::list<std::shared_ptr<Collidable>> m_collidables;	
	//衝突したオブジェクトのリスト
	std::list<OnCollideInfoData> m_onCollideInfo;

	//衝突したオブジェクトのリスト
	SendCollideInfo m_newCollideInfo;
	//衝突したオブジェクトのリストのログ
	SendCollideInfo m_preCollideInfo;
	//衝突した押し出ししないオブジェクトのリスト
	SendCollideInfo m_newTirrigerInfo;
	//衝突した押し出ししないオブジェクトのリストのログ
	SendCollideInfo m_preTirrigerInfo;

	//通常のマップ当たり判定のハンドル
	int m_stageCollisionHandle;
	//敵のマップ当たり判定のハンドル
	int m_enemyCollisionHandle;

private:
	//移動したかどうか
	bool m_isMoveFlag = false;
	//ポリゴンに当たったかどうか
	bool m_isHitFlag = false;
	//壁ポリゴンと判断されたポリゴン数
	int m_wallNum = 0;
	//床ポリゴンと判断されたポリゴン数
	int m_floorNum = 0;

	//当たり判定結果構造体
	MV1_COLL_RESULT_POLY_DIM m_hitDim{};
	// 壁ポリゴンと判断されたポリゴンの構造体のアドレスを保存しておくためのポインタ配列
	MV1_COLL_RESULT_POLY* m_pWallPoly[ColInfo::kMaxColHitPolyNum]{};
	// 床ポリゴンと判断されたポリゴンの構造体のアドレスを保存しておくためのポインタ配列
	MV1_COLL_RESULT_POLY* m_pFloorPoly[ColInfo::kMaxColHitPolyNum]{};
	// ポリゴンの構造体にアクセスするために使用するポインタ
	MV1_COLL_RESULT_POLY* m_pPoly = nullptr;
	// 線分とポリゴンとの当たり判定の結果を代入する構造体
	HITRESULT_LINE m_lineRes{};
};

}
