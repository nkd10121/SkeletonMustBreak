#include "TitlePlayer.h"
#include "DxLib.h"
#include "ModelManager.h"

namespace
{
	constexpr float kModelSizeScale =80.0f;


	//アニメーションの切り替えにかかるフレーム数
	constexpr float kAnimChangeFrame = 10.0f;
	constexpr float kAnimChangeRateSpeed = 1.0f / kAnimChangeFrame;

	//アニメーションブレンド率の最大
	constexpr float kAnimBlendRateMax = 1.0f;
}

/// <summary>
/// コンストラクタ
/// </summary>
TitlePlayer::TitlePlayer():
	m_currentAnimNo(-1),
	m_prevAnimNo(-1),
	m_animBlendRate(1.0f),
	m_animSpeed(0.5f)
{
	m_modelHandle = ModelManager::GetInstance().GetModelHandle("data/model/player.mv1",true);
	MV1SetScale(m_modelHandle,VECTOR(kModelSizeScale, kModelSizeScale, kModelSizeScale));
	MV1SetRotationXYZ(m_modelHandle, VECTOR(0.0f, DX_PI_F, 0.0f));

	//待機アニメーションを設定
	m_currentAnimNo = MV1AttachAnim(m_modelHandle, 69);
}

/// <summary>
/// デストラクタ
/// </summary>
TitlePlayer::~TitlePlayer()
{
	MV1DeleteModel(m_modelHandle);
}

/// <summary>
/// 初期化
/// </summary>
/// <param name="pos">座標</param>
void TitlePlayer::Init(MyLib::Vec3 pos)
{
	MV1SetPosition(m_modelHandle,pos.ConvertToVECTOR());
}

/// <summary>
/// 更新
/// </summary>
void TitlePlayer::Update()
{
	//アニメーションの切り替え
	if (m_prevAnimNo != -1)
	{
		//フレームでアニメーションを切り替える
		m_animBlendRate += kAnimChangeRateSpeed;
		if (m_animBlendRate >= kAnimBlendRateMax)
		{
			m_animBlendRate = kAnimBlendRateMax;
		}

		//アニメーションのブレンド率を設定する
		MV1SetAttachAnimBlendRate(m_modelHandle, m_prevAnimNo, kAnimBlendRateMax - m_animBlendRate);
		MV1SetAttachAnimBlendRate(m_modelHandle, m_currentAnimNo, m_animBlendRate);
	}

	UpdateAnim(m_currentAnimNo);
}

/// <summary>
/// 描画
/// </summary>
void TitlePlayer::Draw()
{
	MV1DrawModel(m_modelHandle);
}

/// <summary>
/// アニメーションの更新
/// </summary>
/// <param name="attachNo">進行させたいアニメーション番号</param>
/// <returns>ループしたかどうか</returns>
bool TitlePlayer::UpdateAnim(int attachNo, float startTime)
{
	//アニメーションが設定されていなかったら早期リターン
	if (attachNo == -1)	return false;

	//アニメーションを進行させる
	float nowFrame = MV1GetAttachAnimTime(m_modelHandle, attachNo);	//現在の再生カウントを取得
	nowFrame += m_animSpeed;

	//現在再生中のアニメーションの総カウントを取得する
	float totalAnimframe = MV1GetAttachAnimTotalTime(m_modelHandle, attachNo);
	bool isLoop = false;

	//NOTE:もしかしたら総フレーム分引いても総フレームより大きいかもしれないからwhileで大きい間引き続ける
	while (totalAnimframe <= nowFrame)
	{
		//NOTE:nowFrameを0にリセットするとアニメーションフレームの飛びがでるから総フレーム分引く
		nowFrame -= totalAnimframe;
		nowFrame += startTime;
		isLoop = true;
	}

	//進めた時間に設定
	MV1SetAttachAnimTime(m_modelHandle, attachNo, nowFrame);

	return isLoop;
}


/// <summary>
///	アニメーションの変更
/// </summary>
/// <param name="animIndex">変更後のアニメーション番号</param>
void TitlePlayer::ChangeAnim(int animIndex, float animSpeed)
{
	//さらに古いアニメーションがアタッチされている場合はこの時点で消しておく
	if (m_prevAnimNo != -1)
	{
		MV1DetachAnim(m_modelHandle, m_prevAnimNo);
	}

	//現在再生中の待機アニメーションは変更目のアニメーションの扱いにする
	m_prevAnimNo = m_currentAnimNo;

	//変更後のアニメーションとして攻撃アニメーションを改めて設定する
	m_currentAnimNo = MV1AttachAnim(m_modelHandle, animIndex);

	//切り替えの瞬間は変更前のアニメーションが再生される状態にする
	m_animBlendRate = 0.0f;

	m_animSpeed = animSpeed;

	//変更前のアニメーション100%
	MV1SetAttachAnimBlendRate(m_modelHandle, m_prevAnimNo, 1.0f - m_animBlendRate);
	//変更後のアニメーション0%
	MV1SetAttachAnimBlendRate(m_modelHandle, m_currentAnimNo, m_animBlendRate);
}
