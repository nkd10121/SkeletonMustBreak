#include "SpikeTrap.h"
#include "SoundManager.h"
#include "ModelManager.h"
#include "CsvLoad.h"

namespace
{
	//トラップ名
	const char* kTrapName = "Spike";

	//モデルのパス
	const char* kFramePath = "data/model/spike_frame.mv1";
	const char* kSpikePath = "data/model/spike.mv1";
}

SpikeTrap::SpikeTrap(std::shared_ptr<MyLib::Physics> physics):
	TrapBase(GameObjectTag::SpikeTrap),
	m_frameModelPos(),
	m_spikeModelPos(),
	m_isAttack(false),
	m_isInitCollision(false),
	m_attackCount(0)
{
	m_pPhysics = physics;

	//ステータスの取得
	status = CsvLoad::GetInstance().TrapStatusLoad(kTrapName);

	//当たり判定の設定
	auto collider = Collidable::AddCollider(MyLib::ColliderData::Kind::Sphere, true);
	auto sphereCol = dynamic_cast<MyLib::ColliderDataSphere*>(collider.get());
	sphereCol->m_radius = status.atkRange;

	m_pSearch = std::make_shared<SearchObject>(status.searchRange);

	//攻撃力の設定
	m_attack = status.atk;
}

SpikeTrap::~SpikeTrap()
{


}

void SpikeTrap::Init()
{
	//存在している状態にする
	m_isExist = true;

	m_frameModelPos = m_pos;
	m_spikeModelPos = m_pos;

	m_modelHandle = ModelManager::GetInstance().GetModelHandle(kFramePath);
	m_spikeModelHandle = ModelManager::GetInstance().GetModelHandle(kSpikePath);

	m_frameModelPos.y -= 0.5f;

	MV1SetPosition(m_modelHandle, m_frameModelPos.ConvertToVECTOR());
	MV1SetScale(m_modelHandle, VECTOR(0.08f, 0.08f, 0.08f));

	m_spikeModelPos.y -= 4.5f;

	MV1SetPosition(m_spikeModelHandle, m_spikeModelPos.ConvertToVECTOR());
	MV1SetScale(m_spikeModelHandle, VECTOR(0.08f, 0.08f, 0.08f));

	m_pSearch->Init(m_pPhysics,m_pos);
}

GameObjectTag SpikeTrap::Finalize()
{
	MV1DeleteModel(m_modelHandle);
	MV1DeleteModel(m_spikeModelHandle);
	m_pSearch->Finalize(m_pPhysics);

	return Collidable::GetTag();
}

void SpikeTrap::Update()
{
	//存在していない状態なら何もさせない
	if (!m_isExist)return;

	if (!m_isAttack)
	{
		if (m_pSearch->GetIsTrigger())
		{

			m_isAttack = true;

			Collidable::Init(m_pPhysics);
			rigidbody.Init();
			rigidbody.SetPos(m_pos);
		}
	}


	if (m_isAttack)
	{
		if (m_attackCount == 1)
		{
			SoundManager::GetInstance().PlaySE("spike");
		}

		if (m_attackCount == 0 && m_spikeModelPos.y < 4.0f)
		{
			m_spikeModelPos.y += 0.8f;
		}
		else
		{
			m_attackCount++;
		}

		MV1SetPosition(m_spikeModelHandle, m_spikeModelPos.ConvertToVECTOR());

		if (m_attackCount == 30)
		{
			if (!m_isInitCollision)
			{
				Collidable::Finalize(m_pPhysics);
				m_isInitCollision = true;
			}
		}

		if (m_attackCount > 30)
		{
			m_spikeModelPos.y -= 0.8f;

			if (m_spikeModelPos.y < -4.5f)
			{
				m_spikeModelPos.y = -4.5f;
			}
		}

		if (m_attackCount >= status.coolTime)
		{
			//すべてを初期化する
			m_spikeModelPos = m_pos;
			m_spikeModelPos.y -= 4.5f;
			MV1SetPosition(m_spikeModelHandle, m_spikeModelPos.ConvertToVECTOR());
			m_isAttack = false;
			m_isInitCollision = false;

			//当たり判定をリセット
			m_pSearch->IsTriggerReset();

			m_attackCount = 0;
		}
	}
}

void SpikeTrap::Draw()
{
	//存在していない状態なら何もさせない
	if (!m_isExist)return;

	MV1DrawModel(m_modelHandle);
	MV1DrawModel(m_spikeModelHandle);
}
