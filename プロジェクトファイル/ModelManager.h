#pragma once

/// <summary>
/// 3Dモデルを管理するクラス
/// </summary>
class ModelManager
{
	//1.そのシーンで量産されるモデルをあらかじめロードしておく
	//2.欲しいモデルのハンドルがあればこのクラスに申請して取得する
	//3.シーン切り替えごとにハンドルをDeleteする

private:
	// シングルトンパターンなのでコンストラクタはprivateに置く
	ModelManager() {};

public:
	virtual ~ModelManager() {};

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

private:
	//staticにすることで
	//Singletonのポインタがプログラム起動時に一つ作られるようにする
	static ModelManager* m_instance;
};

