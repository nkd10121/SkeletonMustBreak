#pragma once

/// <summary>
/// スコアを管理するクラス
/// TODO:オンラインランキングに対応させたいい
/// TODO:ステージが増えてもいじらなくていいつくりにしたい
/// </summary>
class ScoreManager
{
private:
	//スコア
	struct Score
	{
		int stage1Score;
		int stage2Score;
		int stage3Score;
	};

private:
	// シングルトンパターンなのでコンストラクタはprivateに置く
	ScoreManager() {};
	//デストラクタ
	virtual ~ScoreManager();

public:
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
	//スコアをロード
	void Load();
	//スコアをセーブ
	void Save();

	//スコアをリセット
	void ClearData();
	//新しくスコアを生成する
	void CreateNewData();

	//ステージ1のスコアを設定する
	void SetStage1Score(int score);
	//ステージ2のスコアを設定する
	void SetStage2Score(int score);
	//ステージ3のスコアを設定する
	void SetStage3Score(int score);

	//ステージ1のスコアを取得する
	const int GetStage1Score()const { return m_score.stage1Score; }
	//ステージ2のスコアを取得する
	const int GetStage2Score()const { return m_score.stage2Score; }
	//ステージ3のスコアを取得する
	const int GetStage3Score()const { return m_score.stage3Score; }
private:
	//staticにすることで
	//Singletonのポインタがプログラム起動時に一つ作られるようにする
	static ScoreManager* m_instance;

private:
	//スコア
	Score m_score;
};

