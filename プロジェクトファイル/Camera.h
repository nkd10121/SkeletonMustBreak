#pragma once
#include "DxLib.h"
#include "Vec3.h"
#include <memory>

class Input;

/// <summary>
/// カメラクラス
/// </summary>
class Camera
{
public:
	//コンストラクタ
	Camera();
	//デストラクタ
	~Camera();

	//初期化
	void Init();
	//更新
	void Update(std::shared_ptr<Input>& input,int stageHandle);

#ifdef _DEBUG
	//デバッグ描画
	void DebugDraw();
#endif

	//カメラが向いている方向ベクトルを取得
	const MyLib::Vec3 GetDirection()const;

	//プレイヤーの座標を設定する
	void SetPlayerPos(MyLib::Vec3 playerPos) { m_playerPos = playerPos; }

	//クリアフラグを設定する
	void SetClear() { m_isClear = true; }

private:
	//水平角度
	float m_cameraAngleX;
	//垂直角度
	float m_cameraAngleY;
	//カメラの座標
	MyLib::Vec3 m_cameraPos;
	//注視点
	MyLib::Vec3 m_aimPos;
	//プレイヤーの座標
	MyLib::Vec3 m_playerPos;
	//視点移動速度
	float m_angleMoveScale;
	//ライトハンドル
	int m_lightHandle;
	//クリアフラグ
	bool m_isClear;
	//当たり判定結果構造体
	MV1_COLL_RESULT_POLY_DIM m_hitDim{};	
};

