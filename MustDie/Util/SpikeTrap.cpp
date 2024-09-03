#include "SpikeTrap.h"

//�O���t�@�C����
//1.�U����
//2.���G�͈�
//3.�U���͈�
//4.�N�[���^�C��
//5.�ݒu�R�X�g
//		��ύX�ł���悤�ɂ�����

SpikeTrap::SpikeTrap(std::shared_ptr<MyLib::Physics> physics):
	TrapBase(GameObjectTag::SpikeTrap),
	m_spikeModelHandle(-1),
	m_frameModelPos(),
	m_spikeModelPos(),
	m_isAttack(false),
	m_isInitCollision(false)
{
	m_pPhysics = physics;

	//�����蔻��̐ݒ�
	auto collider = Collidable::AddCollider(MyLib::ColliderData::Kind::Sphere, true);
	auto sphereCol = dynamic_cast<MyLib::ColliderDataSphere*>(collider.get());
	sphereCol->m_radius = 8.0f;

	m_pSearch = std::make_shared<SearchObject>(7.0f);

	//�U���͂̐ݒ�
	m_attack = 10;
}

SpikeTrap::~SpikeTrap()
{
}

void SpikeTrap::Init(int handle, int subHandle)
{
	m_frameModelPos = m_pos;
	m_spikeModelPos = m_pos;

	m_modelHandle = handle;
	m_spikeModelHandle = subHandle;

	m_frameModelPos.y -= 0.5f;

	MV1SetPosition(m_modelHandle, m_frameModelPos.ConvertToVECTOR());
	MV1SetScale(m_modelHandle, VECTOR(0.08f, 0.08f, 0.08f));

	m_spikeModelPos.y -= 4.5f;

	MV1SetPosition(m_spikeModelHandle, m_spikeModelPos.ConvertToVECTOR());
	MV1SetScale(m_spikeModelHandle, VECTOR(0.08f, 0.08f, 0.08f));

	m_pSearch->Init(m_pPhysics,m_pos);
}

void SpikeTrap::Update()
{
	if (!m_isAttack)
	{
		if (m_pSearch->GetIsTriggerEnemy())
		{
			m_isAttack = true;

			Collidable::Init(m_pPhysics);
			rigidbody.Init();
			rigidbody.SetPos(m_pos);
		}
	}


	if (m_isAttack)
	{
		m_spikeModelPos.y += 2.0f;

		MV1SetPosition(m_spikeModelHandle, m_spikeModelPos.ConvertToVECTOR());

		if (m_spikeModelPos.y >= 10.0f)
		{
			if (!m_isInitCollision)
			{
				Finalize(m_pPhysics);
				m_isInitCollision = true;
			}
		}

		//�X�p�C�N��y���W���N�[���^�C���Ƃ���	����܂�悭�Ȃ�����
		if (m_spikeModelPos.y >= 1000.0f)
		{
			//���ׂĂ�����������
			m_spikeModelPos = m_pos;
			m_spikeModelPos.y -= 4.5f;
			MV1SetPosition(m_spikeModelHandle, m_spikeModelPos.ConvertToVECTOR());
			m_isAttack = false;
			m_isInitCollision = false;
		}
	}
}

void SpikeTrap::Draw()
{
	MV1DrawModel(m_modelHandle);
	MV1DrawModel(m_spikeModelHandle);
}
