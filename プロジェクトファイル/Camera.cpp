#include "Camera.h"
#include "Input.h"

#include <cmath>
#include "Setting.h"

namespace
{
	constexpr float kCameraDist = 24.0f;
	constexpr float kCameraHeight = 18.0f;

	constexpr float kCameraNear = 3.0f;
	constexpr float kCameraFar = 360.0f;

	constexpr float kCameraRadius = kCameraNear + 0.2f;

	constexpr float kAngleMoveScaleMax = 3.2f;

	constexpr float kAnalogInputMax = 1000.0f;	//アナログスティックから入力されるベクトルの最大

	// カメラの旋回スピード
	constexpr float kCameraAngleSpeedX = 0.1f;
	constexpr float kCameraAngleSpeedY = 0.05f;

	//カメラの角度制限
	constexpr float kCameraAngleVLimitMin = -DX_PI_F / 2.0f + 0.6f;
	constexpr float kCameraAngleVLimitMax = DX_PI_F / 2.0f - 0.6f;

	// カメラからプレイヤーまでの最大距離
	constexpr float kCameraToPlayerLenghtMax = 175.0f;
}

/// <summary>
/// コンストラクタ
/// </summary>
Camera::Camera() :
	m_cameraAngleX(0.0f),
	m_cameraAngleY(0.0f),
	m_cameraPos(),
	m_aimPos(),
	m_playerPos(),
	m_angleMoveScale(kAngleMoveScaleMax),
	m_lightHandle(-1),
	m_isClear(false),
	m_hitDim()
{
	m_lightHandle = CreateDirLightHandle((m_aimPos - m_cameraPos).ConvertToVECTOR());
}

/// <summary>
/// デストラクタ
/// </summary>
Camera::~Camera()
{
	DeleteLightHandle(m_lightHandle);
}

/// <summary>
/// 初期化
/// </summary>
void Camera::Init()
{
	m_cameraAngleX = 0.0f;
	m_cameraAngleY = 12.0f;
	SetCameraNearFar(kCameraNear, kCameraFar);
}

/// <summary>
/// 更新
/// </summary>
/// <param name="input">入力管理クラスの参照</param>
/// <param name="stageHandle">カメラと当たり判定をとるステージのモデルハンドル</param>
void Camera::Update(std::shared_ptr<Input>& input, int stageHandle)
{
	m_angleMoveScale = kAngleMoveScaleMax * Setting::GetInstance().GetSensitivity();
	//右スティックの入力をもらう
	std::pair<float,float> rightStickVec;
	if (!m_isClear)
	{
		rightStickVec = input->GetInputStick(true);
	}

	//入力から角度を計算する
	float inputRateX = rightStickVec.first / kAnalogInputMax;
	float inputRateY = rightStickVec.second / kAnalogInputMax;

	if (inputRateX > 0.001f)
	{
		m_cameraAngleX -= m_angleMoveScale * std::abs(inputRateX);
	}
	else if (inputRateX < -0.001f)
	{
		m_cameraAngleX += m_angleMoveScale * std::abs(inputRateX);
	}

	if (inputRateY > 0.001f)
	{
		m_cameraAngleY += m_angleMoveScale * std::abs(inputRateY);
		if (m_cameraAngleY > 90.0f)
		{
			m_cameraAngleY = 89.99f;
		}
	}
	else if (inputRateY < -0.001f)
	{
		m_cameraAngleY -= m_angleMoveScale * std::abs(inputRateY);
		if (m_cameraAngleY < -90.0f)
		{
			m_cameraAngleY = -89.99f;
		}
	}


	// カメラの位置はカメラの水平角度と垂直角度から算出
	// 最初に垂直角度を反映した位置を算出
	MyLib::Vec3 tempPos1;
	float sinParam = sinf(m_cameraAngleY / 180.0f * DX_PI_F);
	float cosParam = cosf(m_cameraAngleY / 180.0f * DX_PI_F);
	tempPos1.x = 0.0f;
	tempPos1.y = sinParam * kCameraDist;
	tempPos1.z = -cosParam * kCameraDist;

	// 次に水平角度を反映した位置を算出
	MyLib::Vec3 tempPos2;
	sinParam = sinf(m_cameraAngleX / 180.0f * DX_PI_F);
	cosParam = cosf(m_cameraAngleX / 180.0f * DX_PI_F);
	tempPos2.x = cosParam * tempPos1.x - sinParam * tempPos1.z;
	tempPos2.y = tempPos1.y;
	tempPos2.z = sinParam * tempPos1.x + cosParam * tempPos1.z;

	m_aimPos = MyLib::Vec3(m_playerPos.x, m_playerPos.y + 9.0f, m_playerPos.z);

	// 算出した座標に注視点の位置を加算したものがカメラの位置になる
	m_cameraPos = tempPos2 + m_aimPos;

	// 最初はステージ自体と判定
	m_hitDim = MV1CollCheck_Capsule(stageHandle, -1, m_aimPos.ConvertToVECTOR(), m_cameraPos.ConvertToVECTOR(), kCameraRadius);

	// 検出した周囲のポリゴン情報を開放する
	MV1CollResultPolyDimTerminate(m_hitDim);

	// ステージのポリゴンは周囲に無かったら今度はコリジョンオブジェクトのポリゴンが周囲にあるか調べる
	if (m_hitDim.HitNum == 0)
	{
		// 検出した周囲のポリゴン情報を開放する
		MV1CollResultPolyDimTerminate(m_hitDim);
	}
	else if (m_hitDim.HitNum != 0)
	{
		bool doCheck = true;

		while (doCheck)
		{

			doCheck = false;

			//プレイヤーの座標からカメラの移動予定後座標の方向ベクトルを計算する
			auto playerToCamera = (m_cameraPos - m_aimPos);

			//向きと大きさに分ける
			auto vec = playerToCamera.Normalize();
			auto length = playerToCamera.Size();

			//距離を縮める
			length *= 0.998f;

			auto checkPos = m_aimPos + vec * length;

			// 最初はステージ自体と判定
			m_hitDim = MV1CollCheck_Capsule(stageHandle, -1, m_aimPos.ConvertToVECTOR(), checkPos.ConvertToVECTOR(), kCameraRadius);
			MV1CollResultPolyDimTerminate(m_hitDim);

			if (m_hitDim.HitNum != 0)
			{
				m_cameraPos = checkPos;
				doCheck = true;
			}
			else
			{
				doCheck = false;
			}
			// HitLength と NoHitLength が十分に近づいていなかったらループ
		}
	}

	SetLightDirectionHandle(m_lightHandle, (m_aimPos - m_cameraPos).ConvertToVECTOR());

	SetCameraPositionAndTarget_UpVecY(m_cameraPos.ConvertToVECTOR(), m_aimPos.ConvertToVECTOR());
}

#ifdef _DEBUG
/// <summary>
/// デバッグ描画
/// </summary>
void Camera::DebugDraw()
{
	DrawFormatString(640, 460, 0xffffff, "%f,%f,%f", m_cameraPos.x, m_cameraPos.y, m_cameraPos.z);
}
#endif

/// <summary>
/// カメラが向いている方向ベクトルを取得
/// </summary>
/// <returns>カメラが向いている方向ベクトル</returns>
const MyLib::Vec3 Camera::GetDirection() const
{
	return (m_aimPos - m_cameraPos).Normalize();
}