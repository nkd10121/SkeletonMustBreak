#pragma once
#include "DxLib.h"
#include <string>
#include <vector>
#include <memory>

struct Sound;

/// <summary>
/// サウンド関係を管理するクラス
/// </summary>
class SoundManager
{
private:
	// シングルトンパターンなのでコンストラクタはprivateに置く
	SoundManager() {};

public:
	virtual ~SoundManager();

	//コピーコンストラクタから実体の生成ができてしまうため
	//コピーコンストラクタを禁止する
	SoundManager(const SoundManager&) = delete;
	SoundManager& operator=(const SoundManager&) = delete;
	SoundManager(SoundManager&&) = delete;
	SoundManager& operator= (const SoundManager&&) = delete;

	/// <summary>
	/// SoundManagerはGetInstance()を通した参照からしか利用できない
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

	//これをし忘れると普通にメモリリーク
	static void Destroy()
	{
		delete m_instance;
		m_instance = nullptr;
	}

public:

	/// <summary>
	/// サウンドを読み込む
	/// </summary>
	/// <param name="name">サウンドネーム</param>
	/// <param name="path">サウンドパス</param>
	/// <param name="isBGM">true = BGM,false = SE</param>
	void Load(std::string name, std::string path, bool isBGM);

	/// <summary>
	/// 指定した名前のサウンドを流す
	/// </summary>
	/// <param name="name">流したいサウンドネーム</param>
	/// <param name="isFromStart">true = 最初から,false = 途中から</param>
	void PlayBGM(std::string name, bool isFromStart);

	void PlaySE(std::string name);

	void FadeOutBGM(std::string name, int fadeFrame);

	/// <summary>
	/// 指定した名前のサウンドを止める
	/// </summary>
	/// <param name="name">止めたいサウンドネーム</param>
	void StopBGM(std::string name);

	/// <summary>
	/// eternalフラグがfalseのサウンドを削除する
	/// </summary>
	void Delete();

	/// <summary>
	/// 指定したサウンドが流れているどうか
	/// </summary>
	/// <param name="name">サウンドネーム</param>
	/// <returns>true = 流れている,false = 流れていない</returns>
	bool isPlayingSound(std::string name);

	void ChangeBGMVolume(float volume);
	void ChangeSEVolume(float volume);


private:
	/// <summary>
	/// 指定したハンドルのサウンドが流れているかどうか
	/// </summary>
	/// <param name="handle">チェックしたいハンドル</param>
	/// <returns>true = 流れている,false = 流れていない</returns>
	bool CheckPlaying(int handle) { return CheckSoundMem(handle); }

private:
	//staticにすることで
	//Singletonのポインタがプログラム起動時に一つ作られるようにする
	static SoundManager* m_instance;

private:
	std::vector<std::shared_ptr<Sound>> m_BGM;
	std::vector<std::shared_ptr<Sound>> m_SE;

	float m_BGMvolume;
	float m_SEvolume;

};

