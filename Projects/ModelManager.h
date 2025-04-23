#pragma once
#include <string>
#include <list>

/// <summary>
/// ハンドル
/// </summary>
struct Handle
{
	std::string path;	//モデルパス
	int handle;			//モデルハンドル
	bool isEternal;		//永久フラグ
};

/// <summary>
/// 3Dモデルのハンドルを管理するクラス
/// </summary>
class ModelManager
{
private:
	// シングルトンパターンなのでコンストラクタはprivateに置く
	ModelManager() {};
	//デストラクタ
	virtual ~ModelManager();

public:
	//コピーコンストラクタから実体の生成ができてしまうため
	//コピーコンストラクタを禁止する
	ModelManager(const ModelManager&) = delete;
	ModelManager& operator=(const ModelManager&) = delete;
	ModelManager(ModelManager&&) = delete;
	ModelManager& operator= (const ModelManager&&) = delete;

	/// <summary>
	/// ModelManagerはGetInstance()を通した参照からしか利用できない
	/// </summary>
	/// <returns></returns>
	static ModelManager& GetInstance()
	{
		if (!m_instance)
		{
			m_instance = new ModelManager;
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
	//指定したパスのハンドルを取得する
	int GetModelHandle(std::string path, bool isEternal = false);
	//パスのリストを全消去する
	void Clear();

private:
	//staticにすることで
	//Singletonのポインタがプログラム起動時に一つ作られるようにする
	static ModelManager* m_instance;

private:
	//モデルハンドル
	std::list<Handle> m_handles;

};

