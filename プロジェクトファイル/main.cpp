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

//Debug�̂��߂�include
#include "SceneGame.h"

#include "Font.h"
#include "CsvLoad.h"
#include "MapDataLoad.h"
#include "Setting.h"
#include "SoundManager.h"
#include "ScoreManager.h"
#include "EffectManager.h"


#include <filesystem>

// �v���O������ WinMain ����n�܂�܂�
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	// window���[�h�ݒ�
	ChangeWindowMode(true);
	SetGraphMode(Game::kWindowWidth, Game::kWindowHeight, 32);
	// �E�C���h�E���ݒ�
	SetMainWindowText(Game::kTitleText);

	SetFullSceneAntiAliasingMode(4, 4);

	Setting::GetInstance().Load();
	ChangeWindowMode(Setting::GetInstance().GetIsFullScreen());


	if (DxLib_Init() == -1)		// �c�w���C�u��������������
	{
		return -1;			// �G���[���N�����璼���ɏI��
	}

	SetUseZBuffer3D(true);
	SetWriteZBuffer3D(true);
	SetUseBackCulling(true);

	Effekseer_Init(800);
	Effekseer_InitDistortion();
	SetChangeScreenModeGraphicsSystemResetFlag(false);
	Effekseer_SetGraphicsDeviceLostCallbackFunctions();

	// �_�u���o�b�t�@���[�h
	SetDrawScreen(DX_SCREEN_BACK);

	// �Z�[�u�f�[�^�ۑ��p�̃t�H���_���Ȃ��ꍇ��������
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
		// ��ʂ̃N���A
		ClearDrawScreen();

		Effekseer_Sync3DSetting();

		pInput->Update();
		sceneManager.Update(pInput);
		UpdateEffekseer3D();
		sceneManager.Draw();

		//����ʂ�\��ʂ����ւ���
		ScreenFlip();

		// esc�L�[����������I������
		if (CheckHitKey(KEY_INPUT_ESCAPE))
		{
			break;
		}

		// fps��60�ɌŒ�
		while (GetNowHiPerformanceCount() - time < 16667)
		{
		}
	}

	//static�N���X�̉��
	Font::GetInstance().Destroy();
	CsvLoad::GetInstance().Destroy();
	MapDataLoad::GetInstance().Destroy();
	Setting::GetInstance().Destroy();
	SoundManager::GetInstance().Destroy();
	ScoreManager::GetInstance().Destroy();
	EffectManager::GetInstance().Destroy();

	Effkseer_End();
	DxLib_End();				// �c�w���C�u�����g�p�̏I������

	return 0;				// �\�t�g�̏I�� 
}