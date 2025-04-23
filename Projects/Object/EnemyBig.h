#pragma once
#include "EnemyBase.h"

/// <summary>
/// 大きい敵
/// </summary>
class EnemyBig : public EnemyBase
{
public:
	//コンストラクタ
	EnemyBig();
	//デストラクタ
	virtual ~EnemyBig();

	//初期化
	void Init(std::shared_ptr<MyLib::Physics>physics, std::vector<MyLib::Vec3> route) override;
	//更新
	void Update(MyLib::Vec3 playerPos, bool isChase);
	//描画
	void Draw();
};

