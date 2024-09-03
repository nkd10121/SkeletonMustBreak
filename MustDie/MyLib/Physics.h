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
	Physics(int stageCollisionHandle);

	// 衝突物の登録・登録解除
	void Entry(std::shared_ptr<Collidable> collidable);
	void Exit(std::shared_ptr<Collidable> collidable);

	void Update();	// 更新（登録オブジェクトの物理移動、衝突通知）

private:
	// 当たり判定チェック
	void CheckColide();

	bool IsCollide(const Rigidbody& rigidA, const Rigidbody& rigidB, ColliderData* colliderA, ColliderData* colliderB) const;

	void AddNewCollideInfo(const std::shared_ptr<Collidable>& objA, const std::shared_ptr<Collidable>& objB, SendCollideInfo& info);

	// 位置補正、決定
	void FixNextPosition(const Rigidbody& primaryRigid, Rigidbody& secondaryRigid, ColliderData* primaryCollider, ColliderData* secondaryCollider) const;
	
	void CheckSendOnCollideInfo(SendCollideInfo& preSendInfo, SendCollideInfo& newSendInfo, bool isTrigger);
	void AddOnCollideInfo(const std::shared_ptr<Collidable>& own, const std::shared_ptr<Collidable>& send, eOnCollideInfoKind kind);

	void FixPosition();


	Vec3 GetClosestPtOnSegment(Vec3 pt, Vec3 start, Vec3 end);

private:	//ステージ当たり判定テスト
	//チェックしたポリゴンが壁ポリゴンか床ポリゴンかを判断し保存する
	void CheckWallAndFloor(std::shared_ptr<Collidable>& col);
	//壁ポリゴンとの当たり判定をチェックし、移動させる
	void FixPositionWithWall(std::shared_ptr<Collidable>& col);
	void FixPositionWithWallInternal(std::shared_ptr<Collidable>& col);
	//床ポリゴンとの当たり判定をチェックし、移動させる
	void FixNowPositionWithFloor(std::shared_ptr<Collidable>& col);

private:
	// 重力と最大重力加速度
	static constexpr float Gravity = -0.01f;
	static constexpr float MaxGravityAccel = -0.30f;

	static const int BeforeFixInfoColor = 0x0000ff;	// 補正前情報色
	static const int AimInfoColor = 0x00ff00;	// 補正前予定情報色
	static const int AfterFixInfoColor = 0xff00ff;	// 補正後情報色

	std::list<std::shared_ptr<Collidable>> m_collidables;	// 登録されたCollidableのリスト

	std::list<OnCollideInfoData> m_onCollideInfo;

	SendCollideInfo m_newCollideInfo;
	SendCollideInfo m_preCollideInfo;
	SendCollideInfo m_newTirrigerInfo;
	SendCollideInfo m_preTirrigerInfo;


	int m_stageCollisionHandle;

private:	//ステージ当たり判定テスト
	bool m_isMoveFlag = false;		//移動したかどうか
	bool m_isHitFlag = false;		//ポリゴンに当たったかどうか
	int m_wallNum = 0;				//壁ポリゴンと判断されたポリゴン数
	int m_floorNum = 0;				//床ポリゴンと判断されたポリゴン数

	MV1_COLL_RESULT_POLY_DIM m_hitDim{};								//当たり判定結果構造体
	MV1_COLL_RESULT_POLY* m_pWallPoly[ColInfo::kMaxColHitPolyNum]{};	// 壁ポリゴンと判断されたポリゴンの構造体のアドレスを保存しておくためのポインタ配列
	MV1_COLL_RESULT_POLY* m_pFloorPoly[ColInfo::kMaxColHitPolyNum]{};	// 床ポリゴンと判断されたポリゴンの構造体のアドレスを保存しておくためのポインタ配列
	MV1_COLL_RESULT_POLY* m_pPoly = nullptr;							// ポリゴンの構造体にアクセスするために使用するポインタ
	HITRESULT_LINE m_lineRes{};											// 線分とポリゴンとの当たり判定の結果を代入する構造体

};

}
