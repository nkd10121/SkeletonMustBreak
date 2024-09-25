#pragma once

#include <vector>
#include <string>
#include <map>
#include <memory>

#include "Vec3.h"
#include "DxLib.h"

class EffectManager
{
private:
	//�G�t�F�N�g�\����
	struct Effect
	{
		int handle;
		float x, y, z;
		int frame;
		bool isExist;
	};

	//�G�t�F�N�g�����\����
	struct EffectEmitter
	{
		int emitterHandle = -1;
		const char* effectPath = "";
		std::vector<Effect> effects;
		int endFrame;
	};


private:
	// �V���O���g���p�^�[���Ȃ̂ŃR���X�g���N�^��private�ɒu��
	EffectManager() {};

public:
	virtual ~EffectManager();

	//�R�s�[�R���X�g���N�^������̂̐������ł��Ă��܂�����
	//�R�s�[�R���X�g���N�^���֎~����
	EffectManager(const EffectManager&) = delete;
	EffectManager& operator=(const EffectManager&) = delete;
	EffectManager(EffectManager&&) = delete;
	EffectManager& operator= (const EffectManager&&) = delete;

	/// <summary>
	/// EffectManager��GetInstance()��ʂ����Q�Ƃ��炵�����p�ł��Ȃ�
	/// </summary>
	/// <returns></returns>
	static EffectManager& GetInstance()
	{
		if (!m_instance)
		{
			m_instance = new EffectManager;
		}

		return *m_instance;
	}

	//��������Y���ƕ��ʂɃ��������[�N
	static void Destroy()
	{
		delete m_instance;
		m_instance = nullptr;
	}

public:
	void Load(std::string name, const char* path, int endFrame, float scale = 1.0f);
	void Update();
	void Draw();

	void CreateEffect(std::string name, MyLib::Vec3 pos, MyLib::Vec3 rot = MyLib::Vec3());

	// �w�肵�����O�̂��ׂẴG�t�F�N�g�̍��W���ړ�������
	void SetPos(std::string name, MyLib::Vec3 pos);

	void SetRotation(std::string name, MyLib::Vec3 rot);

private:
	//static�ɂ��邱�Ƃ�
	//Singleton�̃|�C���^���v���O�����N�����Ɉ�����悤�ɂ���
	static EffectManager* m_instance;

private:

	std::map<std::string, std::shared_ptr<EffectEmitter>> m_effect;
};

