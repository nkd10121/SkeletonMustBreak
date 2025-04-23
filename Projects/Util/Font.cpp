#include "Font.h"
#include "DxLib.h"

Font* Font::m_instance = nullptr;

namespace
{
	const std::string kPathFront = "data/font/";
}

/// <summary>
/// デストラクタ
/// </summary>
Font::~Font()
{
	for (auto path : m_paths)
	{
		// ********** フォントのアンロード **********
		if (RemoveFontResourceEx(path.c_str(), FR_PRIVATE, NULL))
		{
		}
	}

	for (auto handle : m_handle)
	{
		DeleteFontToHandle(handle.second);
	}

	m_handle.clear();
	m_paths.clear();
}

/// <summary>
/// フォントのハンドルを取得する
/// </summary>
/// <param name="path">パス</param>
/// <param name="name">名前</param>
/// <param name="fontSize">フォントサイズ</param>
/// <returns></returns>
int Font::GetFontHandle(std::string path, std::string name, int fontSize)
{
	for (const auto& handle : m_handle)
	{
		auto& data = handle.first;

		//既に同じ条件のハンドルが存在していたらそのハンドルを返す
		if (data.first == name && data.second == fontSize)
		{
			return handle.second;
		}
	}

	//準備をスキップするかどうか
	bool isPreSkip = false;

	for (auto path : m_paths)
	{
		if (path == path)
		{
			isPreSkip = true;
		}
	}

	if (!isPreSkip)
	{
		//使用するフォントを準備する
		if (AddFontResourceEx(path.c_str(), FR_PRIVATE, NULL) > 0) {
		}
		else {
			// フォント読込エラー処理
			MessageBox(NULL, "フォント読込失敗", "", MB_OK);
		}

		m_paths.push_back(path);
	}
	

	//ここを通るときは同じ条件のハンドルが存在しなかったとき
	int ret = -1;
	ret = CreateFontToHandle(name.c_str(), fontSize, -1);

	std::pair<std::string, int> add;
	add.first = name;
	add.second = fontSize;

	m_handle[add] = ret;

	return ret;
}

void Font::DrawStringUseFont(int x, int y, std::string str, unsigned int color, std::string name, int size)
{
	float drawX = static_cast<float>(x);
	float drawY = static_cast<float>(y);
	int handle = -1;

	for (auto& h : m_handle)
	{
		if (h.first.first == name && h.first.second == size)
		{
			handle = h.second;
		}
	}

	auto len = static_cast<float>(str.length());
	drawX -= (len/2) * (size/2);
	drawY -= size/2;


	DrawStringToHandle(static_cast<int>(drawX), static_cast<int>(drawY), str.c_str(), color, handle);
}
