#pragma once
#include "DxLib.h"
#include <string>
#include <vector>

struct Sound;

class SoundManager
{
private:
	// �V���O���g���p�^�[���Ȃ̂ŃR���X�g���N�^��private�ɒu��
	SoundManager() {};

public:
	virtual ~SoundManager();

	//�R�s�[�R���X�g���N�^������̂̐������ł��Ă��܂�����
	//�R�s�[�R���X�g���N�^���֎~����
	SoundManager(const SoundManager&) = delete;
	SoundManager& operator=(const SoundManager&) = delete;
	SoundManager(SoundManager&&) = delete;
	SoundManager& operator= (const SoundManager&&) = delete;

	/// <summary>
	/// SoundManager��GetInstance()��ʂ����Q�Ƃ��炵�����p�ł��Ȃ�
	/// </summary>
	/// <returns></returns>
	static SoundManager& GetInstance()
	{
		if (!m_instance)
		{
			m_instance = new SoundManager;
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

	/// <summary>
	/// �T�E���h��ǂݍ���
	/// </summary>
	/// <param name="name">�T�E���h�l�[��</param>
	/// <param name="path">�T�E���h�p�X</param>
	/// <param name="isBGM">true = BGM,false = SE</param>
	void Load(std::string name, std::string path, bool isBGM);

	/// <summary>
	/// �w�肵�����O�̃T�E���h�𗬂�
	/// </summary>
	/// <param name="name">���������T�E���h�l�[��</param>
	/// <param name="isFromStart">true = �ŏ�����,false = �r������</param>
	void PlayBGM(std::string name, bool isFromStart);

	void PlaySE(std::string name);

	void FadeOutBGM(std::string name, int fadeFrame);

	/// <summary>
	/// �w�肵�����O�̃T�E���h���~�߂�
	/// </summary>
	/// <param name="name">�~�߂����T�E���h�l�[��</param>
	void StopBGM(std::string name);

	/// <summary>
	/// eternal�t���O��false�̃T�E���h���폜����
	/// </summary>
	void Delete();

	/// <summary>
	/// �w�肵���T�E���h������Ă���ǂ���
	/// </summary>
	/// <param name="name">�T�E���h�l�[��</param>
	/// <returns>true = ����Ă���,false = ����Ă��Ȃ�</returns>
	bool isPlayingSound(std::string name);

	void ChangeBGMVolume(int volume);
	void ChangeSEVolume(int volume);


private:
	/// <summary>
/// �w�肵���n���h���̃T�E���h������Ă��邩�ǂ���
/// </summary>
/// <param name="handle">�`�F�b�N�������n���h��</param>
/// <returns>true = ����Ă���,false = ����Ă��Ȃ�</returns>
	bool CheckPlaying(int handle) { return CheckSoundMem(handle); }

private:
	//static�ɂ��邱�Ƃ�
	//Singleton�̃|�C���^���v���O�����N�����Ɉ�����悤�ɂ���
	static SoundManager* m_instance;

private:
	vector<Sound*> m_BGM;
	vector<Sound*> m_SE;

	float m_BGMvolume = 0.5f;
	float m_SEvolume = 0.5f;

};

