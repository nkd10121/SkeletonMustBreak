#include "DxLib.h"
#include "EffekseerForDXLib.h"
#include "SceneManager.h"
#include "SceneBase.h"
#include "SceneTitle.h"
#include "SceneDebug.h"
#include "Input.h"
#include "Game.h"

#include <cmath>
#include <memory>

//Debugのためのinclude
#include "SceneGame.h"

#include "Font.h"
#include "CsvLoad.h"
#include "MapDataLoad.h"
#include "Setting.h"
#include "SoundManager.h"
#include "ScoreManager.h"
#include "EffectManager.h"
#include "ModelManager.h"


#include <filesystem>

// プログラムは WinMain から始まります
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	// windowモード設定
	ChangeWindowMode(true);
	SetGraphMode(Game::kWindowWidth, Game::kWindowHeight, 32);
	// ウインドウ名設定
	SetMainWindowText(Game::kWindowName);

	SetFullSceneAntiAliasingMode(4, 4);

	Setting::GetInstance().Load();
	ChangeWindowMode(Setting::GetInstance().GetIsFullScreen());


	if (DxLib_Init() == -1)		// ＤＸライブラリ初期化処理
	{
		return -1;			// エラーが起きたら直ちに終了
	}

	SetUseZBuffer3D(true);
	SetWriteZBuffer3D(true);
	SetUseBackCulling(true);

	Effekseer_Init(800);
	Effekseer_InitDistortion();
	SetChangeScreenModeGraphicsSystemResetFlag(false);
	Effekseer_SetGraphicsDeviceLostCallbackFunctions();

	// ダブルバッファモード
	SetDrawScreen(DX_SCREEN_BACK);

	// セーブデータ保存用のフォルダがない場合生成する
	if (!std::filesystem::is_directory("data/save"))
	{
		std::filesystem::create_directory("data/save");
	}


	SoundManager::GetInstance().ChangeBGMVolume(Setting::GetInstance().GetBGMVolume());
	SoundManager::GetInstance().ChangeSEVolume(Setting::GetInstance().GetSEVolume());

	ScoreManager::GetInstance().Load();

	SceneManager sceneManager;
#ifdef _DEBUG
	//sceneManager.ChangeScene(std::make_shared<SceneTitle>(sceneManager));
	//sceneManager.ChangeScene(std::make_shared<SceneGame>(sceneManager));
	sceneManager.ChangeScene(std::make_shared<SceneDebug>(sceneManager));
#else
	sceneManager.ChangeScene(std::make_shared<SceneTitle>(sceneManager));
#endif

	std::shared_ptr<Input> pInput = std::make_shared<Input>();

	while (ProcessMessage() == 0)
	{
		LONGLONG  time = GetNowHiPerformanceCount();
		// 画面のクリア
		ClearDrawScreen();

		Effekseer_Sync3DSetting();

		pInput->Update();
		sceneManager.Update(pInput);
		UpdateEffekseer3D();
		sceneManager.Draw();

		//裏画面を表画面を入れ替える
		ScreenFlip();

		// escキーを押したら終了する
		if (CheckHitKey(KEY_INPUT_ESCAPE))
		{
			break;
		}

		// fpsを60に固定
		while (GetNowHiPerformanceCount() - time < 16667)
		{
		}
	}

	//staticクラスの解放
	Font::GetInstance().Destroy();
	CsvLoad::GetInstance().Destroy();
	MapDataLoad::GetInstance().Destroy();
	Setting::GetInstance().Destroy();
	SoundManager::GetInstance().Destroy();
	ScoreManager::GetInstance().Destroy();
	EffectManager::GetInstance().Destroy();
	ModelManager::GetInstance().Destroy();

	Effkseer_End();
	DxLib_End();				// ＤＸライブラリ使用の終了処理

	return 0;				// ソフトの終了 
}