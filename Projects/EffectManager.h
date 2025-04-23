#pragma once

#include <vector>
#include <string>
#include <map>
#include <memory>

#include "Vec3.h"
#include "DxLib.h"

/// <summary>
/// エフェクトを管理するクラス
/// </summary>
class EffectManager
{
private:
	//エフェクト構造体
	struct Effect
	{
		int handle;
		float x, y, z;
		int frame;
		bool isExist;
	};

	//エフェクト生成構造体
	struct EffectEmitter
	{
		int emitterHandle = -1;
		const char* effectPath = "";
		std::vector<Effect> effects;
		int endFrame;
	};


private:
	// シングルトンパターンなのでコンストラクタはprivateに置く
	EffectManager() {};

public:
	//デストラクタ
	virtual ~EffectManager();

	//コピーコンストラクタから実体の生成ができてしまうため
	//コピーコンストラクタを禁止する
	EffectManager(const EffectManager&) = delete;
	EffectManager& operator=(const EffectManager&) = delete;
	EffectManager(EffectManager&&) = delete;
	EffectManager& operator= (const EffectManager&&) = delete;

	/// <summary>
	/// EffectManagerはGetInstance()を通した参照からしか利用できない
	/// </summary>
	/// <returns></returns>
	static EffectManager& GetInstance()
	{
		if (!m_instance)
		{
			m_instance = new EffectManager;
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
	//エフェクトをロード
	void Load(std::string name, const char* path, int endFrame, float scale = 1.0f);
	//更新
	void Update();
	//描画
	void Draw();

	//エフェクトを生成
	void CreateEffect(std::string name, MyLib::Vec3 pos, MyLib::Vec3 rot = MyLib::Vec3());

	// 指定した名前のすべてのエフェクトの座標を移動させる
	void SetPos(std::string name, MyLib::Vec3 pos);

	//指定した名前のすべてのエフェクトを回転させる
	void SetRotation(std::string name, MyLib::Vec3 rot);

private:
	//staticにすることで
	//Singletonのポインタがプログラム起動時に一つ作られるようにする
	static EffectManager* m_instance;

private:
	//エフェクトハンドル
	std::map<std::string, std::shared_ptr<EffectEmitter>> m_effect;
};

