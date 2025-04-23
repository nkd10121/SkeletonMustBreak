#pragma once
#include "Vec3.h"

/// <summary>
/// タイトルやステージセレクトの背景プレイヤー
/// TODO:クラス名をBGCharacterとかにしてもいいかも
/// </summary>
class TitlePlayer
{
public:
	//コンストラクタ
	TitlePlayer();
	//デストラクタ
	~TitlePlayer();

	//初期化
	void Init(MyLib::Vec3 pos);
	//更新
	void Update();
	//描画
	void Draw();

private:
	//アニメーションの更新
	bool UpdateAnim(int attachNo, float startTime = 0.0f);
	//アニメーションの変更
	void ChangeAnim(int animIndex, float animSpeed = 0.5f);

private:
	//モデルハンドル
	int m_modelHandle;
	//モデル座標
	MyLib::Vec3 m_pos;

	/*アニメーション情報*/
	//現在のアニメーションナンバー
	int m_currentAnimNo;
	//直前のアニメーションナンバー
	int m_prevAnimNo;
	//アニメーションのブレンド割合
	float m_animBlendRate;
	//アニメーション再生速度
	float m_animSpeed;

};

