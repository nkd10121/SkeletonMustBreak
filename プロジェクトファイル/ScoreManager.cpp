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
		char id[4] = "inf";		//�Ō��\n�������Ă��邽�߂����4�������Ǝv���Ă���(4�o�C�g)
		float version = 1.0f;	//4�o�C�g
		size_t dataCount = 0;	//4�o�C�g
		//�󔒂�4�o�C�g�������Ă���(�p�f�B���O)
	};

	// �Z�[�u�f�[�^�t�@�C����
	const char* const kSaveDataFilename = "data/save/score.dat";

	constexpr int kScoreNum = 3;
}

ScoreManager::~ScoreManager()
{
}

void ScoreManager::Load()
{
	// �f�[�^�̃N���A
	ClearData();

	std::ifstream ifs;
	ifs.open(kSaveDataFilename, std::ios_base::binary);
	if (ifs.fail())
	{
		// �t�@�C���ǂݍ��ݎ��s �Z�[�u�f�[�^�����
		CreateNewData();
		return;
	}
	else
	{
		Score data;
		ifs.read((char*)&data, sizeof(Score));
		//	printfDx("version %d\n", data.version);
		ifs.close();

		// �ǂݍ��񂾃f�[�^�����ۂɎg�p����f�[�^�ɃR�s�[
		m_score = data;
	}
}

void ScoreManager::Save()
{
	std::ofstream ofs(kSaveDataFilename, std::ios_base::binary);
	ofs.write((char*)&m_score, sizeof(Score));
}

void ScoreManager::ClearData()
{
	m_score.stage1Score = 0;
	m_score.stage2Score = 0;
	m_score.stage3Score = 0;
}

void ScoreManager::CreateNewData()
{
	ClearData();

	std::ofstream ofs(kSaveDataFilename, std::ios_base::binary);
	ofs.write((char*)&m_score, sizeof(Score));
}

void ScoreManager::SetStage1Score(int score)
{
	m_score.stage1Score = score;
	Save();
}

void ScoreManager::SetStage2Score(int score)
{
	m_score.stage2Score = score;
	Save();
}

void ScoreManager::SetStage3Score(int score)
{
	m_score.stage3Score = score;
	Save();
}
