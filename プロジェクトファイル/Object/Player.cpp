#include "Player.h"
#include "Input.h"
#include "Shot.h"
#include "WeaponBase.h"
#include "HealPortion.h"
#include <cmath>
#include "CsvLoad.h"
#include "EffectManager.h"
#include "EffekseerForDXLib.h"

#include "SoundManager.h"
#include "ModelManager.h"

#ifdef _DEBUG
#define IsDebugDrawNowAnimName true
#else
#define IsDebugDrawNowAnimName FALSE
#endif
namespace
{
	//モデルサイズ
	constexpr float kModelSizeScale = 4.0f;
	//モデルの中心と当たり判定の中心に差があったため、その距離をもってきた(ModelViewerから)
	constexpr float kModelOffsetY = 1.0f;

	//アニメーションの切り替えにかかるフレーム数
	constexpr float kAnimChangeFrame = 10.0f;
	constexpr float kAnimChangeRateSpeed = 1.0f / kAnimChangeFrame;

	//アニメーションブレンド率の最大
	constexpr float kAnimBlendRateMax = 1.0f;

	//狙うアニメーションの開始フレーム数
	constexpr float kAimAnimStartFrame = 9.0f;

	/*アナログスティックによる移動関連*/
	constexpr float kMaxSpeed = 0.2f;			//プレイヤーの最大速度
	constexpr float kAnalogRangeMin = 0.1f;		//アナログスティックの入力判定範囲
	constexpr float kAnalogRangeMax = 0.8f;
	constexpr float kAnalogInputMax = 1000.0f;	//アナログスティックから入力されるベクトルの最大

	constexpr float kRunVecSize = 0.12f;
	constexpr int kShotIntervalFrame = 20;

	constexpr float kJumpPower = 0.58f;

}

Player::Player() :
	CharacterBase(Collidable::Priority::Low, GameObjectTag::Player),
	//temp_nowAnimIdx(),
	m_moveCount(0),
	m_weponHandle(-1),
	m_angle(0.0f),
	m_isMove(true),
	m_isClear(false),
	m_cameraDirection(),
	m_cameraAngle(0.0f),
	m_updateFunc(nullptr),
	shotTime(0),
	m_shotOffsetPower(0.0f),
	m_isDown(false),
	m_isDead(false),
	m_jumpCount(0),
	m_weponAttachFrameNum(-1),
	m_weponFrameMat(MGetIdent()),
	m_nowSlotIdx(0),
	m_slotNumMax(0)
{
	auto collider = Collidable::AddCollider(MyLib::ColliderData::Kind::Sphere, false);
	auto sphereCol = dynamic_cast<MyLib::ColliderDataSphere*>(collider.get());
	sphereCol->m_radius = 3.4f;

	//モデル読み込み
	m_modelHandle = ModelManager::GetInstance().GetModelHandle("data/model/player.mv1");
	m_weponHandle = ModelManager::GetInstance().GetModelHandle("data/model/crossbow.mv1");
	//モデルのサイズ設定
	MV1SetScale(m_modelHandle, VGet(kModelSizeScale, kModelSizeScale, kModelSizeScale));

	EffectManager::GetInstance().Load("PlayerHit","data/effect/player_hit.efk",120);
	EffectManager::GetInstance().Load("Heal","data/effect/heal.efk",120);

	CsvLoad::GetInstance().AnimDataLoad("Player", m_animIdx);
}

Player::~Player()
{
	//メモリの解放
	MV1DeleteModel(m_modelHandle);
	MV1DeleteModel(m_weponHandle);

	m_pShots.clear();
}

void Player::Init(std::shared_ptr<MyLib::Physics> physics)
{
	m_pPhysics = physics;

	Collidable::Init(m_pPhysics);

	CsvLoad::GetInstance().StatusLoad(m_status, "Player");
	m_hpMax = m_status.hp;


	//プレイヤーの初期位置設定
	rigidbody.Init(true);
	rigidbody.SetPos(MyLib::Vec3(0.0f, 1.0f+kModelOffsetY * kModelSizeScale, 0.0f));
	rigidbody.SetNextPos(rigidbody.GetPos());
	m_collisionPos = rigidbody.GetPos();
	SetModelPos();
	MV1SetPosition(m_modelHandle, m_modelPos.ConvertToVECTOR());
	MV1SetRotationXYZ(m_modelHandle, VGet(0.0f, 0.0f, 0.0f));

	//狙うアニメーションを設定
	m_equipAnimNo = MV1AttachAnim(m_modelHandle, m_animIdx["Aim"]);
	//待機アニメーションを設定
	m_currentAnimNo = MV1AttachAnim(m_modelHandle, m_animIdx["Idle"]);
	m_nowAnimIdx = m_animIdx["Idle"];

	//通常状態に設定しておく
	m_updateFunc = &Player::NeutralUpdate;

	// 物理挙動の初期化
	//rigidbody.Init(true);
	//rigidbody.SetPos(MyLib::Vec3());
}

void Player::Finalize()
{
	Collidable::Finalize(m_pPhysics);
}

void Player::Update(std::shared_ptr<Input>& input)
{
	//アニメーションの更新
	if (!m_isDead)
	{
		UpdateAnim(m_equipAnimNo, kAimAnimStartFrame);

	}
	m_isAnimationFinish = UpdateAnim(m_currentAnimNo);

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

	if (!m_isClear)
	{
		//状態の更新
		(this->*m_updateFunc)(input);
	}

	if (m_status.hp <= 0)
	{
		if (!m_isDown)
		{
			m_isDown = true;

			m_nowAnimIdx = m_animIdx["Death0"];
			ChangeAnim(m_nowAnimIdx);

			Finalize();

			m_updateFunc = &Player::DeathUpdate;

		}
	}

	if (m_shotOffsetPower > 0.0f)
	{
		m_shotOffsetPower -= 0.001f;
	}

	if (m_shotOffsetPower <= 0.0f)
	{
		m_shotOffsetPower = 0.0f;
	}

	if (m_status.hp > 0)
	{
		if (input->IsTriggered("R"))
		{
			m_nowSlotIdx += 1;
			if (m_nowSlotIdx > m_slotNumMax)
			{
				m_nowSlotIdx = 0;
			}
		}

		if (input->IsTriggered("L"))
		{
			m_nowSlotIdx -= 1;
			if (m_nowSlotIdx < 0)
			{
				m_nowSlotIdx = m_slotNumMax;
			}
		}
	}

	//加速度を0にする
	//m_moveVec = MyLib::Vec3();
	//rigidbody.SetVelocity(MyLib::Vec3());

	//座標を取得する
	m_collisionPos = rigidbody.GetPos();

	std::pair<float, float> stick;

	if (!m_isDown && !m_isClear)
	{	//コントローラーの左スティック入力を受け取る
		stick = input->GetInputStick(false);
	}

	//移動方向を設定する
	auto moveVec = MyLib::Vec3(stick.first, 0.0f, -stick.second);
	//移動ベクトルの長さを取得する
	float len = moveVec.Size();

	//移動中に足音のSEを鳴らす
	if (moveVec.Size() != 0.0f)
	{
		if (m_moveCount % 45 == 0)
		{
			SoundManager::GetInstance().PlaySE("walk");
		}
		m_moveCount++;

	}
	else
	{
		m_moveCount = 0;
	}

	//ベクトルの長さを0.0～1.0の割合に変換する
	float rate = len / kAnalogInputMax;

	//アナログスティック無効な範囲を除外する(デッドゾーン)
	rate = (rate - kAnalogRangeMin) / (kAnalogRangeMax - kAnalogRangeMin);
	rate = std::min(rate, 1.0f);
	rate = std::max(rate, 0.0f);

	//速度が決定できるので移動ベクトルに反映する
	moveVec = moveVec.Normalize();
	float speed = m_status.speed * rate;

	moveVec = moveVec * speed;


	// 移動方向 =  カメラの右側  * 右への移動 +   カメラの正面   * 正面への移動
	// 　　　　 = R(0.5, 0, 0.5) *    speed   +  F(-0.5, 0, 0.5) * speed

	//プレイヤーの正面方向を計算して正面方向を基準に移動する
	//カメラの正面方向ベクトル
	MyLib::Vec3 front(m_cameraDirection.x, 0.0f, m_cameraDirection.z);
	//向きベクトル*移動量
	front = front * moveVec.z;
	//カメラの右方向ベクトル
	MyLib::Vec3 right(-m_cameraDirection.z, 0.0f, m_cameraDirection.x);
	//向きベクトル*移動量
	right = right * (-moveVec.x);

	//移動ベクトルの生成
	m_moveVec = front + right;
	m_moveVec = m_moveVec.Normalize() * speed;
	//移動処理
	MV1SetPosition(m_modelHandle, m_collisionPos.ConvertToVECTOR());

	//カメラの座標からプレイヤーを回転させる方向を計算する
	m_angle = -atan2f(m_cameraDirection.z, m_cameraDirection.x) - DX_PI_F / 2;
	m_rot = MyLib::Vec3(0.0f, m_angle, 0.0f);

	//プレイヤーを回転させる
	if (!m_isDown && !m_isClear)
	{
		MV1SetRotationXYZ(m_modelHandle, m_rot.ConvertToVECTOR());
	}
	//m_moveVec.y += kModelOffsetY * kModelSize;

	MyLib::Vec3 prevVelocity = rigidbody.GetVelocity();
	MyLib::Vec3 newVelocity = MyLib::Vec3(m_moveVec.x, prevVelocity.y, m_moveVec.z);
	rigidbody.SetVelocity(newVelocity);


	//もっている武器の移動、回転を更新する
	//TODO:現状後ろからだとよく見えないため違和感ないが、武器の回転がおかしい
	m_weponAttachFrameNum = MV1SearchFrame(m_modelHandle, "handslot.r");
	//auto moveMat = MGetTranslate(VGet(0.0f,-m_pos.y,0.0f));
	m_weponFrameMat = MV1GetFrameLocalWorldMatrix(m_modelHandle, m_weponAttachFrameNum);

	auto offsetMat = MGetTranslate(VGet(0.0f, -kModelOffsetY * kModelSizeScale, 0.0f));

	m_weponFrameMat = MAdd(m_weponFrameMat, offsetMat);
	MATRIX temp = MMult(MGetRotY(-DX_PI_F / 2), m_weponFrameMat);
	MV1SetMatrix(m_weponHandle, temp);

	if (input->GetIsPushedTriggerButton(true) && m_nowSlotIdx == 0 && m_status.hp > 0)
	{
		if (shotTime % kShotIntervalFrame == 0)
		{
			SoundManager::GetInstance().PlaySE("shot");

			auto add = std::make_shared<Shot>();
			add->Init(m_pPhysics);
			MyLib::Vec3 offset = MyLib::Vec3(0.0f, kModelOffsetY * kModelSizeScale / 2, 0.0f);

			auto shotVec = m_cameraDirection;
			int offsetX = GetRand(100) - 50;
			int offsetY = GetRand(100) - 50;
			int offsetZ = GetRand(100) - 50;
			MyLib::Vec3 offsetVec = MyLib::Vec3(static_cast<float>(offsetX), static_cast<float>(offsetY), static_cast<float>(offsetZ));
			offsetVec = offsetVec.Normalize() * m_shotOffsetPower;
			shotVec += offsetVec;

			add->Set(m_collisionPos + offset, shotVec, m_status.atk);
			m_pShots.emplace_back(add);

			m_shotOffsetPower += 0.028f;

			if (m_shotOffsetPower >= 0.1f)
			{
				m_shotOffsetPower = 0.1f;
			}
		}
		shotTime++;
	}
	else
	{
		shotTime = 0;
	}

	m_difAngle = static_cast<int>(m_shotOffsetPower * 100);
	if (m_difAngle < 2)
	{
		m_difAngle = 2;
	}

	for (auto& shot : m_pShots)
	{
		shot->Update();
		if (!shot->GetIsExist())
		{
			//メモリを解放する
			shot->Finalize(m_pPhysics);
			shot.reset();
			shot = nullptr;
		}
	}

	//不要になった敵をここで削除処理する
	auto lIt = remove_if(m_pShots.begin(), m_pShots.end(), [](auto& v) {
		return v == nullptr;
		});
	m_pShots.erase(lIt, m_pShots.end());
}

void Player::Draw()
{
	//FIX:Drawのなかで座標を変更しているのはどうなの？
	rigidbody.SetPos(rigidbody.GetNextPos());
	m_collisionPos = rigidbody.GetPos();
	SetModelPos();
	MV1SetPosition(m_modelHandle, m_modelPos.ConvertToVECTOR());
	//モデルの描画

	MV1DrawModel(m_modelHandle);
	MV1DrawModel(m_weponHandle);

	for (const auto& shot : m_pShots)
	{
		shot->Draw();
	}

	int rad = static_cast<int>(m_shotOffsetPower * 100);
	if (rad == 0)
	{
		rad = 2;
	}

#ifdef _DEBUG
	auto pos = rigidbody.GetPos();
	auto vel = rigidbody.GetVelocity();
	DrawFormatString(420, 112, 0xffffff, "%f", m_shotOffsetPower);
	DrawFormatString(0, 144, 0xffffff, "プレイヤー座標:%f,%f,%f", pos.x, pos.y, pos.z);
	DrawFormatString(0, 128, 0xffffff, "移動ベクトル:%f,%f,%f", vel.x, vel.y, vel.z);
	DrawFormatString(0, 160, 0xffffff, "プレイヤーステータス:%d,%d,%d", m_status.hp, m_status.atk, m_status.def);
#endif
}

void Player::OnCollideEnter(const std::shared_ptr<Collidable>& colider)
{
#ifdef _DEBUG
	std::string message = "プレイヤーが";
#endif
	auto tag = colider->GetTag();
	switch (tag)
	{
	case GameObjectTag::Enemy:
#ifdef _DEBUG
		message += "敵";
#endif
		break;
	case GameObjectTag::Shot:
#ifdef _DEBUG
		message += "弾";
#endif
		break;
	}
#ifdef _DEBUG
	message += "と当たった！\n";
	printfDx(message.c_str());
#endif
}

void Player::OnTriggerEnter(const std::shared_ptr<Collidable>& colider)
{
#ifdef _DEBUG
	std::string message = "プレイヤーが";
#endif
	auto tag = colider->GetTag();
	switch (tag)
	{
	case GameObjectTag::Enemy:
#ifdef _DEBUG
		message += "敵";
#endif
		break;
	case GameObjectTag::Shot:
#ifdef _DEBUG
		message += "弾";
#endif
		break;
	case GameObjectTag::Sword:
	{//なんか{}がないとエラー吐く
		//剣にあたった時HPを減らす
		WeaponBase* col = dynamic_cast<WeaponBase*>(colider.get());
		m_status.hp -= col->GetAtk() - m_status.def;

		//被ダメージエフェクトを出す
		auto pos = rigidbody.GetPos();
		//m_hitEffect->effects.push_back(CreateEffect(m_hitEffect->emitterHandle));
		MyLib::Vec3 offset = MyLib::Vec3(static_cast<float>(GetRand(4) - 2), static_cast<float>(GetRand(4) - 2), static_cast<float>(GetRand(4) - 2));
		pos += offset;

		EffectManager::GetInstance().CreateEffect("PlayerHit",pos);

		SoundManager::GetInstance().PlaySE("playerHit");
	}
	break;
	case GameObjectTag::Portion:

		if (m_hpMax > m_status.hp)
		{
			m_status.hp = m_hpMax;

			auto pos = rigidbody.GetPos();
			EffectManager::GetInstance().CreateEffect("Heal", pos);

			HealPortion* col = dynamic_cast<HealPortion*>(colider.get());
			col->End();

			SoundManager::GetInstance().PlaySE("heal");

		}
		break;


	}
#ifdef _DEBUG
	message += "と当たった！\n";
	printfDx(message.c_str());
#endif
}

void Player::SetGameClear()
{
	if (!m_isClear)
	{
		m_isClear = true;
		m_nowAnimIdx = m_animIdx["Win"];
		ChangeAnim(m_nowAnimIdx);
	}

}

void Player::SetModelPos()
{
	m_modelPos = m_collisionPos;
	m_modelPos.y -= kModelOffsetY * kModelSizeScale*0.8f;
}



/// <summary>
/// 通常(入力なし)状態
/// </summary>
void Player::NeutralUpdate(std::shared_ptr<Input> input)
{
#if IsDebugDrawNowAnimName
	DrawFormatString(0, 32, 0xffffff, "Neutral");
#endif
	m_isMove = true;

	//スティック入力があったら歩き状態に遷移する
	if (m_moveVec.Size() != 0.0f)
	{
		m_updateFunc = &Player::WalkUpdate;
		m_nowAnimIdx = m_animIdx["Move"];
		ChangeAnim(m_nowAnimIdx);

		return;
	}

	//ジャンプボタンを押したら通常ジャンプ状態に遷移する
	if (input->IsTriggered("A"))
	{
		//auto vel = rigidbody.GetVelocity();
		//vel.y += kJumpPower;
		//rigidbody.SetVelocity(vel);
		m_updateFunc = &Player::NormalJumpUpdate;
		m_nowAnimIdx = m_animIdx["Jump"];
		ChangeAnim(m_nowAnimIdx,0.7f);

		return;
	}
}

/// <summary>
/// 通常ジャンプ状態
/// </summary>
void Player::NormalJumpUpdate(std::shared_ptr<Input> input)
{
#if IsDebugDrawNowAnimName
	DrawFormatString(0, 32, 0xffffff, "NormalJump");
#endif
	m_isMove = false;

	m_jumpCount++;

	if (m_jumpCount == 10)
	{
		auto vel = rigidbody.GetVelocity();
		vel.y += kJumpPower;
		rigidbody.SetVelocity(vel);
	}

	if (m_isAnimationFinish)
	{
		m_jumpCount = 0;

		//スティックの入力がなかったら通常状態に遷移する
		if (m_moveVec.Size() == 0.0f)
		{
			m_updateFunc = &Player::NeutralUpdate;
			m_nowAnimIdx = m_animIdx["Idle"];
			ChangeAnim(m_nowAnimIdx);

			return;
		}

		//スティックの入力が小さかったら歩き状態に遷移する
		//if (m_moveVec.Size() < kRunVecSize)
		{
			m_updateFunc = &Player::WalkUpdate;
			m_nowAnimIdx = m_animIdx["Move"];
			ChangeAnim(m_nowAnimIdx);

			return;
		}
	}
}

/// <summary>
/// 歩き状態
/// </summary>
void Player::WalkUpdate(std::shared_ptr<Input> input)
{
#if IsDebugDrawNowAnimName
	DrawFormatString(0, 32, 0xffffff, "Walk");
#endif
	m_isMove = true;

	//スティックの入力がなかったら通常状態に遷移する
	if (m_moveVec.Size() == 0.0f)
	{
		m_updateFunc = &Player::NeutralUpdate;
		m_nowAnimIdx = m_animIdx["Idle"];
		ChangeAnim(m_nowAnimIdx);

		return;
	}

	//ジャンプボタンを押したら移動中ジャンプ状態に遷移する
	if (input->IsTriggered("A"))
	{

		m_updateFunc = &Player::NormalJumpUpdate;
		m_nowAnimIdx = m_animIdx["Jump"];
		ChangeAnim(m_nowAnimIdx, 0.7f);

		SoundManager::GetInstance().PlaySE("jump");
		return;
	}
}

void Player::DeathUpdate(std::shared_ptr<Input> input)
{
	rigidbody.SetVelocity(MyLib::Vec3());

	if (m_isAnimationFinish)
	{
		if (!m_isDead)
		{
			m_isDead = true;
			m_nowAnimIdx = m_animIdx["DeathPose"];
			ChangeAnim(m_nowAnimIdx, 1.0f);
			return;
		}
	}
}
