#include <cassert>
#include "MyLib.h"
#include "DxLib.h"

//using namespace MyLib;

namespace
{
	//最大当たり判定ポリゴン数
	constexpr int kMaxColHitPolyNum = 2000;
	//壁押し出し処理の最大試行回数
	constexpr int kMaxColHitTryNum = 16;
	//壁押し出し時にスライドさせる距離
	constexpr float kColHitSlideLength = 0.2f;
	//壁ポリゴンか床ポリゴンかを判断するための変数
	constexpr float kWallPolyBorder = 0.4f;
	//壁ポリゴンと判断するための高さ変数
	constexpr float kWallPolyHeight = 5.0f;

	//重力
	constexpr float kGravity = -0.018f;
	//最大重力加速度
	constexpr float kMaxGravityAccel = -2.00f;

	//補正前情報色
	const int kBeforeFixInfoColor = 0x0000ff;
	//補正前予定情報色
	const int kAimInfoColor = 0x00ff00;
	//補正後情報色
	const int kAfterFixInfoColor = 0xff00ff;
}

/// <summary>
/// コンストラクタ
/// </summary>
/// <param name="normalStageCollisionHandle">プレイヤーなどのステージ当たり判定ハンドル</param>
/// <param name="enemyStageCollisionHandle">敵のステージ当たり判定ハンドル</param>
MyLib::Physics::Physics(int normalStageCollisionHandle,int enemyStageCollisionHandle)
{
	m_stageCollisionHandle = normalStageCollisionHandle;
	m_enemyCollisionHandle = enemyStageCollisionHandle;
}

/// <summary>
/// 衝突物の登録
/// </summary>
/// <param name="collidable">追加する当たり判定</param>
void MyLib::Physics::Entry(std::shared_ptr<Collidable> collidable)
{
	// 登録
	bool found = (std::find(m_collidables.begin(), m_collidables.end(), collidable) != m_collidables.end());
	if (!found)
	{
		m_collidables.emplace_back(collidable);
		//collidables.push_back(collidable);
	}
	// 既に登録されてたらエラー
	else
	{
		assert(0 && "指定のcollidableは登録済です。");
	}
}

/// <summary>
/// 衝突物の登録削除
/// </summary>
/// <param name="collidable">削除する当たり判定</param>
void MyLib::Physics::Exit(std::shared_ptr<Collidable> collidable)
{
	bool found = (std::find(m_collidables.begin(), m_collidables.end(), collidable) != m_collidables.end());
	if (found)
	{
		m_collidables.remove(collidable);
	}
	// 登録されてなかったらエラー
	else
	{
		assert(0 && "指定のcollidableが登録されていません。");
	}
}

/// <summary>
/// 更新（登録オブジェクトの物理移動、衝突通知）
/// </summary>
void MyLib::Physics::Update()
{
	//当たり判定のログを残す
	m_preCollideInfo = m_newCollideInfo;
	m_newCollideInfo.clear();
	m_preTirrigerInfo = m_newTirrigerInfo;
	m_newTirrigerInfo.clear();
	m_onCollideInfo.clear();

	// 移動
	for (auto& item : m_collidables)
	{
		// ポジションに移動力を足す
		auto pos = item->rigidbody.GetPos();
		auto m_velocity = item->rigidbody.GetVelocity();

		// 重力を利用する設定なら、重力を追加する
		if (item->rigidbody.GetUseGravity())
		{
			m_velocity = m_velocity + Vec3(0.0f, kGravity, 0.0f);

			// 最大重力加速度より大きかったらクランプ
			if (m_velocity.y < kMaxGravityAccel)
			{
				m_velocity = MyLib::Vec3(m_velocity.x, kMaxGravityAccel, m_velocity.z);
			}
		}

		auto nextPos = pos + m_velocity;

		item->rigidbody.SetVelocity(m_velocity);

		// もともとの情報、予定情報をデバッグ表示
#if _DEBUG

		for (const auto& collider : item->m_colliders)
		{
			auto kind = collider->GetKind();
			if (kind == ColliderData::Kind::Sphere)
			{
				auto sphereData = dynamic_cast<MyLib::ColliderDataSphere*> (collider.get());
				auto radius = sphereData->m_radius;
				MyLib::DebugDraw::AddDrawSphere(pos, radius, kBeforeFixInfoColor);
				MyLib::DebugDraw::AddDrawSphere(nextPos, radius, kAimInfoColor);
			}
		}

#endif
		// 予定ポジション設定
		item->rigidbody.SetNextPos(nextPos);
	}

	// 当たり判定チェック（nextPos指定）
	CheckColide();

	for (auto& item : m_collidables)
	{
		if (item->GetTag() == GameObjectTag::Player || item->GetTag() == GameObjectTag::Enemy)
		{
			float rad = 0;
			int modelHandle = -1;

			for (auto& col : item->m_colliders)
			{
				rad = dynamic_cast<MyLib::ColliderDataSphere*> (col.get())->m_radius;

				if (item->GetTag() == GameObjectTag::Player)
				{
					modelHandle = m_stageCollisionHandle;
				}
				else
				{
					modelHandle = m_enemyCollisionHandle;
				}
			}
			m_hitDim = MV1CollCheck_Sphere(modelHandle, -1, item->rigidbody.GetNextPosVECTOR(), rad);
		}
		else
		{
			continue;
		}

		//壁と床の当たり判定を行う
		CheckWallAndFloor(item);
		//壁との当たり判定処理
		FixPositionWithWall(item);
		//床との当たり判定処理
		FixNowPositionWithFloor(item);

		// 検出したプレイヤーの周囲のポリゴン情報を開放する
		MV1CollResultPolyDimTerminate(m_hitDim);
	}

	CheckSendOnCollideInfo(m_preCollideInfo, m_newCollideInfo, false);
	CheckSendOnCollideInfo(m_preTirrigerInfo, m_newTirrigerInfo, true);

	// 位置確定
	FixPosition();

	for (const auto& info : m_onCollideInfo)
	{
		if (info.kind == eOnCollideInfoKind::CollideEnter)
		{
			info.own->OnCollideEnter(info.send);
		}
		else if (info.kind == eOnCollideInfoKind::CollideStay)
		{
			info.own->OnCollideStay(info.send);
		}
		else if (info.kind == eOnCollideInfoKind::CollideExit)
		{
			info.own->OnCollideExit(info.send);
		}
		else if (info.kind == eOnCollideInfoKind::TriggerEnter)
		{
			info.own->OnTriggerEnter(info.send);
		}
		else if (info.kind == eOnCollideInfoKind::TriggerStay)
		{
			info.own->OnTriggerStay(info.send);
		}
		else if (info.kind == eOnCollideInfoKind::TriggerExit)
		{
			info.own->OnTriggerExit(info.send);
		}
	}
}

/// <summary>
/// 当たり判定チェック
/// </summary>
void MyLib::Physics::CheckColide()
{
	std::vector<OnCollideInfoData> onCollideInfo;
	// 衝突通知、ポジション補正
	bool	doCheck = true;
	int		checkCount = 0;	// チェック回数
	while (doCheck)
	{
		doCheck = false;
		++checkCount;

		// 2重ループで全オブジェクト当たり判定
		// FIXME: 重いので近いオブジェクト同士のみ当たり判定するなど工夫がいる
		for (const auto& objA : m_collidables)
		{
			for (const auto& objB : m_collidables)
			{
				//同一オブジェクトなら早期リターン
				if (objA == objB)
					continue;

				for (const auto& colA : objA->m_colliders)
				{
					for (const auto& colB : objB->m_colliders)
					{
						if (!IsCollide(objA->rigidbody, objB->rigidbody, colA.get(), colB.get())) continue;

						bool isTrigger = colA->IsTrigger() || colB->IsTrigger();

						if (isTrigger)
						{
							AddNewCollideInfo(objA, objB, m_newTirrigerInfo);
						}
						else
						{
							AddNewCollideInfo(objA, objB, m_newCollideInfo);
						}

						// Triggerの場合は位置補正はしない
						if (isTrigger) continue;

						auto primary = objA;
						auto secondary = objB;

						if (primary == secondary)
						{
							break;
						}

						auto primaryCollider = colA;
						auto secondaryCollider = colB;
						if (objA->priority < objB->priority)
						{
							primary = objB;
							secondary = objA;
							primaryCollider = colB;
							secondaryCollider = colA;
						}

						FixNextPosition(primary->rigidbody, secondary->rigidbody, primaryCollider.get(), secondaryCollider.get());
						// 位置補正をしたらもう一度初めから行う
						doCheck = true;
						break;
					}
					if (doCheck) break;
				}
				if (doCheck) break;
			}
			if (doCheck) break;
		}
		if (doCheck && checkCount > 800)
		{
#if _DEBUG
			printfDx("規定回数を超えました");
#endif
			break;
		}
	}
}

/// <summary>
/// /二つのオブジェクトが接触しているかどうか
/// </summary>
/// <param name="rigidA">オブジェクトAの物理データ</param>
/// <param name="rigidB">オブジェクトBの物理データ</param>
/// <param name="colliderA">オブジェクトAの当たり判定データ</param>
/// <param name="colliderB">オブジェクトBの当たり判定データ</param>
/// <returns></returns>
bool MyLib::Physics::IsCollide(const Rigidbody& rigidA, const Rigidbody& rigidB, ColliderData* colliderA, ColliderData* colliderB) const
{
	bool isCollide = false;

	auto kindA = colliderA->GetKind();
	auto kindB = colliderB->GetKind();

	if (kindA == MyLib::ColliderData::Kind::Sphere && kindB == MyLib::ColliderData::Kind::Sphere)
	{
		auto colA = dynamic_cast<MyLib::ColliderDataSphere*>(colliderA);
		auto colB = dynamic_cast<MyLib::ColliderDataSphere*>(colliderB);

		auto atob = rigidA.GetNextPos() - rigidB.GetNextPos();
		auto atobLength = atob.Size();

		// お互いの距離が、それぞれの半径を足したものより小さければ当たる
		isCollide = (atobLength < colA->m_radius + colB->m_radius);
	}
	return isCollide;
}

/// <summary>
/// 当たったオブジェクトのペアを登録する
/// </summary>
/// <param name="objA">オブジェクトA</param>
/// <param name="objB">オブジェクトB</param>
/// <param name="info">登録する配列</param>
void MyLib::Physics::AddNewCollideInfo(const std::shared_ptr<Collidable>& objA, const std::shared_ptr<Collidable>& objB, SendCollideInfo& info)
{
	bool isParentA = info.find(objA) != info.end();
	bool isParentB = info.find(objB) != info.end();
	if (isParentA || isParentB)
	{
		std::shared_ptr<Collidable> parent = objA;
		std::shared_ptr<Collidable> child = objB;
		if (isParentB)
		{
			parent = objB;
			child = objA;
		}
		bool isChild = std::find(info[parent].begin(), info[parent].end(), child) != info[parent].end();
		if (!isChild)
		{
			info[parent].emplace_back(child);
		}
	}
	else
	{
		info[objA].emplace_back(objB);
	}
}

/// <summary>
/// 移動予定の座標を修正する
/// </summary>
/// <param name="primaryRigid">優先度高オブジェクトの物理データ</param>
/// <param name="secondaryRigid">優先度低オブジェクトの物理データ</param>
/// <param name="primaryCollider">優先度高オブジェクトの当たり判定データ</param>
/// <param name="secondaryCollider">優先度低オブジェクトの当たり判定データ</param>
void MyLib::Physics::FixNextPosition(const Rigidbody& primaryRigid, Rigidbody& secondaryRigid, ColliderData* primaryCollider, ColliderData* secondaryCollider) const
{
	// 当たり判定の種別ごとに補正方法を変える
	auto primaryKind = primaryCollider->GetKind();
	auto secondaryKind = secondaryCollider->GetKind();

	if (primaryKind == MyLib::ColliderData::Kind::Sphere && secondaryKind == MyLib::ColliderData::Kind::Sphere)
	{
		auto primaryToSecondary = secondaryRigid.GetNextPos() - primaryRigid.GetNextPos();
		auto primaryToSecondaryN = primaryToSecondary.Normalize();

		auto primaryColliderData = dynamic_cast<MyLib::ColliderDataSphere*>(primaryCollider);
		auto secondaryColliderData = dynamic_cast<MyLib::ColliderDataSphere*>(secondaryCollider);
		auto awayDist = primaryColliderData->m_radius + secondaryColliderData->m_radius + 0.0001f;	// そのままだとちょうど当たる位置になるので少し余分に離す
		auto primaryToNewSecondaryPos = primaryToSecondaryN * awayDist;
		auto fixedPos = primaryRigid.GetNextPos() + primaryToNewSecondaryPos;
		fixedPos.y = secondaryRigid.GetPos().y;
		secondaryRigid.SetNextPos(fixedPos);
	}
}

/// <summary>
/// 種類ごとに衝突通知を飛ばす配列に追加する
/// </summary>
/// <param name="preSendInfo">衝突したオブジェクトのリストのログ</param>
/// <param name="newSendInfo">衝突したオブジェクトのリスト</param>
/// <param name="isTrigger">無視するかどうか</param>
void MyLib::Physics::CheckSendOnCollideInfo(SendCollideInfo& preSendInfo, SendCollideInfo& newSendInfo, bool isTrigger)
{
	for (auto& parent : newSendInfo)
	{
		// 以前の情報に親として登録されているか
		bool isPreParent = preSendInfo.find(parent.first) != preSendInfo.end();
		bool isAllEnter = true;

		for (auto& child : parent.second)
		{
			bool isPreChild = false;
			if (isPreParent)
			{
				// 以前の情報に子として登録されているか
				auto& preParent = preSendInfo[parent.first];
				isPreChild = std::find(preParent.begin(), preParent.end(), child) != preParent.end();
			}

			// 今回入ってきた場合はEnterを呼ぶ(子として登録されていない)
			if (!isPreChild)
			{
				if (isTrigger)
				{
					AddOnCollideInfo(parent.first, child, eOnCollideInfoKind::TriggerEnter);
					AddOnCollideInfo(child, parent.first, eOnCollideInfoKind::TriggerEnter);
				}
				else
				{
					AddOnCollideInfo(parent.first, child, eOnCollideInfoKind::CollideEnter);
					AddOnCollideInfo(child, parent.first, eOnCollideInfoKind::CollideEnter);
				}
			}

			// Staryは毎度呼ぶ
			if (isTrigger)
			{
				AddOnCollideInfo(parent.first, child, eOnCollideInfoKind::TriggerStay);
				AddOnCollideInfo(child, parent.first, eOnCollideInfoKind::TriggerStay);
			}
			else
			{
				AddOnCollideInfo(parent.first, child, eOnCollideInfoKind::CollideStay);
				AddOnCollideInfo(child, parent.first, eOnCollideInfoKind::CollideStay);
			}

			// 登録されていた情報を削除
			if (isPreChild)
			{
				preSendInfo[parent.first].remove(child);
			}
			// 全て登録されていなかったとする
			else
			{
				isAllEnter = false;
			}
		}

		// 全て登録されていたら親の情報も消す
		if (isAllEnter)
		{
			preSendInfo.erase(parent.first);
		}
	}

	// 残っている前情報からExistを呼ぶ(登録が残っている＝今回抜けた)
	for (auto& parent : preSendInfo)
	{
		for (auto& child : parent.second)
		{
			if (isTrigger)
			{
				AddOnCollideInfo(parent.first, child, eOnCollideInfoKind::TriggerExit);
				AddOnCollideInfo(child, parent.first, eOnCollideInfoKind::TriggerExit);
			}
			else
			{
				AddOnCollideInfo(parent.first, child, eOnCollideInfoKind::CollideExit);
				AddOnCollideInfo(child, parent.first, eOnCollideInfoKind::CollideExit);
			}
		}
	}
}

/// <summary>
/// 衝突通知を飛ばす配列に追加する
/// </summary>
/// <param name="own">自身</param>
/// <param name="send">衝突した相手</param>
/// <param name="kind">当たり判定の種類</param>
void MyLib::Physics::AddOnCollideInfo(const std::shared_ptr<Collidable>& own, const std::shared_ptr<Collidable>& send, eOnCollideInfoKind kind)
{
	OnCollideInfoData addInfo;
	addInfo.own = own;
	addInfo.send = send;
	addInfo.kind = kind;
	m_onCollideInfo.emplace_back(addInfo);
}

/// <summary>
/// 最終的な位置を決定する
/// </summary>
void MyLib::Physics::FixPosition()
{
	for (auto& item : m_collidables)
	{
#if _DEBUG
		//// 補正後の位置をデバッグ表示
		//DebugDraw::DrawLine(item->rigidbody.GetPos(), item->nextPos, AfterFixInfoColor);

		//if (item->colliderData->GetKind() == ColliderData::Kind::Circle2D)
		//{
		//	auto itemCircleData = dynamic_cast<ColliderDataCircle2D*>(item->colliderData);
		//	DebugDraw::DrawCircle(item->nextPos, itemCircleData->radius, AfterFixInfoColor);
		//}
#endif
		// Posを更新するので、velocityもそこに移動するvelocityに修正
		Vec3 toFixedPos = item->rigidbody.GetNextPos() - item->rigidbody.GetPos();
		item->rigidbody.SetVelocity(toFixedPos);

		// 位置確定
		item->rigidbody.SetPos(item->rigidbody.GetNextPos());
	}
}


//MyLib::Vec3 MyLib::Physics::GetClosestPtOnSegment(Vec3 pt, Vec3 start, Vec3 end)
//{
//	// 最近接点がstart側線分外領域の場合
//	auto startToPt = pt - start;
//	auto startToEnd = end - start;
//	auto startToEndN = startToEnd.Normalize();
//	if (startToPt.Dot(startToEndN) < 0)
//	{
//		return start;
//	}
//	auto endToPt = pt - end;
//	auto endToStart = start - end;
//	auto endToStartN = endToStart.Normalize();
//	// 最近接点がend側線分外領域の場合
//	if (endToPt.Dot(endToStartN) < 0)
//	{
//		return end;
//	}
//	// 中間領域の場合
//	else
//	{
//		float t = startToEndN.Dot(startToPt);
//		return start + startToEndN * t;
//	}
//}

/// <summary>
/// チェックしたポリゴンが壁ポリゴンか床ポリゴンかを判断し保存する
/// </summary>
/// <param name="col">チェックするオブジェクト</param>
void MyLib::Physics::CheckWallAndFloor(std::shared_ptr<Collidable>& col)
{
	// 壁ポリゴンと床ポリゴンの数を初期化する
	m_wallNum = 0;
	m_floorNum = 0;

	// 検出されたポリゴンの数だけ繰り返し
	for (int i = 0; i < m_hitDim.HitNum; i++)
	{
		// ポリゴンの法線のＹ成分が壁ポリゴンボーダーに達っしているかどうかで壁ポリゴンか床ポリゴンかを判断する
		if (m_hitDim.Dim[i].Normal.y < kWallPolyBorder && m_hitDim.Dim[i].Normal.y > -kWallPolyBorder)
		{
			// 壁ポリゴンと判断された場合でも、プレイヤーのＹ座標より高いポリゴンのみ当たり判定を行う
			if (m_hitDim.Dim[i].Position[0].y > col->rigidbody.GetPos().y + kWallPolyHeight ||
				m_hitDim.Dim[i].Position[1].y > col->rigidbody.GetPos().y + kWallPolyHeight ||
				m_hitDim.Dim[i].Position[2].y > col->rigidbody.GetPos().y + kWallPolyHeight)
			{
				// ポリゴンの数が限界数に達していなかったらポリゴンを配列に追加
				if (m_wallNum < ColInfo::kMaxColHitPolyNum)
				{
					// ポリゴンの構造体のアドレスを壁ポリゴンポインタ配列に保存する
					m_pWallPoly[m_wallNum] = &m_hitDim.Dim[i];

					// 壁ポリゴンの数を加算する
					m_wallNum++;
				}
			}
		}
		else
		{
			// ポリゴンの数が限界数に達していなかったらポリゴンを配列に追加
			if (m_floorNum < ColInfo::kMaxColHitPolyNum)
			{
				// ポリゴンの構造体のアドレスを床ポリゴンポインタ配列に保存する
				m_pFloorPoly[m_floorNum] = &m_hitDim.Dim[i];

				// 床ポリゴンの数を加算する
				m_floorNum++;
			}
		}
	}
}

/// <summary>
/// 壁ポリゴンとの当たり判定をチェックし、移動させる
/// </summary>
/// <param name="col">チェックするオブジェクト</param>
void MyLib::Physics::FixPositionWithWall(std::shared_ptr<Collidable>& col)
{
	float radius = 0.0f;
	for (auto& col : col->m_colliders)
	{
		radius = dynamic_cast<MyLib::ColliderDataSphere*> (col.get())->m_radius;
	}

	// 壁ポリゴンがない場合は何もしない
	if (m_wallNum == 0) return;

	// 壁ポリゴンとの当たり判定処理
	// 壁に当たったかどうかのフラグは初期状態では「当たっていない」にしておく
	m_isHitFlag = false;

	// 移動したかどうかで処理を分岐
	if (col->rigidbody.GetDir().Size() != 0.0f)
	{
		// 壁ポリゴンの数だけ繰り返し
		for (int i = 0; i < m_wallNum; i++)
		{
			// i番目の壁ポリゴンのアドレスを壁ポリゴンポインタ配列から取得
			m_pPoly = m_pWallPoly[i];

			// ポリゴンとプレイヤーが当たっていなかったら次のカウントへ
			if (!HitCheck_Sphere_Triangle(col->rigidbody.GetNextPosVECTOR(), radius,
				m_pPoly->Position[0], m_pPoly->Position[1], m_pPoly->Position[2])) continue;

			// ここにきたらポリゴンとプレイヤーが当たっているということなので、ポリゴンに当たったフラグを立てる
			m_isHitFlag = true;

			//壁を考慮した移動を外積を使って算出
			MyLib::Vec3 SlideVec;

			VECTOR ret;
			ret = VCross(col->rigidbody.GetVelocityVECTOR(), m_pPoly->Normal);
			// 進行方向ベクトルと壁ポリゴンの法線ベクトルに垂直なベクトルを算出
			SlideVec = MyLib::Vec3(ret.x, ret.y, ret.z);

			// 算出したベクトルと壁ポリゴンの法線ベクトルに垂直なベクトルを算出、これが
			// 元の移動成分から壁方向の移動成分を抜いたベクトル
			ret = VCross(m_pPoly->Normal, SlideVec.ConvertToVECTOR());
			SlideVec = MyLib::Vec3(ret.x, ret.y, ret.z);

			// それを移動前の座標に足したものを新たな座標とする
			col->rigidbody.SetNextPos(col->rigidbody.GetPos() + SlideVec);

			//変更したため新たに取得
			//tempNextPos = col->rigidbody.GetNextPos();
			//nextPos = tempNextPos.ConvertToVECTOR();

			// 新たな移動座標で壁ポリゴンと当たっていないかどうかを判定する
			bool isHitWallPolygon = false;
			for (int j = 0; j < m_wallNum; j++)
			{
				// j番目の壁ポリゴンのアドレスを壁ポリゴンポインタ配列から取得
				m_pPoly = m_pWallPoly[j];

				// 当たっていたらループから抜ける
				if (HitCheck_Sphere_Triangle(col->rigidbody.GetNextPosVECTOR(), radius,
					m_pPoly->Position[0], m_pPoly->Position[1], m_pPoly->Position[2]))
				{
					//trueにする
					isHitWallPolygon = true;
					break;
				}
			}

			// 全てのポリゴンと当たっていなかったらここでループ終了
			if (!isHitWallPolygon)
			{
				//ヒットフラグを倒す
				m_isHitFlag = false;
				break;
			}
		}
	}
	else
	{
		// 移動していない場合の処理

		// 壁ポリゴンの数だけ繰り返し
		for (int i = 0; i < m_wallNum; i++)
		{
			// i番目の壁ポリゴンのアドレスを壁ポリゴンポインタ配列から取得
			m_pPoly = m_pWallPoly[i];

			// ポリゴンに当たっていたら当たったフラグを立てた上でループから抜ける
			if (HitCheck_Sphere_Triangle(col->rigidbody.GetNextPosVECTOR(), radius,
				m_pPoly->Position[0], m_pPoly->Position[1], m_pPoly->Position[2]))
			{
				m_isHitFlag = true;
				break;
			}
		}
	}

	// 壁に当たっていたら壁から押し出す処理を行う
	if (m_isHitFlag)
	{
		FixPositionWithWallInternal(col);
	}
}

/// <summary>
/// 壁の中から押し出す
/// </summary>
/// <param name="col">チェックするオブジェクト</param>
void MyLib::Physics::FixPositionWithWallInternal(std::shared_ptr<Collidable>& col)
{
	float radius = 0.0f;
	for (auto& col : col->m_colliders)
	{
		radius = dynamic_cast<MyLib::ColliderDataSphere*> (col.get())->m_radius;
	}

	// 壁からの押し出し処理を試みる最大数だけ繰り返し
	for (int i = 0; i < ColInfo::kMaxColHitTryNum; i++)
	{
		// 当たる可能性のある壁ポリゴンを全て見る
		bool isHitWall = false;
		// 壁ポリゴンの数だけ繰り返し
		for (int j = 0; j < m_wallNum; j++)
		{
			// i番目の壁ポリゴンのアドレスを壁ポリゴンポインタ配列から取得
			m_pPoly = m_pWallPoly[j];

			// ポリゴンとプレイヤーが当たっていなかったら次のカウントへ
			if (!HitCheck_Sphere_Triangle(col->rigidbody.GetNextPosVECTOR(), radius,
				m_pPoly->Position[0], m_pPoly->Position[1], m_pPoly->Position[2])) continue;

			auto ret = VAdd(col->rigidbody.GetNextPosVECTOR(), VScale(m_pPoly->Normal, kColHitSlideLength));

			MyLib::Vec3 set;
			set = MyLib::Vec3(ret.x, ret.y, ret.z);

			// 当たっていたら規定距離分プレイヤーを壁の法線方向に移動させる
			col->rigidbody.SetNextPos(set);

			// 移動した上で壁ポリゴンと接触しているかどうかを判定
			for (int k = 0; k < m_wallNum; k++)
			{
				// 当たっていたらループを抜ける
				m_pPoly = m_pWallPoly[k];
				if (HitCheck_Sphere_Triangle(col->rigidbody.GetNextPosVECTOR(), radius,
					m_pPoly->Position[0], m_pPoly->Position[1], m_pPoly->Position[2]))
				{
					isHitWall = true;
					break;
				}
			}

			// 全てのポリゴンと当たっていなかったらここでループ終了
			if (!isHitWall) break;
		}

		//ループ終了
		if (!isHitWall) break;
	}
}

/// <summary>
/// 床ポリゴンとの当たり判定をチェックし、移動させる
/// </summary>
/// <param name="col">チェックするオブジェクト</param>
void MyLib::Physics::FixNowPositionWithFloor(std::shared_ptr<Collidable>& col)
{
	//床ポリゴンがない場合は何もしない
	if (m_floorNum == 0) return;

	float radius = 0.0f;
	for (auto& col : col->m_colliders)
	{
		radius = dynamic_cast<MyLib::ColliderDataSphere*> (col.get())->m_radius;
	}

	// 床ポリゴンとの当たり判定処理
	//あたったかどうかのフラグ初期化
	bool IsHitFlag = false;

	////ジャンプ中かつ上昇中の場合
	//if (dynamic_cast<CharacterBase*>(this)->GetJumpState() && dynamic_cast<CharacterBase*>(this)->GetJumpPower() >= 0.0f)
	//{
	//	// 天井に頭をぶつける処理を行う
	//	// 一番低い天井にぶつける為の判定用変数を初期化
	//	float PolyMinPosY = 0.0f;

	//	// 床ポリゴンの数だけ繰り返し
	//	for (int i = 0; i < m_floorNum; i++)
	//	{
	//		// i番目の床ポリゴンのアドレスを床ポリゴンポインタ配列から取得
	//		m_pPoly = m_pFloorPoly[i];

	//		// 足先から頭の高さまでの間でポリゴンと接触しているかどうかを判定
	//		m_lineRes = HitCheck_Line_Triangle(m_nextPos, VAdd(m_nextPos, VGet(0.0f, kHeadHeight, 0.0f)),
	//			m_pPoly->Position[0], m_pPoly->Position[1], m_pPoly->Position[2]);

	//		// 接触していなかったら何もしない
	//		if (!m_lineRes.HitFlag) continue;

	//		// 天井ポリゴンが今まで検出されたポリゴンより低い場合処理を通す
	//		if (PolyMinPosY < m_lineRes.Position.y)
	//		{
	//			// ポリゴンに当たったフラグを立てる
	//			IsHitFlag = true;

	//			// 接触したＹ座標を保存する
	//			PolyMinPosY = m_lineRes.Position.y;
	//		}
	//	}

	//	// 接触したポリゴンがあれば
	//	if (IsHitFlag)
	//	{
	//		// 接触した場合はプレイヤーのＹ座標を接触座標を元に更新
	//		m_nextPos.y = PolyMinPosY - kHeadHeight;

	//		//地面に当たった時の処理を行う
	//		dynamic_cast<CharacterBase*>(this)->HitGroundUpdate();
	//	}
	//}

	// 床ポリゴンとの当たり判定
	//一番高い床ポリゴンにぶつける為の判定用変数を初期化
	float PolyMaxPosY = 0.0f;

	// 床ポリゴンに当たったかどうかのフラグを倒しておく
	m_isHitFlag = false;

	// 床ポリゴンの数だけ繰り返し
	for (int i = 0; i < m_floorNum; i++)
	{
		// i番目の床ポリゴンのアドレスを床ポリゴンポインタ配列から取得
		m_pPoly = m_pFloorPoly[i];

		// ポリゴンとプレイヤーが当たっていなかったら次のカウントへ
		if (!HitCheck_Sphere_Triangle(col->rigidbody.GetNextPosVECTOR(), radius,
			m_pPoly->Position[0], m_pPoly->Position[1], m_pPoly->Position[2])) continue;

		float mostHeightY = m_pPoly->Position[0].y;

		if (mostHeightY < m_pPoly->Position[1].y)
		{
			mostHeightY = m_pPoly->Position[1].y;
		}

		if (mostHeightY < m_pPoly->Position[2].y)
		{
			mostHeightY = m_pPoly->Position[2].y;
		}

		// 既に当たったポリゴンがあり、且つ今まで検出した床ポリゴンより低い場合は何もしない
		if (m_isHitFlag && PolyMaxPosY > mostHeightY) continue;

		// ポリゴンに当たったフラグを立てる
		m_isHitFlag = true;

		// 接触したＹ座標を保存する
		PolyMaxPosY = mostHeightY;
	}

	// 床ポリゴンの当たり判定かつ、ジャンプ力が0よりも小さい(下降中の場合)どうかで処理を分岐
	if (m_isHitFlag)
	{
		// 接触したポリゴンで一番高いＹ座標をプレイヤーのＹ座標にする
		auto set = col->rigidbody.GetNextPos();
		set.y = PolyMaxPosY + radius;
		col->rigidbody.SetNextPos(set);

		//m_nextPos.y = PolyMaxPosY;
		//dynamic_cast<CharacterBase*>(this)->HitGroundUpdate();

		////ジャンプ力が0よりも小さい(下降中)かつ、ジャンプ中であった場合
		////ジャンプ処理を終了する
		//if (dynamic_cast<CharacterBase*>(this)->GetJumpPower() <= 0.0f &&
		//	dynamic_cast<CharacterBase*>(this)->GetJumpState())
		//{
		//	dynamic_cast<CharacterBase*>(this)->EndJumpState();
		//}
	}

}