#include "TrapManager.h"
#include "Input.h"
#include "TrapBase.h"
#include "SpikeTrap.h"

namespace
{
	enum modelIdx :int
	{
		SpikeFrame,
		Spike,
		Spring,

		MAX
	};

	//�g���b�v�t�@�C����
	const char* const kFileName[modelIdx::MAX] =
	{
		"data/model/spike_frame.mv1",
		"data/model/spike.mv1",
		"data/model/SpringTrap.mv1",
	};
}

TrapManager::TrapManager(int trapNum, std::list<MyLib::Vec3>& trapPoss):
	m_previewPos(),
	m_slotNum(0)
{
	for (int i = 0; i < trapNum; i++)
	{
		int handle = MV1LoadModel(kFileName[i]);
		m_modelHandles.emplace_back(handle);
	}

	m_trapPoss = trapPoss;
}

TrapManager::~TrapManager()
{
}

void TrapManager::Init()
{
}

void TrapManager::Update(std::shared_ptr<Input>& input, int slotNum,MyLib::Vec3 playerPos, MyLib::Vec3 playerVec, int* trapPoint, int nowPhase, std::shared_ptr<MyLib::Physics> physics)
{
	//���݂̃v���C���[�̃X���b�g���擾
	m_slotNum = slotNum;

	float defaultLength = 100.0f;

	//0�̂Ƃ��̓N���X�{�E�������Ă��邽�߉����������Ȃ�
	if (m_slotNum != 0)
	{
		//�����̎n�_�ƏI�_��ݒ�
		auto start = playerPos;
		auto end = playerPos + playerVec * 20;

		//�ݒu�\�ȃg���b�v�̍��W����
		for (auto pos : m_trapPoss)
		{
			////�v���C���[�̒��S���W�����苗������Ă�����v�Z���Ȃ�
			//if ((pos - start).Size() > 50.0f)
			//{
			//	return;
			//}

			//�����ƃg���b�v�ݒu�\���W�̋������v�Z����
			//float length = Segment_Point_MinLength(start.ConvertToVECTOR(), end.ConvertToVECTOR(), pos.ConvertToVECTOR());
			float length = (pos - end).Size();

			if (defaultLength > length)
			{
				defaultLength = length;
				m_previewPos = pos;
			}

		}

		//�U���{�^�����������Ƃ�
		if (input->GetIsPushedZR())
		{
			//���łɐݒu���ꂽ㩂̍��W������
			for (auto trap : m_traps)
			{
				//���łɂ��̍��W��㩂��ݒu����Ă�����I���
				if (trap->GetPos() == m_previewPos)
				{
					return;
				}
			}

			//�����܂ŗ�����㩂��ݒu����Ă��Ȃ����W��㩂�ݒu���悤�Ƃ��Ă���

			//�ݒu�R�X�g�ȏ��㩃|�C���g�����邩�m�F����
			if (slotNum == 1)
			{
				//����Ă��Ȃ�������I���
				if (*trapPoint < 600)
				{
					return;
				}

				*trapPoint -= 600;
			}


			std::shared_ptr<TrapBase> add;

			if (slotNum == 1)
			{
				add = std::make_shared<SpikeTrap>(physics);
				add->SetDrawPos(m_previewPos);
				add->Init(MV1DuplicateModel(m_modelHandles[0]), MV1DuplicateModel(m_modelHandles[1]));
			}

			m_traps.emplace_back(add);
		}
	}

	//���łɐݒu���ꂽ㩂̍��W������
	for (auto trap : m_traps)
	{
		trap->Update();
	}
}

void TrapManager::Draw()
{
	//���łɐݒu���ꂽ㩂̍��W������
	for (auto trap : m_traps)
	{
		trap->Draw();
	}
}

void TrapManager::PreviewDraw()
{
	switch (m_slotNum)
	{
	case 0:
		//0�̎��̓N���X�{�E�������Ă��邽�߉����`�悵�Ȃ�
		break;
	case 1:
		MV1SetPosition(m_modelHandles[modelIdx::SpikeFrame], m_previewPos.ConvertToVECTOR());
		MV1SetScale(m_modelHandles[modelIdx::SpikeFrame], VECTOR(0.08f, 0.08f, 0.08f));
		MV1SetOpacityRate(m_modelHandles[modelIdx::SpikeFrame], 0.5f);

		MV1DrawModel(m_modelHandles[modelIdx::SpikeFrame]);
		break;

	case 2:

		break;

	case 3:

		break;
	}
}
