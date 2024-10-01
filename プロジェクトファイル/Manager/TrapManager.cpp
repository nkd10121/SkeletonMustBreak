#include "TrapManager.h"
#include "Input.h"
#include "TrapBase.h"
#include "SpikeTrap.h"
#include "CutterTrap.h"
#include "HammerTrap.h"

#include "EffekseerForDXLib.h"
#include "SoundManager.h"
#include "EffectManager.h"

namespace
{
	//㩃��f��
	enum modelIdx :int
	{
		SpikeFrame,
		Spike,
		Cutter,
		Hammer,
		MAX
	};

	//�g���b�v�t�@�C����
	const char* const kFileName[modelIdx::MAX] =
	{
		"data/model/spike_frame.mv1",
		"data/model/spike.mv1",
		"data/model/cutter.mv1",
		"data/model/hammer.mv1",
	};

	//���ꂼ���㩂̃R�X�g
	//TODO:�O���f�[�^��
	constexpr int kSpikeCost = 400;
	constexpr int kCutterCost = 250;
}

/// <summary>
/// �R���X�g���N�^
/// </summary>
/// <param name="trapNum">�g���b�v�̑���</param>
/// <param name="trapPositions">㩐ݒu�\���W</param>
TrapManager::TrapManager(int trapNum, std::list<MyLib::Vec3>& trapPositions) :
	m_modelHandles(),
	m_traps(),
	m_trapPositions(),
	m_previewPos(),
	m_nowSlotIdx(0),
	m_isAlreadyPlaced(false),
	m_angle(0.0f),
	m_transparency(0.0f),
	m_pushZLCount(0)
{
	//���f���n���h�����擾����
	//TODO:���̃X�e�[�W�Ŏg�p����㩂̃��f���݂̂����[�h����悤�ɂ�����
	for (int i = 0; i < trapNum; i++)
	{
		int handle = MV1LoadModel(kFileName[i]);
		m_modelHandles.emplace_back(handle);
	}

	//㩐ݒu�\���W�̎擾
	m_trapPositions = trapPositions;

	//㩐ݒu���̃G�t�F�N�g�����[�h
	EffectManager::GetInstance().Load("CreateTrap", "data/effect/create_trap.efk",120);
}

/// <summary>
/// �f�X�g���N�^
/// </summary>
TrapManager::~TrapManager()
{
	for (auto& trap : m_modelHandles)
	{
		MV1DeleteModel(trap);
	}
	m_modelHandles.clear();

	m_traps.clear();
	m_trapPositions.clear();
}

/// <summary>
/// ������
/// </summary>
void TrapManager::Init()
{
}

/// <summary>
/// �X�V
/// </summary>
/// <param name="input">���͏��</param>
/// <param name="slotNum">���ݑI�����Ă���J�[�\���̃C���f�b�N�X</param>
/// <param name="playerPos">�v���C���[���W</param>
/// <param name="playerVec">�v���C���[�̌���</param>
/// <param name="isPlayerSurvival">�v���C���[�����������ǂ���</param>
/// <param name="trapPoint">�g���b�v�ݒu�|�C���g</param>
/// <param name="nowPhase">���݂̃t�F�[�Y</param>
/// <param name="physics">�����N���X</param>
void TrapManager::Update(std::shared_ptr<Input>& input, int slotNum, MyLib::Vec3 playerPos, MyLib::Vec3 playerVec, bool isPlayerSurvival, int* trapPoint, int nowPhase, std::shared_ptr<MyLib::Physics> physics)
{
	//���݂̃v���C���[�̃X���b�g���擾
	m_nowSlotIdx = slotNum;

	float defaultLength = 100.0f;

	//0�̂Ƃ��̓N���X�{�E�������Ă��邽�߉����������Ȃ�
	if (m_nowSlotIdx != 0)
	{
		m_angle += 0.04f;
		m_transparency = abs(sinf(m_angle) / 2.5f) + 0.1f;

		//�����̎n�_�ƏI�_��ݒ�
		auto start = playerPos;
		auto end = playerPos + playerVec * 30;

		//�ݒu�\�ȃg���b�v�̍��W����
		for (auto pos : m_trapPositions)
		{
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
		if (input->GetIsPushedTriggerButton(true))
		{
			if (m_pushZLCount == 0 && !isPlayerSurvival)
			{

				m_isAlreadyPlaced = false;

				//���łɐݒu���ꂽ㩂̍��W������
				for (auto trap : m_traps)
				{
					//���łɂ��̍��W��㩂��ݒu����Ă�����
					if (trap->GetPos() == m_previewPos)
					{
						//���łɒu����Ă���t���O��true�ɂ���
						m_isAlreadyPlaced = true;

						//�v���C���Ă���l�ɕ�����
						// �u���ł�㩂��u����Ă��܂��v�I�Ȃ̂�\�����Ă�������
					}
				}

				//�u�����Ƃ��Ă�����W��㩂����łɒu����Ă��Ȃ�������
				if (!m_isAlreadyPlaced)
				{
					//true���A���Ă����琬���Afalse���A���Ă����玸�s(�|�C���g�s��)
					if (!CreateTrap(slotNum, physics, trapPoint))
					{
						//�v���C���Ă���l�ɕ�����
						// �u�|�C���g������܂���v�I�Ȃ̂�\�����Ă�������

					}
					else	//�������Ă����炱���ŃG�t�F�N�g���o��
					{
						//�ݒu�G�t�F�N�g�̐���
						EffectManager::GetInstance().CreateEffect("CreateTrap", m_previewPos);
						//�ݒu����炷
						SoundManager::GetInstance().PlaySE("trap");
					}
				}
			}

			m_pushZLCount++;
		}
		else
		{
			m_pushZLCount = 0;
		}
	}
	else
	{
		m_angle = 0.0f;
	}

	//���łɐݒu���ꂽ㩂̍��W������
	for (auto trap : m_traps)
	{
		trap->Update();
	}
}

/// <summary>
/// �`��
/// </summary>
void TrapManager::Draw()
{
	//���łɐݒu���ꂽ㩂̍��W������
	for (auto trap : m_traps)
	{
		trap->Draw();
	}

#ifdef _DEBUG
	//DrawFormatString(1100,600,0xff0000,"%f",m_transparency);
#endif
}

/// <summary>
/// �g���b�v�̂ǂ��ɐݒu���邩���邽�߂̕`��
/// </summary>
void TrapManager::PreviewDraw()
{
	switch (m_nowSlotIdx)
	{
	case 0:
		//0�̎��̓N���X�{�E�������Ă��邽�߉����`�悵�Ȃ�
		break;
	case 1:
		MV1SetPosition(m_modelHandles[modelIdx::SpikeFrame], m_previewPos.ConvertToVECTOR());
		MV1SetScale(m_modelHandles[modelIdx::SpikeFrame], VECTOR(0.08f, 0.08f, 0.08f));
		MV1SetOpacityRate(m_modelHandles[modelIdx::SpikeFrame], m_transparency);

		MV1DrawModel(m_modelHandles[modelIdx::SpikeFrame]);
		break;

	case 2:
		MV1SetPosition(m_modelHandles[modelIdx::Cutter], m_previewPos.ConvertToVECTOR());
		MV1SetScale(m_modelHandles[modelIdx::Cutter], VECTOR(0.08f, 0.08f, 0.08f));
		MV1SetOpacityRate(m_modelHandles[modelIdx::Cutter], m_transparency);

		MV1DrawModel(m_modelHandles[modelIdx::Cutter]);
		break;

	case 3:
		MV1SetPosition(m_modelHandles[modelIdx::Hammer], m_previewPos.ConvertToVECTOR());
		MV1SetScale(m_modelHandles[modelIdx::Hammer], VECTOR(10.0f, 10.0f, 10.0f));
		MV1SetOpacityRate(m_modelHandles[modelIdx::Hammer], m_transparency);

		MV1DrawModel(m_modelHandles[modelIdx::Hammer]);
		break;
	}
}

/// <summary>
/// �g���b�v�̐���
/// </summary>
/// <param name="slotNum">���ݑI�����Ă���X���b�g</param>
/// <param name="physics">�����N���X</param>
/// <param name="trapPoint">㩃|�C���g</param>
/// <returns></returns>
bool TrapManager::CreateTrap(int slotNum, std::shared_ptr<MyLib::Physics> physics, int* trapPoint)
{

	if (slotNum == 1)
	{
		//�ݒu�R�X�g�ȏ��㩃|�C���g�����邩�m�F����
		//����Ă���������㩂�ݒu����
		if (*trapPoint >= kSpikeCost)
		{
			*trapPoint -= kSpikeCost;

			//㩂�ݒu����
			std::shared_ptr<TrapBase> add;
			add = std::make_shared<SpikeTrap>(physics);
			add->SetPos(m_previewPos);
			add->Init(MV1DuplicateModel(m_modelHandles[0]), MV1DuplicateModel(m_modelHandles[1]));
			m_traps.emplace_back(add);

			//�ݒu����
			return true;
		}
		else
		{
			//�|�C���g�s���Őݒu���s
			return false;
		}

	}
	else if (slotNum == 2)
	{
		//�ݒu�R�X�g�ȏ��㩃|�C���g�����邩�m�F����
		//����Ă��Ȃ�������I���
		if (*trapPoint >= kCutterCost)
		{
			//����Ă���������
			*trapPoint -= kCutterCost;

			//㩂�ݒu����
			std::shared_ptr<TrapBase> add;
			add = std::make_shared<CutterTrap>(physics);
			add->SetPos(m_previewPos);
			add->Init(MV1DuplicateModel(m_modelHandles[Cutter]));
			m_traps.emplace_back(add);

			//�ݒu����
			return true;
		}
		else
		{
			//�|�C���g�s���Őݒu���s
			return false;
		}
	}
	else if (slotNum == 3)
	{
		////�ݒu�R�X�g�ȏ��㩃|�C���g�����邩�m�F����
		////����Ă��Ȃ�������I���
		//if (*trapPoint >= 200)
		//{
		//	//����Ă���������
		//	*trapPoint -= 200;

		//	//㩂�ݒu����
		//	std::shared_ptr<TrapBase> add;
		//	add = std::make_shared<HammerTrap>(physics);
		//	add->SetPos(m_previewPos);
		//	add->Init(MV1DuplicateModel(m_modelHandles[Hammer]));
		//	m_traps.emplace_back(add);

		//	//�ݒu����
		//	return true;
		//}
		//else
		//{
		//	//�|�C���g�s���Őݒu���s
		//	return false;
		//}
	}

	return false;

}