#include "Setting.h"
#include "DxLib.h"
#include <cassert>
#include <fstream>
#include <string>

Setting* Setting::m_instance = nullptr;

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
	const char* const kSaveDataFilename = "data/save/config.dat";

	constexpr int kScoreNum = 3;
}

Setting::~Setting()
{
}

void Setting::Load()
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
		Data data;
		ifs.read((char*)&data, sizeof(Data));
		//	printfDx("version %d\n", data.version);
		ifs.close();

		// �ǂݍ��񂾃f�[�^�����ۂɎg�p����f�[�^�ɃR�s�[
		m_data = data;
	}
}

void Setting::Save()
{
	std::ofstream ofs(kSaveDataFilename, std::ios_base::binary);
	ofs.write((char*)&m_data, sizeof(Data));
}

void Setting::ClearData()
{
	m_data.bgmVolume = 0.5f;
	m_data.seVolume = 0.5f;
	m_data.sensitivity = 0.5f;
	m_data.isFullScreen = false;
}

void Setting::CreateNewData()
{
	ClearData();

	std::ofstream ofs(kSaveDataFilename, std::ios_base::binary);
	ofs.write((char*)&m_data, sizeof(Data));
}
