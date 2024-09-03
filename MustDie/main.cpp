#include "DxLib.h"
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

// �v���O������ WinMain ����n�܂�܂�
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	// window���[�h�ݒ�
	ChangeWindowMode(true);
	SetGraphMode(Game::kWindowWidth, Game::kWindowHeight, 32);
	// �E�C���h�E���ݒ�
	SetMainWindowText(Game::kTitleText);

	if (DxLib_Init() == -1)		// �c�w���C�u��������������
	{
		return -1;			// �G���[���N�����璼���ɏI��
	}

	SetUseZBuffer3D(true);
	SetWriteZBuffer3D(true);
	SetUseBackCulling(true);

	// �_�u���o�b�t�@���[�h
	SetDrawScreen(DX_SCREEN_BACK);

	SceneManager sceneManager;
#ifdef _DEBUG
	sceneManager.ChangeScene(std::make_shared<SceneTitle>(sceneManager));
	//sceneManager.ChangeScene(std::make_shared<SceneGame>(sceneManager));
	//sceneManager.ChangeScene(std::make_shared<SceneDebug>(sceneManager));
#else
	sceneManager.ChangeScene(std::make_shared<SceneTitle>(sceneManager));
#endif

	std::shared_ptr<Input> pInput = std::make_shared<Input>();

	while (ProcessMessage() == 0)
	{
		LONGLONG  time = GetNowHiPerformanceCount();
		// ��ʂ̃N���A
		ClearDrawScreen();

		pInput->Update();
		sceneManager.Update(pInput);
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

	DxLib_End();				// �c�w���C�u�����g�p�̏I������

	return 0;				// �\�t�g�̏I�� 
}