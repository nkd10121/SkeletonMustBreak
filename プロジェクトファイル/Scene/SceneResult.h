#pragma once
#include "SceneBase.h"
#include <memory>
#include <vector>
#include <map>

struct ScoreInfo;

class SceneResult : public SceneBase
{
public:
	SceneResult(SceneManager& mgr);
	virtual~SceneResult();

	/// <summary>
	/// �X�V����
	/// </summary>
	/// <param name="input"></param>
	void Update(std::shared_ptr<Input>& input);

	/// <summary>
	/// �`�揈��
	/// </summary>
	void Draw();

private:
	void GetScore();
	void FrameToTime();

	void CalculationScore();

private:
	//�J�ڐ�
	enum class e_Destination : int
	{
		InGame,
		details,
		//Upgrade,
		StageSelect,
	};

	e_Destination m_destinationScene;

	//�X�R�A�v�Z�ɕK�v�ȏ��
	std::shared_ptr<ScoreInfo> m_scoreInfo;

	int m_frame;
	int m_hp;
	int m_playerKillCount;
	int m_trapKillCount;
	int m_combo;

	int m_min;
	char m_sec1;
	char m_sec2;

	int m_score;

	bool m_isClear;

	std::vector<int> m_handles;

	std::vector<std::pair<int, int>> m_uiPos;


	float m_angle;
	int m_cursorOffsetX;

	//�t�H���g�n���h��
	int m_fontHandle;
	int m_textHandle;
	int m_detailHandle;

	int m_alpha;

	//�X�V�����o�֐��|�C���^
	void (SceneResult::* m_updateFunc)(std::shared_ptr<Input> input);
	/*��Ԋ֐�*/
	//�t�F�[�h�A�E�g
	void FadeOutUpdate(std::shared_ptr<Input> input);
	//�ʏ�
	void NormalUpdate(std::shared_ptr<Input> input);
	//�ڍ�
	void DetailUpdate(std::shared_ptr<Input> input);

	//�`�惁���o�֐��|�C���^
	void (SceneResult::* m_drawFunc)();
	//�t�F�[�h�A�E�g
	void FadeOutDraw();
	//�ʏ�
	void NormalDraw();
	//�ڍ�
	void DetailDraw();
};

