#pragma once
#include "Vec3.h"
#include <string>
#include <vector>
#include <unordered_map>

class MapDataLoad
{
private:
	// シングルトンパターンなのでコンストラクタはprivateに置く
	MapDataLoad();

	struct EnemyRoute
	{
		std::vector<MyLib::Vec3> start;
		std::vector<std::vector<MyLib::Vec3>> pos;
		MyLib::Vec3 end;
	};


public:
	virtual ~MapDataLoad();

	//コピーコンストラクタから実体の生成ができてしまうため
	//コピーコンストラクタを禁止する
	MapDataLoad(const MapDataLoad&) = delete;
	MapDataLoad& operator=(const MapDataLoad&) = delete;
	MapDataLoad(MapDataLoad&&) = delete;
	MapDataLoad& operator= (const MapDataLoad&&) = delete;

	/// <summary>
	/// MapDataLoadはGetInstance()を通した参照からしか利用できない
	/// </summary>
	/// <returns></returns>
	static MapDataLoad& GetInstance()
	{
		if (!m_instance)
		{
			m_instance = new MapDataLoad;
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
	void Load(const char* stageName);

	void Draw();

	const int GetStageCollisionHandle()const { return m_collisionHandle; }
	const int GetStageEnemyCollisionHandle()const { return m_enemyCollisionHandle; }

	std::list<MyLib::Vec3> GetTrapPos()const { return m_trapPos; }

	void ResetStageData();

	const MyLib::Vec3 GetCrystalPos()const { return m_crustalPos; }

	EnemyRoute GetEnemyRoute() { return m_enemyRoute; }

private:
	//配置情報データ
	struct LocationData
	{
		std::string name;	//名前
		std::string tag;	//タグ
		MyLib::Vec3 pos;	//座標
		MyLib::Vec3 rot;	//回転
		MyLib::Vec3 scale;	//大きさ
		int handle;
	};

	void LoadEnemyRoute(LocationData& data);

private:
	//staticにすることで
	//Singletonのポインタがプログラム起動時に一つ作られるようにする
	static MapDataLoad* m_instance;

private:
	float m_angle;

	std::vector<LocationData> m_data;

	std::unordered_map<std::string, int> m_models;

	std::list<MyLib::Vec3> m_trapPos;

	int m_collisionHandle;
	int m_enemyCollisionHandle;

	MyLib::Vec3 m_crustalPos;

	EnemyRoute m_enemyRoute;
};

