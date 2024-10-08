#pragma once
#include "DxLib.h"
#include "MyLib.h"
#include "EnemyBase.h"
#include <map>

/// <summary>
/// 通常の敵
/// </summary>
class Enemy : public EnemyBase
{
public:
	//コンストラクタ
	Enemy();
	//デストラクタ
	virtual ~Enemy();

	//初期化
	void Init(std::shared_ptr<MyLib::Physics>physics, std::vector<MyLib::Vec3> route) override;
	//更新
	void Update(MyLib::Vec3 playerPos, bool isChase);
	//描画
	void Draw();
};