#include "SceneOption.h"
#include "SceneManager.h"
#include "Setting.h"
#include "SoundManager.h"

#include "Font.h"
#include "Game.h"

namespace
{
	const char* kFontPath = "data/font/Dela-Gothic-One.ttf";
	const char* kFontName = "Dela Gothic One";
	constexpr int kTextSize = 32;

	constexpr int kTextPosX = 340;

	enum ui
	{
		rightArrow,
		leftArrow,
	};

	constexpr int kTextPosY[4] = { 180 ,280,380,480 };

	constexpr int kBoxXLeft = 540;
	constexpr int kBoxXSize = 400;
	constexpr int kBoxXRight = kBoxXLeft + kBoxXSize;

	constexpr int kPushWaitCount = 30;
}

SceneOption::SceneOption(SceneManager& mgr) :
	SceneBase(mgr),
	m_angle(0),
	m_cursorOffsetX(0),
	m_nowItem(Item::BGM),
	m_bgmScale(50),
	m_seScale(50),
	m_sensitivityScale(50),
	m_isFullScreen(false),
	m_pushCount(0)
{
	FadeInSkip();

	m_fontHandle = Font::GetInstance().GetFontHandle(kFontPath, kFontName, kTextSize);

	m_handle.push_back(LoadGraph("data/img/ArrowRight.png"));
	m_handle.push_back(LoadGraph("data/img/ArrowLeft.png"));

	m_updateFunc = &SceneOption::BgmUpdate;

	m_bgmScale = static_cast<int>(Setting::GetInstance().GetBGMVolume() * 100);
	m_seScale = static_cast<int>(Setting::GetInstance().GetSEVolume() * 100);
	m_sensitivityScale = static_cast<int>(Setting::GetInstance().GetSensitivity() * 100);
	m_isFullScreen = !Setting::GetInstance().GetIsFullScreen();
}

SceneOption::~SceneOption()
{
	for (auto& handle : m_handle)
	{
		DeleteGraph(handle);
	}

	m_handle.clear();
}

void SceneOption::Update(std::shared_ptr<Input>& input)
{
	UpdateFade();

	m_angle += 0.14f;
	m_cursorOffsetX = static_cast<int>(sinf(m_angle) * 8);

	//何も入力がないとき押しているカウントをリセットする
	if (!input->IsPushed("RIGHT") && !input->IsPushed("LEFT"))
	{
		m_pushCount = 0;
	}

	//状態の更新
	(this->*m_updateFunc)(input);

	if (input->IsTriggered("B"))
	{
		Setting::GetInstance().SetBGMVolume(static_cast<float>(m_bgmScale) / 100);
		Setting::GetInstance().SetSEVolume(static_cast<float>(m_seScale) / 100);
		Setting::GetInstance().SetSensitivity(static_cast<float>(m_sensitivityScale) / 100);
		Setting::GetInstance().SetIsFullScreen(m_isFullScreen);

		Setting::GetInstance().Save();
		m_manager.popScene();
	}
}

void SceneOption::Draw()
{
	//背景の枠を描画
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 220);
	DrawBox(0 + 120, 0 + 100, Game::kWindowWidth - 120, Game::kWindowHeight - 100, 0x000000, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	DrawBox(0 + 120, 0 + 100, Game::kWindowWidth - 120, Game::kWindowHeight - 100, 0xcccccc, false);

	//カーソルを描画
	DrawRotaGraph(640 + kTextPosX + 100 + m_cursorOffsetX, kTextPosY[m_nowItem] + kTextSize / 2, 1.0f, 0.0f, m_handle[leftArrow], true);
	DrawRotaGraph(640 - kTextPosX - 100 - m_cursorOffsetX, kTextPosY[m_nowItem] + kTextSize / 2, 1.0f, 0.0f, m_handle[rightArrow], true);

	//テキストを描画
	DrawStringToHandle(kTextPosX - kTextSize - kTextSize / 2, kTextPosY[BGM], "BGM", 0xffffff, m_fontHandle);
	DrawStringToHandle(kTextPosX - kTextSize, kTextPosY[SE], "SE", 0xffffff, m_fontHandle);
	DrawStringToHandle(kTextPosX - kTextSize, kTextPosY[Sensitivity], "感度", 0xffffff, m_fontHandle);
	DrawStringToHandle(kTextPosX - kTextSize * 3 - kTextSize / 2, kTextPosY[FullScreen], "フルスクリーン", 0xffffff, m_fontHandle);

	//移動幅を描画
	DrawBox(kBoxXLeft, kTextPosY[BGM] + kTextSize / 2 - 2, kBoxXRight, kTextPosY[BGM] + kTextSize / 2 + 2, 0xffffff, false);
	DrawBox(kBoxXLeft, kTextPosY[SE] + kTextSize / 2 - 2, kBoxXRight, kTextPosY[SE] + kTextSize / 2 + 2, 0xffffff, false);
	DrawBox(kBoxXLeft, kTextPosY[Sensitivity] + kTextSize / 2 - 2, kBoxXRight, kTextPosY[Sensitivity] + kTextSize / 2 + 2, 0xffffff, false);

	//中心線の描画
	DrawLine(kBoxXLeft + kBoxXSize / 2, kTextPosY[BGM], kBoxXLeft + kBoxXSize / 2, kTextPosY[BGM] + kTextSize, 0xffffff);
	DrawLine(kBoxXLeft + kBoxXSize / 2, kTextPosY[SE], kBoxXLeft + kBoxXSize / 2, kTextPosY[SE] + kTextSize, 0xffffff);
	DrawLine(kBoxXLeft + kBoxXSize / 2, kTextPosY[Sensitivity], kBoxXLeft + kBoxXSize / 2, kTextPosY[Sensitivity] + kTextSize, 0xffffff);

	//それぞれのスケールの位置を描画
	DrawBox(kBoxXLeft + kBoxXSize * m_bgmScale/100 - 8, kTextPosY[BGM] + kTextSize / 2 - 8, kBoxXLeft + kBoxXSize * m_bgmScale / 100 + 8, kTextPosY[BGM] + kTextSize / 2 + 8, 0xffffff, true);
	DrawBox(kBoxXLeft + kBoxXSize * m_seScale / 100 - 8, kTextPosY[SE] + kTextSize / 2 - 8, kBoxXLeft + kBoxXSize * m_seScale / 100 + 8, kTextPosY[SE] + kTextSize / 2 + 8, 0xffffff, true);
	DrawBox(kBoxXLeft + kBoxXSize * m_sensitivityScale / 100 - 8, kTextPosY[Sensitivity] + kTextSize / 2 - 8, kBoxXLeft + kBoxXSize * m_sensitivityScale / 100 + 8, kTextPosY[Sensitivity] + kTextSize / 2 + 8, 0xffffff, true);

	DrawBox(kBoxXLeft + kBoxXSize / 2 + 14, kTextPosY[FullScreen] + kTextSize / 2 - 14, kBoxXLeft + kBoxXSize / 2 - 14, kTextPosY[FullScreen] + kTextSize / 2 + 14, 0xffffff, m_isFullScreen);


#ifdef _DEBUG
	//DrawBox(0, 0, 1280, 720, 0x000000, true);
	DrawFormatString(0, 0, 0xffffff, "SceneOption");
#endif
	DrawFade();
}

void SceneOption::BgmUpdate(std::shared_ptr<Input> input)
{
	if (input->IsTriggered("DOWN"))
	{
		SoundManager::GetInstance().PlaySE("select");
		m_nowItem = Item::SE;
		m_updateFunc = &SceneOption::SeUpdate;
	}

	if (input->IsPushed("RIGHT"))
	{
		if (m_pushCount == 0 || m_pushCount > kPushWaitCount)
		{
			m_bgmScale++;

			if (m_bgmScale > 100)
			{
				m_bgmScale = 100;
			}
			SoundManager::GetInstance().ChangeBGMVolume(static_cast<float>(m_bgmScale) / 100);
		}
		m_pushCount++;
	}

	if (input->IsPushed("LEFT"))
	{
		if (m_pushCount == 0 || m_pushCount > kPushWaitCount)
		{
			m_bgmScale--;
			if (m_bgmScale < 0)
			{
				m_bgmScale = 0;
			}
			SoundManager::GetInstance().ChangeBGMVolume(static_cast<float>(m_bgmScale) / 100);
		}
		m_pushCount++;
	}
}

void SceneOption::SeUpdate(std::shared_ptr<Input> input)
{
	if (input->IsTriggered("DOWN"))
	{
		SoundManager::GetInstance().PlaySE("select");
		m_nowItem = Item::Sensitivity;
		m_updateFunc = &SceneOption::SensitivityUpdate;
	}

	if (input->IsTriggered("UP"))
	{
		SoundManager::GetInstance().PlaySE("select");
		m_nowItem = Item::BGM;
		m_updateFunc = &SceneOption::BgmUpdate;
	}

	if (input->IsPushed("RIGHT"))
	{
		if (m_pushCount == 0 || m_pushCount > kPushWaitCount)
		{
			m_seScale++;
			if (m_seScale > 100)
			{
				m_seScale = 100;
			}
			SoundManager::GetInstance().ChangeSEVolume(static_cast<float>(m_seScale) / 100);
		}
		m_pushCount++;
	}

	if (input->IsPushed("LEFT"))
	{
		if (m_pushCount == 0 || m_pushCount > kPushWaitCount)
		{
			m_seScale--;
			if (m_seScale < 0)
			{
				m_seScale = 0;
			}
			SoundManager::GetInstance().ChangeSEVolume(static_cast<float>(m_seScale) / 100);
		}
		m_pushCount++;
	}
}

void SceneOption::SensitivityUpdate(std::shared_ptr<Input> input)
{
	if (input->IsTriggered("DOWN"))
	{
		SoundManager::GetInstance().PlaySE("select");
		m_nowItem = Item::FullScreen;
		m_updateFunc = &SceneOption::FullScreenUpdate;
	}

	if (input->IsTriggered("UP"))
	{
		SoundManager::GetInstance().PlaySE("select");
		m_nowItem = Item::SE;
		m_updateFunc = &SceneOption::SeUpdate;
	}

	if (input->IsPushed("RIGHT"))
	{
		if (m_pushCount == 0 || m_pushCount > kPushWaitCount)
		{
			m_sensitivityScale++;
			if (m_sensitivityScale > 100)
			{
				m_sensitivityScale = 100;
			}
		}
		m_pushCount++;
	}

	if (input->IsPushed("LEFT"))
	{
		if (m_pushCount == 0 || m_pushCount > kPushWaitCount)
		{
			m_sensitivityScale--;;
			if (m_sensitivityScale < 0)
			{
				m_sensitivityScale = 0;
			}
		}
		m_pushCount++;
	}
}

void SceneOption::FullScreenUpdate(std::shared_ptr<Input> input)
{
	if (input->IsTriggered("UP"))
	{
		SoundManager::GetInstance().PlaySE("select");
		m_nowItem = Item::Sensitivity;
		m_updateFunc = &SceneOption::SensitivityUpdate;
	}

	if (input->IsTriggered("A"))
	{
		ChangeWindowMode(m_isFullScreen);
		m_isFullScreen = !m_isFullScreen;
	}
}
