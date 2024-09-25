#pragma once
class ScoreManager
{
private:
	struct Score
	{
		int stage1Score;
		int stage2Score;
		int stage3Score;
	};

private:
	// �V���O���g���p�^�[���Ȃ̂ŃR���X�g���N�^��private�ɒu��
	ScoreManager() {};

public:
	virtual ~ScoreManager();

	//�R�s�[�R���X�g���N�^������̂̐������ł��Ă��܂�����
	//�R�s�[�R���X�g���N�^���֎~����
	ScoreManager(const ScoreManager&) = delete;
	ScoreManager& operator=(const ScoreManager&) = delete;
	ScoreManager(ScoreManager&&) = delete;
	ScoreManager& operator= (const ScoreManager&&) = delete;

	/// <summary>
	/// ScoreManager��GetInstance()��ʂ����Q�Ƃ��炵�����p�ł��Ȃ�
	/// </summary>
	/// <returns></returns>
	static ScoreManager& GetInstance()
	{
		if (!m_instance)
		{
			m_instance = new ScoreManager;
		}

		return *m_instance;
	}

	//��������Y���ƕ��ʂɃ��������[�N
	static void Destroy()
	{
		delete m_instance;
		m_instance = nullptr;
	}

public:
	void Load();
	void Save();

	void ClearData();
	void CreateNewData();

	void SetStage1Score(int score);
	void SetStage2Score(int score);
	void SetStage3Score(int score);

	const int GetStage1Score()const { return m_score.stage1Score; }
	const int GetStage2Score()const { return m_score.stage2Score; }
	const int GetStage3Score()const { return m_score.stage3Score; }

private:
	//static�ɂ��邱�Ƃ�
	//Singleton�̃|�C���^���v���O�����N�����Ɉ�����悤�ɂ���
	static ScoreManager* m_instance;

private:
	Score m_score;
};

