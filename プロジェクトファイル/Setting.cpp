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
		char id[4] = "inf";		//最後に\nが入っているためこれで4文字だと思っていい(4バイト)
		float version = 1.0f;	//4バイト
		size_t dataCount = 0;	//4バイト
		//空白の4バイトが入っている(パディング)
	};

	// セーブデータファイル名
	const char* const kSaveDataFilename = "data/save/config.dat";

	constexpr int kScoreNum = 3;
}

Setting::~Setting()
{
}

void Setting::Load()
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
		Data data;
		ifs.read((char*)&data, sizeof(Data));
		//	printfDx("version %d\n", data.version);
		ifs.close();

		// 読み込んだデータを実際に使用するデータにコピー
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
