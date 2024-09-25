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
	// シングルトンパターンなのでコンストラクタはprivateに置く
	ScoreManager() {};

public:
	virtual ~ScoreManager();

	//コピーコンストラクタから実体の生成ができてしまうため
	//コピーコンストラクタを禁止する
	ScoreManager(const ScoreManager&) = delete;
	ScoreManager& operator=(const ScoreManager&) = delete;
	ScoreManager(ScoreManager&&) = delete;
	ScoreManager& operator= (const ScoreManager&&) = delete;

	/// <summary>
	/// ScoreManagerはGetInstance()を通した参照からしか利用できない
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

	//これをし忘れると普通にメモリリーク
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
	//staticにすることで
	//Singletonのポインタがプログラム起動時に一つ作られるようにする
	static ScoreManager* m_instance;

private:
	Score m_score;
};

