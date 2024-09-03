#include "SceneBase.h"
#include "SceneManager.h"

#include "Game.h"

#ifdef _DEBUG
namespace
{
	//�����p�����o�[�̕����̍��W
	constexpr int kUpdateBarStringPosX = 0;
	constexpr int kUpdateBarStringPosY = Game::kWindowHeight - 48;
	constexpr int kDrawBarPosStringPosX = 0;
	constexpr int kDrawBarPosStringPosY = Game::kWindowHeight - 32;

	//�����p�����o�[�̍��W
	constexpr int kExplainUpdateBarPosX1 = 34;
	constexpr int kExplainUpdateBarPosY1 = Game::kWindowHeight - 46;
	constexpr int kExplainUpdateBarPosX2 = 62;
	constexpr int kExplainUpdateBarPosY2 = Game::kWindowHeight - 34;
	constexpr int kExplainDrawBarPosX1 = 34;
	constexpr int kExplainDrawBarPosY1 = Game::kWindowHeight - 30;
	constexpr int kExplainDrawBarPosX2 = 62;
	constexpr int kExplainDrawBarPosY2 = Game::kWindowHeight - 18;


	//�����o�[�̐F
	constexpr int kUpdateBarColor = 0x0000ff;
	constexpr int kDrawBarColor = 0xff0000;

	//�����o�[�̍��W
	constexpr int kBarPosX = 0;
	constexpr int kBarPosY = Game::kWindowHeight - 16;
}

#endif
SceneManager::SceneManager()
#ifdef _DEBUG
	:m_updateTime(0.0f),
	m_drawTime(0.0f),
	m_isClear(false)
#endif
{

}

SceneManager::~SceneManager()
{
}

void SceneManager::Update(std::shared_ptr<Input>& input)
{
#ifdef _DEBUG
	//�X�V�O�̃��[�f�B���O���Ԏ擾
	LONGLONG start = GetNowHiPerformanceCount();
#endif

	//�����̂ݎ��s
	scenes.back()->Update(input);

#ifdef _DEBUG
	//�X�V��̃��[�f�B���O���Ԃ���X�V�O�̃��[�f�B���O���Ԃ��������l���擾
	m_updateTime = static_cast<float>(GetNowHiPerformanceCount() - start);
#endif
}

void SceneManager::Draw()
{
#ifdef _DEBUG
	//�X�V�O�̃��[�f�B���O���Ԏ擾
	LONGLONG start = GetNowHiPerformanceCount();
#endif
	//�擪���珇�ɕ`��
	//�Ō�ɐς񂾂��̂��Ō�ɕ`�悳���
	for (auto& scenes : scenes)
	{
		scenes->Draw();
	}
	
#ifdef _DEBUG
	//�X�V��̃��[�f�B���O���Ԃ���X�V�O�̃��[�f�B���O���Ԃ��������l���擾
	m_drawTime = static_cast<float>(GetNowHiPerformanceCount() - start);

	DrawDebug();
#endif
}

void SceneManager::ChangeScene(std::shared_ptr<SceneBase> nextScene)
{
	if (scenes.empty())//���X�g������ۂ����������ւ�����
	{
		scenes.push_back(nextScene);//�����ɒǉ�
	}
	else
	{
		scenes.back() = nextScene;//���Ɉ�ȏ゠��Ζ�����
	}
}

void SceneManager::ChangeAndClearScene(std::shared_ptr<SceneBase> nextScene)
{
	scenes.clear();

	scenes.push_back(nextScene);//�����ɒǉ�
}

void SceneManager::PushScene(std::shared_ptr<SceneBase> scene)
{
	scenes.push_back(scene);
}

void SceneManager::popScene(bool isToTitle)
{
	scenes.pop_back();
}

#ifdef _DEBUG
void SceneManager::DrawDebug()
{
	//�����o�[�̕\���@
	//����
	DrawString(kUpdateBarStringPosX, kUpdateBarStringPosY, "����", 0xffffff, 0x000000);
	DrawBox(kExplainUpdateBarPosX1, kExplainUpdateBarPosY1, kExplainUpdateBarPosX2, kExplainUpdateBarPosY2, kUpdateBarColor, true);
	DrawString(kDrawBarPosStringPosX, kDrawBarPosStringPosY, "�`��", 0xffffff, 0x000000);
	DrawBox(kExplainDrawBarPosX1, kExplainDrawBarPosY1, kExplainDrawBarPosX2, kExplainDrawBarPosY2, kDrawBarColor, true);

	float rate = static_cast<float>(m_updateTime + m_drawTime) / 16666.6f;
	float width = static_cast<float>(Game::kWindowWidth * rate);
	DrawBox(kBarPosX, kBarPosY, static_cast<int>(width), Game::kWindowHeight, kDrawBarColor, true);

	rate = static_cast<float>(m_updateTime) / 16666.6f;
	width = static_cast<float>(Game::kWindowWidth * rate);
	DrawBox(kBarPosX, kBarPosY, static_cast<int>(width), Game::kWindowHeight, kUpdateBarColor, true);
}

#endif