#pragma once
#include "DxLib.h"
#include "MyLib.h"
#include "EnemyBase.h"
#include <map>

/// <summary>
/// プレイヤーを無視して目的地に移動する敵
/// </summary>
class EnemyFast : public EnemyBase
{
public:
	//コンストラクタ
	EnemyFast();
	//デストラクタ
	virtual ~EnemyFast();

	//初期化
	void Init(std::shared_ptr<MyLib::Physics>physics, std::vector<MyLib::Vec3> route) override;
	//更新
	void Update(MyLib::Vec3 playerPos, bool isChase);
	//描画
	void Draw();
};

