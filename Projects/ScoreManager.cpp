#include "ScoreManager.h"
#include "DxLib.h"
#include <cassert>
#include <fstream>
#include <string>

ScoreManager* ScoreManager::m_instance = nullptr;

namespace
{
	struct Header
	{
		char id[4] = "inf";		//最後に\nが入っているためこれで4文字だと思っていい(4バイト)
		float version = 1.0f;	//4バイト
		size_t dataCount = 0;	//4バイト
		//空白の4バイトが入っている(パディング)
	};

	// セーブデータファイル名
	const char* const kSaveDataFilename = "data/save/score.dat";

	//スコア(ステージ)数
	constexpr int kScoreNum = 3;
}

/// <summary>
/// デストラクタ
/// </summary>
ScoreManager::~ScoreManager()
{
}

/// <summary>
/// スコアをロード
/// </summary>
void ScoreManager::Load()
{
	// データのクリア
	ClearData();

	std::ifstream ifs;
	ifs.open(kSaveDataFilename, std::ios_base::binary);
	if (ifs.fail())
	{
		// ファイル読み込み失敗 セーブデータを作る
		CreateNewData();
		return;
	}
	else
	{
		Score data;
		ifs.read((char*)&data, sizeof(Score));
		//	printfDx("version %d\n", data.version);
		ifs.close();

		// 読み込んだデータを実際に使用するデータにコピー
		m_score = data;
	}
}

/// <summary>
/// スコアをセーブ
/// </summary>
void ScoreManager::Save()
{
	std::ofstream ofs(kSaveDataFilename, std::ios_base::binary);
	ofs.write((char*)&m_score, sizeof(Score));
}

/// <summary>
/// スコアをリセット
/// </summary>
void ScoreManager::ClearData()
{
	m_score.stage1Score = 0;
	m_score.stage2Score = 0;
	m_score.stage3Score = 0;
}

/// <summary>
/// 新しくスコアを生成する
/// </summary>
void ScoreManager::CreateNewData()
{
	ClearData();

	std::ofstream ofs(kSaveDataFilename, std::ios_base::binary);
	ofs.write((char*)&m_score, sizeof(Score));
}

/// <summary>
/// ステージ1のスコアを設定する
/// </summary>
/// <param name="score">設定するスコア</param>
void ScoreManager::SetStage1Score(int score)
{
	m_score.stage1Score = score;
	Save();
}

/// <summary>
/// ステージ2のスコアを設定する
/// </summary>
/// <param name="score">設定するスコア</param>
void ScoreManager::SetStage2Score(int score)
{
	m_score.stage2Score = score;
	Save();
}

/// <summary>
/// ステージ3のスコアを設定する
/// </summary>
/// <param name="score">設定するスコア</param>
void ScoreManager::SetStage3Score(int score)
{
	m_score.stage3Score = score;
	Save();
}
