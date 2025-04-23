#pragma once
#include "Vec3.h"
#include <string>
#include <vector>
#include <unordered_map>

/// <summary>
/// ステージ情報を読み込むクラス
/// </summary>
class MapDataLoad
{
private:
	// シングルトンパターンなのでコンストラクタはprivateに置く
	MapDataLoad();
	//コンストラクタ
	virtual ~MapDataLoad();

	struct EnemyRoute
	{
		std::vector<MyLib::Vec3> start;
		std::vector<std::vector<MyLib::Vec3>> pos;
		MyLib::Vec3 end;
	};

public:
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
	//ステージ情報を読み込む
	void Load(const char* stageName);
	//ステージを描画する
	void Draw();

	//プレイヤーなどのマップとの当たり判定のハンドルを取得する
	const int GetStageCollisionHandle()const { return m_collisionHandle; }
	//敵のマップとの当たりhな艇のハンドルを取得する
	const int GetStageEnemyCollisionHandle()const { return m_enemyCollisionHandle; }

	//罠設置可能座標を取得する
	std::list<MyLib::Vec3> GetTrapPos()const { return m_trapPos; }

	//読み込んだマップデータをリセットする
	void ResetStageData();
	//クリスタル座標を取得する
	const MyLib::Vec3 GetCrystalPos()const { return m_crustalPos; }
	//敵の移動予定ルートを取得する
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

	//敵の移動予定ルートを読み込む
	void LoadEnemyRoute(LocationData& data);

private:
	//staticにすることで
	//Singletonのポインタがプログラム起動時に一つ作られるようにする
	static MapDataLoad* m_instance;

private:
	//クリスタルを動かすための角度
	float m_angle;
	//マップ情報
	std::vector<LocationData> m_data;
	//モデルハンドル
	std::unordered_map<std::string, int> m_models;
	//トラップ設置可能座標
	std::list<MyLib::Vec3> m_trapPos;
	//プレイヤーなどのマップとの当たり判定
	int m_collisionHandle;
	//敵のマップとの当たり判定
	int m_enemyCollisionHandle;

	//クリスタル座標
	MyLib::Vec3 m_crustalPos;
	//敵の移動予定ルート
	EnemyRoute m_enemyRoute;
};

