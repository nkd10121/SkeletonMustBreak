#pragma once

/// <summary>
/// オプションを管理するクラス
/// </summary>
class Setting
{
private:
	//オプションデータ
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
	//デストラクタ
	virtual ~Setting();

public:
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
	//BGMボリュームを設定する
	void SetBGMVolume(float volume) { m_data.bgmVolume = volume; }
	//SEボリュームを設定する
	void SetSEVolume(float volume) { m_data.seVolume = volume; }
	//感度を設定する
	void SetSensitivity(float sensitivity) { m_data.sensitivity = sensitivity; }
	//フルスクリーンを設定する
	void SetIsFullScreen(bool isFullScreen) { m_data.isFullScreen = isFullScreen; }

	//BGMボリュームを取得する
	const float GetBGMVolume()const { return m_data.bgmVolume; }
	//SEボリュームを取得する
	const float GetSEVolume()const { return m_data.seVolume; }
	//感度を取得する
	const float GetSensitivity()const { return m_data.sensitivity; }
	//フルスクリーンを取得する
	const bool GetIsFullScreen()const { return !m_data.isFullScreen; }

	//オプションデータを読み込む
	void Load();
	//オプションデータを保存する
	void Save();

private:
	//オプションデータを全消去する
	void ClearData();
	//新しいオプションデータを生成する
	void CreateNewData();

private:
	//staticにすることで
	//Singletonのポインタがプログラム起動時に一つ作られるようにする
	static Setting* m_instance;

public:
	//オプションデータ
	Data m_data;
};

