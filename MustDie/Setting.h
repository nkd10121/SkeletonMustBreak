#pragma once
class Setting
{
private:
	// �V���O���g���p�^�[���Ȃ̂ŃR���X�g���N�^��private�ɒu��
	Setting() {};

public:
	virtual ~Setting();

	//�R�s�[�R���X�g���N�^������̂̐������ł��Ă��܂�����
	//�R�s�[�R���X�g���N�^���֎~����
	Setting(const Setting&) = delete;
	Setting& operator=(const Setting&) = delete;
	Setting(Setting&&) = delete;
	Setting& operator= (const Setting&&) = delete;

	/// <summary>
	/// Setting��GetInstance()��ʂ����Q�Ƃ��炵�����p�ł��Ȃ�
	/// </summary>
	/// <returns></returns>
	static Setting& GetInstance()
	{
		if (!m_instance)
		{
			m_instance = new Setting;
		}

		return *m_instance;
	}

	//��������Y���ƕ��ʂɃ��������[�N
	static void Destroy()
	{
		delete m_instance;
		m_instance = nullptr;
	}
private:
	//static�ɂ��邱�Ƃ�
	//Singleton�̃|�C���^���v���O�����N�����Ɉ�����悤�ɂ���
	static Setting* m_instance;

public:
	int m_masterVolume;
	int m_bgmVolume;
	int m_seVolume;

	float m_sensitivity;
	bool m_isFullScreen;
};

