#pragma once
#include <map>
#include <string>
#include <list>

/// <summary>
/// フォントハンドルを管理するクラス
/// </summary>
class Font
{
private:
	// シングルトンパターンなのでコンストラクタはprivateに置く
	Font() {};

public:
	virtual ~Font();

	//コピーコンストラクタから実体の生成ができてしまうため
	//コピーコンストラクタを禁止する
	Font(const Font&) = delete;
	Font& operator=(const Font&) = delete;
	Font(Font&&) = delete;
	Font& operator= (const Font&&) = delete;

	/// <summary>
	/// FontはGetInstance()を通した参照からしか利用できない
	/// </summary>
	/// <returns></returns>
	static Font& GetInstance()
	{
		if (!m_instance)
		{
			m_instance = new Font;
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
	//フォントのハンドルを取得する
	int GetFontHandle(std::string path,std::string name, int fontSize);

private:
	//staticにすることで
	//Singletonのポインタがプログラム起動時に一つ作られるようにする
	static Font* m_instance;

private:
	//フォントハンドル
	std::map<std::pair<std::string, int>, int> m_handle;
	//フォントパス
	std::list < std::string> m_paths;
};

