#pragma once
class Setting
{
private:
	struct Data
	{
		float bgmVolume = 0.5f;
		float seVolume = 0.5f;
		float sensitivity = 0.5f;
		bool isFullScreen = false;
	};
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

public:
	void SetBGMVolume(float volume) { m_data.bgmVolume = volume; }
	void SetSEVolume(float volume) { m_data.seVolume = volume; }
	void SetSensitivity(float sensitivity) { m_data.sensitivity = sensitivity; }
	void SetIsFullScreen(bool isFullScreen) { m_data.isFullScreen = isFullScreen; }

	const float GetBGMVolume()const { return m_data.bgmVolume; }
	const float GetSEVolume()const { return m_data.seVolume; }
	const float GetSensitivity()const { return m_data.sensitivity; }
	const bool GetIsFullScreen()const { return !m_data.isFullScreen; }

	void Load();
	void Save();

private:
	void ClearData();

	void CreateNewData();

private:
	//static�ɂ��邱�Ƃ�
	//Singleton�̃|�C���^���v���O�����N�����Ɉ�����悤�ɂ���
	static Setting* m_instance;

public:
	Data m_data;
};

