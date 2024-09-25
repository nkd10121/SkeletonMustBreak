#include "EffectManager.h"
#include "EffekseerForDXLib.h"

#include <cassert>

EffectManager* EffectManager::m_instance = nullptr;

EffectManager::~EffectManager()
{
	//�܂����łɓ����p�X�̃G�t�F�N�g�����[�h����Ă��Ȃ����m�F����
	for (auto& effect : m_effect)
	{
		DeleteEffekseerEffect(effect.second->emitterHandle);
	}
}

void EffectManager::Load(std::string name, const char* path, int endFrame, float scale)
{
	//�܂����łɓ����p�X�̃G�t�F�N�g�����[�h����Ă��Ȃ����m�F����
	for (auto& effect : m_effect)
	{
		if (effect.second->effectPath == path)
		{
			return;
		}
	}

	//�����܂ŗ�����G�t�F�N�g�����[�h����
	std::shared_ptr<EffectEmitter> add = std::make_shared<EffectEmitter>();
	add->emitterHandle = LoadEffekseerEffect(path, scale);
	assert(add->emitterHandle != -1 && "�G�t�F�N�g���[�h���s");
	add->endFrame = endFrame;

	m_effect[name] = add;

}

void EffectManager::Update()
{
	UpdateEffekseer3D();

	//�G�t�F�N�g�̍X�V
	for (auto& effects : m_effect)
	{
		for (auto& ef : effects.second->effects)
		{
			if (ef.frame > effects.second->endFrame)
			{
				StopEffekseer3DEffect(ef.handle);
				ef.isExist = false;
			}

			ef.frame++;
		}

		//isExist��false�̃I�u�W�F�N�g���폜
		auto it = std::remove_if(effects.second->effects.begin(), effects.second->effects.end(), [](auto& v)
			{
				return v.isExist == false;
			});
		effects.second->effects.erase(it, effects.second->effects.end());
	}
}

void EffectManager::Draw()
{
	DrawEffekseer3D();
}

void EffectManager::CreateEffect(std::string name, MyLib::Vec3 pos, MyLib::Vec3 rot)
{
	int handle = -1;
	for (auto& effects : m_effect)
	{
		if (effects.first == name)
		{
			handle = effects.second->emitterHandle;
		}
	}

	//�����ǉ����悤�Ƃ��Ă���G�t�F�N�g�̖��O�̃n���h�����Ȃ���ΏI���
	if (handle == -1)
	{
		return;
	}

	Effect add;
	add.handle = PlayEffekseer3DEffect(handle);
	add.frame = 0;
	add.x = pos.x;
	add.y = pos.y;
	add.z = pos.z;
	add.isExist = true;

	SetPosPlayingEffekseer3DEffect(add.handle, add.x, add.y, add.z);
	SetRotationPlayingEffekseer3DEffect(add.handle, rot.x, rot.y, rot.z);

	m_effect[name]->effects.emplace_back(add);

	return;
}

/// <summary>
/// �w�肵�����O�̂��ׂẴG�t�F�N�g�̍��W���ړ�������
/// </summary>
/// <param name="name">�ړ����������G�t�F�N�g�̖��O</param>
/// <param name="pos">�ړ�����W</param>
void EffectManager::SetPos(std::string name, MyLib::Vec3 pos)
{
	std::shared_ptr<EffectEmitter> emit;

	for (auto& effects : m_effect)
	{
		if (effects.first == name)
		{
			for (auto& ef : effects.second->effects)
			{
				ef.x = pos.x;
				ef.y = pos.y;
				ef.z = pos.z;

				SetPosPlayingEffekseer3DEffect(ef.handle, ef.x, ef.y, ef.z);

			}
		}
	}

	return;
}

void EffectManager::SetRotation(std::string name, MyLib::Vec3 rot)
{
	for (auto& effects : m_effect)
	{
		if (effects.first == name)
		{
			for (auto& ef : effects.second->effects)
			{
				SetRotationPlayingEffekseer3DEffect(ef.handle,rot.x,rot.y,rot.z);
			}
		}
	}
}
