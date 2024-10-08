#pragma once
#include "ObjectBase.h"

/// <summary>
/// ヒールポーションクラス
/// </summary>
class HealPortion : public ObjectBase
{
public:
	//コンストラクタ
	HealPortion();
	//デストラクタ
	virtual ~HealPortion();

	//初期化
	void Init(std::shared_ptr<MyLib::Physics> physics);
	//当たり判定を削除
	void Finalize(std::shared_ptr<MyLib::Physics> physics) override;
	//更新
	void Update();
	//描画
	void Draw();

	//存在しているかどうかを取得
	const bool GetIsExist()const { return m_isExist; }
	//座標を設定
	void SetPosition(MyLib::Vec3 pos);
	//終了させる
	void End();

private:
	//存在フラグ
	bool m_isExist;
	//フレーム数
	int m_frame;

	//角度
	float m_angle;
	//ポーションの動き幅
	float m_posOffsetY;
};

