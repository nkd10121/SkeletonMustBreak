#pragma once
class Setting
{
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
private:
	//staticにすることで
	//Singletonのポインタがプログラム起動時に一つ作られるようにする
	static Setting* m_instance;

public:
	int m_masterVolume;
	int m_bgmVolume;
	int m_seVolume;

	float m_sensitivity;
	bool m_isFullScreen;
};

