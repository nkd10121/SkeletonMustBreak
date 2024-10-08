#include "SceneOption.h"
#include "SceneManager.h"
#include "Setting.h"
#include "SoundManager.h"

#include "UICursor.h"
#include "Font.h"
#include "Game.h"

namespace
{
	//フォントパス
	const char* kFontPath = "data/font/Dela-Gothic-One.ttf";
	//フォント名
	const char* kFontName = "Dela Gothic One";
	//テキストサイズ
	constexpr int kTextSize = 32;
	//テキスト描画X座標
	constexpr int kTextPosX = 340;
	//テキスト描画Y座標
	constexpr int kTextPosY[4] = { 180 ,280,380,480 };

	//カーソル描画X座標
	constexpr int kCursorPosX = kTextPosX + 100;

	constexpr int kBoxXLeft = 540;
	constexpr int kBoxXSize = 400;
	constexpr int kBoxXRight = kBoxXLeft + kBoxXSize;

	constexpr int kPushWaitCount = 30;

	//設定のデフォルト値
	constexpr int kBGMDefaultScale = 50;
	constexpr int kSEDefaultScale = 50;
	constexpr int kSensitivityDefaultScale = 50;
}

/// <summary>
/// コンストラクタ
/// </summary>
/// <param name="mgr">シーン管理クラスのポインタ</param>
SceneOption::SceneOption(SceneManager& mgr) :
	SceneBase(mgr),
	m_fontHandle(-1),
	m_pCursor(nullptr),
	m_nowItem(eItem::BGM),
	m_bgmScale(kBGMDefaultScale),
	m_seScale(kSEDefaultScale),
	m_sensitivityScale(kSensitivityDefaultScale),
	m_isFullScreen(false),
	m_pushCount(0)
{
	FadeInSkip();

	//カーソルの生成
	m_pCursor = std::make_shared<UICursor>();
	m_pCursor->Init();

	m_fontHandle = Font::GetInstance().GetFontHandle(kFontPath, kFontName, kTextSize);

	m_updateFunc = &SceneOption::BgmUpdate;

	m_bgmScale = static_cast<int>(Setting::GetInstance().GetBGMVolume() * 100);
	m_seScale = static_cast<int>(Setting::GetInstance().GetSEVolume() * 100);
	m_sensitivityScale = static_cast<int>(Setting::GetInstance().GetSensitivity() * 100);
	m_isFullScreen = !Setting::GetInstance().GetIsFullScreen();
}

/// <summary>
/// デストラクタ
/// </summary>
SceneOption::~SceneOption()
{
}

/// <summary>
/// 更新
/// </summary>
/// <param name="input">入力管理クラスの参照</param>
void SceneOption::Update(std::shared_ptr<Input>& input)
{
	//フェード処理の更新
	UpdateFade();

	//カーソルの更新
	m_pCursor->Update(kCursorPosX, kTextPosY[m_nowItem] + kTextSize / 2);

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

/// <summary>
/// 描画
/// </summary>
void SceneOption::Draw()
{
	//背景の枠を描画
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 220);
	DrawBox(0 + 120, 0 + 100, Game::kWindowWidth - 120, Game::kWindowHeight - 100, 0x000000, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	DrawBox(0 + 120, 0 + 100, Game::kWindowWidth - 120, Game::kWindowHeight - 100, 0xcccccc, false);

	//カーソルを描画
	m_pCursor->Draw();

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
	DrawFormatString(0, 0, 0xffffff, "SceneOption");
#endif
	DrawFade();
}

/// <summary>
/// 更新(BGM設定)
/// </summary>
/// <param name="input">入力管理クラスの参照</param>
void SceneOption::BgmUpdate(std::shared_ptr<Input> input)
{
	if (input->IsTriggered("DOWN"))
	{
		SoundManager::GetInstance().PlaySE("select");
		m_nowItem = eItem::SE;
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

/// <summary>
/// 更新(SE設定)
/// </summary>
/// <param name="input">入力管理クラスの参照</param>
void SceneOption::SeUpdate(std::shared_ptr<Input> input)
{
	if (input->IsTriggered("DOWN"))
	{
		SoundManager::GetInstance().PlaySE("select");
		m_nowItem = eItem::Sensitivity;
		m_updateFunc = &SceneOption::SensitivityUpdate;
	}

	if (input->IsTriggered("UP"))
	{
		SoundManager::GetInstance().PlaySE("select");
		m_nowItem = eItem::BGM;
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

/// <summary>
/// 更新(感度設定)
/// </summary>
/// <param name="input">入力管理クラスの参照</param>
void SceneOption::SensitivityUpdate(std::shared_ptr<Input> input)
{
	if (input->IsTriggered("DOWN"))
	{
		SoundManager::GetInstance().PlaySE("select");
		m_nowItem = eItem::FullScreen;
		m_updateFunc = &SceneOption::FullScreenUpdate;
	}

	if (input->IsTriggered("UP"))
	{
		SoundManager::GetInstance().PlaySE("select");
		m_nowItem = eItem::SE;
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

/// <summary>
/// 更新(フルスクリーン設定)
/// </summary>
/// <param name="input">入力管理クラスの参照</param>
void SceneOption::FullScreenUpdate(std::shared_ptr<Input> input)
{
	if (input->IsTriggered("UP"))
	{
		SoundManager::GetInstance().PlaySE("select");
		m_nowItem = eItem::Sensitivity;
		m_updateFunc = &SceneOption::SensitivityUpdate;
	}

	if (input->IsTriggered("A"))
	{
		ChangeWindowMode(m_isFullScreen);
		m_isFullScreen = !m_isFullScreen;
	}
}
