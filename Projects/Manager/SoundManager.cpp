#include "SoundManager.h"

SoundManager* SoundManager::m_instance = nullptr;

namespace
{
	constexpr float kFadeFrame = 30.0f;
}

struct Sound
{
private:
	std::string SoundName = "";
	std::string SoundPath = "";
	int SoundHandle = -1;

public:

	void Set(std::string name, std::string path)
	{
		SoundName = name;
		SoundPath = path;
		SoundHandle = LoadSoundMem(path.c_str());
	}

	std::string GetName()const
	{
		return SoundName;
	}

	std::string GetPath()const
	{
		return SoundPath;
	}

	int GetHandle()const
	{
		return SoundHandle;
	}

	void DeleteSound()
	{
		DeleteSoundMem(SoundHandle);
	}
};

SoundManager::~SoundManager()
{
	for (auto& bgm : m_BGM)
	{
		bgm->DeleteSound();
	}

	for (auto& se : m_SE)
	{
		se->DeleteSound();
	}
	InitSoundMem();
}

void SoundManager::Load(std::string name, std::string path, bool isBGM)
{
	//BGMフラグがtrueの時は、BGMのvectorに入れる
	if (isBGM)
	{
		m_BGM.resize(m_BGM.size() + 1);

		for (int i = 0; i < m_BGM.size(); i++)
		{
			//存在していて
			if (m_BGM[i])
			{
				//パスが同じ場合は増やした枠を減らして返す
				if (m_BGM[i]->GetPath() == path)
				{
					m_BGM.pop_back();
					return;
				}
			}
			//パスが同じものが存在していない場合、ロードをする
			else
			{
				m_BGM[i] = std::make_shared<Sound>();
				m_BGM[i]->Set(name, path);
				ChangeVolumeSoundMem(static_cast<int>(255 * m_BGMvolume), m_BGM[i]->GetHandle());
			}
		}
	}
	//BGMフラグがfalseの時は、SEのvectorに入れる
	else
	{
		m_SE.resize(m_SE.size() + 1);

		for (int i = 0; i < m_SE.size(); i++)
		{
			//存在していて
			if (m_SE[i])
			{
				//パスが同じ場合は増やした枠を減らして返す
				if (m_SE[i]->GetPath() == path)
				{
					m_SE.pop_back();
					return;
				}
			}
			//パスが同じものが存在していない場合、ロードをする
			else
			{
				m_SE[i] = std::make_shared<Sound>();
				m_SE[i]->Set(name, path);
				ChangeVolumeSoundMem(static_cast<int>(255 * m_SEvolume), m_SE[i]->GetHandle());
			}
		}
	}
}

void SoundManager::PlayBGM(std::string name, bool isFromStart)
{
	for (int i = 0; i < m_BGM.size(); i++)
	{
		//存在しているかつ、引数の名前と一致する名前
		if (m_BGM[i] && m_BGM[i]->GetName() == name)
		{
			//流れていたら何もしない
			if (CheckPlaying(m_BGM[i]->GetHandle()))
			{
				return;
			}

			PlaySoundMem(m_BGM[i]->GetHandle(), DX_PLAYTYPE_BACK, isFromStart);
			return;
		}
	}
}

void SoundManager::PlaySE(std::string name)
{
	for (int i = 0; i < m_SE.size(); i++)
	{
		//存在しているかつ、引数の名前と一致する名前
		if (m_SE[i] && m_SE[i]->GetName() == name)
		{
			PlaySoundMem(m_SE[i]->GetHandle(), DX_PLAYTYPE_BACK, true);
			return;
		}
	}
}

void SoundManager::FadeOutBGM(std::string name, int fadeFrame)
{
	float dif = m_BGMvolume * ((kFadeFrame - static_cast<float>(fadeFrame)) / kFadeFrame);
	for (int i = 0; i < m_BGM.size(); i++)
	{
		//存在しているかつ、引数の名前と一致する名前
		if (m_BGM[i] && m_BGM[i]->GetName() == name)
		{
			//BGMvolume = dif;
			ChangeVolumeSoundMem(static_cast<int>(255 * dif), m_BGM[i]->GetHandle());

			if (255 * dif <= 0.0f)
			{
				StopSoundMem(m_BGM[i]->GetHandle());
			}
			return;
		}
	}
	return;
}

void SoundManager::StopBGM(std::string name)
{
	for (int i = 0; i < m_BGM.size(); i++)
	{
		//存在しているかつ、引数の名前と一致する名前
		if (m_BGM[i] && m_BGM[i]->GetName() == name)
		{
			int tempHandle = m_BGM[i]->GetHandle();

			//流れていなかったら何もしない
			if (!CheckPlaying(tempHandle))
			{
				return;
			}

			StopSoundMem(tempHandle);

			return;
		}
	}
}

void SoundManager::Delete()
{
	for (int i = 0; i < m_BGM.size(); i++)
	{
		//存在していて
		if (m_BGM[i])
		{
			m_BGM[i]->DeleteSound();
			//delete m_BGM[i];
			m_BGM[i] = nullptr;
		}
	}

	for (int i = 0; i < m_SE.size(); i++)
	{
		//存在していて
		if (m_SE[i])
		{
			m_SE[i]->DeleteSound();
			//delete m_SE[i];
			m_SE[i] = nullptr;
		}
	}
	return;
}

bool SoundManager::isPlayingSound(std::string name)
{
	for (int i = 0; i < m_BGM.size(); i++)
	{
		//存在しているかつ、引数の名前と一致する名前があったとき
		if (m_BGM[i] && m_BGM[i]->GetName() == name)
		{
			//流れているかどうかを返す
			return CheckPlaying(m_BGM[i]->GetHandle());
		}
	}
	return false;
}

void SoundManager::ChangeBGMVolume(float volume)
{
	m_BGMvolume = volume;
	for (int i = 0; i < m_BGM.size(); i++)
	{
		ChangeVolumeSoundMem(static_cast<int>(255 * m_BGMvolume), m_BGM[i]->GetHandle());
	}
	return;
}

void SoundManager::ChangeSEVolume(float volume)
{
	m_SEvolume = volume;
	for (int i = 0; i < m_SE.size(); i++)
	{
		ChangeVolumeSoundMem(static_cast<int>(255 * m_SEvolume), m_SE[i]->GetHandle());
	}
	return;
}
