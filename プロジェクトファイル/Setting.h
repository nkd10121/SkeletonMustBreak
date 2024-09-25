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
	// シングルトンパターンなのでコンストラクタはprivateに置く
	Setting() {};

public:
	virtual ~Setting();

	//コピーコンストラクタから実体の生成ができてしまうため
	//コピーコンストラクタを禁止する
	Setting(const Setting&) = delete;
	Setting& operator=(const Setting&) = delete;
	Setting(Setting&&) = delete;
	Setting& operator= (const Setting&&) = delete;

	/// <summary>
	/// SettingはGetInstance()を通した参照からしか利用できない
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

	//これをし忘れると普通にメモリリーク
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
	//staticにすることで
	//Singletonのポインタがプログラム起動時に一つ作られるようにする
	static Setting* m_instance;

public:
	Data m_data;
};

