#include "MapDataLoad.h"
#include <fstream>
#include <sstream>

#include "ModelManager.h"

MapDataLoad* MapDataLoad::m_instance = nullptr;

namespace
{
	float DegreeToRadian(float deg)
	{
		return deg * DX_PI_F / 180.0f;
	}

	const std::string kModelPathFront = "data/model/";
	const std::string kModelPathBack = ".mv1";

	const std::string kStageDataPathFront = "data/stageData/";
	const std::string kStageDataPathBack = ".loc";
}

/// <summary>
/// コンストラクタ
/// </summary>
MapDataLoad::MapDataLoad() :
	m_collisionHandle(-1),
	m_enemyCollisionHandle(-1)
{
	m_models["floor"] = ModelManager::GetInstance().GetModelHandle((kModelPathFront + "floor" + kModelPathBack));
	m_models["wall"] = ModelManager::GetInstance().GetModelHandle((kModelPathFront + "wall" + kModelPathBack));
	m_models["doorFrame"] = ModelManager::GetInstance().GetModelHandle((kModelPathFront + "doorFrame" + kModelPathBack));

	m_models["crystal"] = ModelManager::GetInstance().GetModelHandle((kModelPathFront + "crystal" + kModelPathBack));
	m_models["Blocks"] = ModelManager::GetInstance().GetModelHandle((kModelPathFront + "Blocks" + kModelPathBack));

	m_enemyRoute.pos.resize(10);
}

/// <summary>
/// デストラクタ
/// </summary>
MapDataLoad::~MapDataLoad()
{
	for (auto& model : m_models)
	{
		MV1DeleteModel(model.second);
	}

	m_models.clear();

	for (auto& data : m_data)
	{
		MV1DeleteModel(data.handle);
	}

	m_data.clear();

	MV1DeleteModel(m_collisionHandle);
	MV1DeleteModel(m_enemyCollisionHandle);
}

/// <summary>
/// ステージ情報を読み込む
/// </summary>
/// <param name="stageName">ステージ名</param>
void MapDataLoad::Load(const char* stageName)
{
	//ステージの当たり判定モデルを取得　
	std::string check = kStageDataPathFront + stageName + "_col" + kModelPathBack;
	m_collisionHandle = ModelManager::GetInstance().GetModelHandle(check);
	MV1SetPosition(m_collisionHandle, VECTOR(0.0f, 0.0f, 0.0f));
	MV1SetScale(m_collisionHandle, VECTOR(0.01f, 0.01f, 0.01f));
	MV1SetRotationXYZ(m_collisionHandle, VECTOR(0.0f, static_cast<float>(DX_PI), 0.0f));

	//ステージの当たり判定モデルを取得　
	std::string enemyCheck = kStageDataPathFront + stageName +"_enemy"+ "_col" + kModelPathBack;
	m_enemyCollisionHandle = ModelManager::GetInstance().GetModelHandle(enemyCheck);
	MV1SetPosition(m_enemyCollisionHandle, VECTOR(0.0f, 0.0f, 0.0f));
	MV1SetScale(m_enemyCollisionHandle, VECTOR(0.01f, 0.01f, 0.01f));
	MV1SetRotationXYZ(m_enemyCollisionHandle, VECTOR(0.0f, static_cast<float>(DX_PI), 0.0f));



	//開くファイルのハンドルを取得
	int handle = FileRead_open((kStageDataPathFront + stageName + kStageDataPathBack).c_str());

	//読み込むオブジェクト数が何個あるか取得
	int dataCnt = 0;
	FileRead_read(&dataCnt, sizeof(dataCnt), handle);
	//読み込むオブジェクト数分の配列に変更する
	m_data.resize(dataCnt);

	//配列の数分回す
	for (auto& loc : m_data)
	{
		//名前のバイト数を取得する
		byte nameCnt = 0;
		FileRead_read(&nameCnt, sizeof(nameCnt), handle);
		//名前のサイズを変更する
		loc.name.resize(nameCnt);
		//名前を取得する
		FileRead_read(loc.name.data(), sizeof(char) * static_cast<int>(loc.name.size()), handle);

		//タグのバイト数を取得する
		byte tagCnt = 0;
		FileRead_read(&tagCnt, sizeof(tagCnt), handle);
		//タグのサイズを変更する
		loc.tag.resize(tagCnt);
		//タグを取得する
		FileRead_read(loc.tag.data(), sizeof(char) * static_cast<int>(loc.tag.size()), handle);

		//座標を取得する
		FileRead_read(&loc.pos, sizeof(loc.pos), handle);
		//回転を取得する
		FileRead_read(&loc.rot, sizeof(loc.rot), handle);
		//大きさを取得する
		FileRead_read(&loc.scale, sizeof(loc.scale), handle);

		if (loc.tag == "trapPos")
		{
			m_trapPos.push_back(loc.pos);
		}

		if (loc.tag == "crystal")
		{
			m_crustalPos = loc.pos;
		}



		LoadEnemyRoute(loc);

	}
	FileRead_close(handle);

	for (auto& loc : m_data)
	{
		loc.rot.x = DegreeToRadian(loc.rot.x);
		loc.rot.y = DegreeToRadian(loc.rot.y);
		loc.rot.z = DegreeToRadian(loc.rot.z);

		loc.handle = MV1DuplicateModel(m_models[loc.tag]);
		MV1SetPosition(loc.handle, loc.pos.ConvertToVECTOR());
		MV1SetScale(loc.handle, loc.scale.ConvertToVECTOR());
		MV1SetRotationXYZ(loc.handle, loc.rot.ConvertToVECTOR());

	}
}

/// <summary>
/// ステージを描画する
/// </summary>
void MapDataLoad::Draw()
{
	m_angle += 0.025f;
	auto posY = sinf(m_angle);

	int y = 0;
	for (auto& loc : m_data)
	{
		if (loc.tag != "floor" && loc.tag != "wall" && loc.tag != "doorFrame" && loc.tag != "crystal" && loc.tag != "Blocks")
		{
			continue;
		}

#ifdef false
		DrawFormatString(0, y, 0xffffff, "name = %s,tag = %s,pos = {%.2f,%.2f,%.2f},rot = {%.2f,%.2f,%.2f},scale = {%.2f,%.2f,%.2f}",
			loc.name.c_str(),
			loc.tag.c_str(),
			loc.pos.x, loc.pos.y, loc.pos.z,
			loc.rot.x, loc.rot.y, loc.rot.z,
			loc.scale.x, loc.scale.y, loc.scale.z
		);

		y += 20;
#endif
		//クリスタルを動かす
		if (loc.tag == "crystal")
		{
			MV1SetPosition(loc.handle, VECTOR(loc.pos.x, loc.pos.y + posY, loc.pos.z));
			MV1SetRotationXYZ(loc.handle, VECTOR(0.0f, m_angle/4, 0.0f));
		}

		MV1DrawModel(loc.handle);
	}

#ifdef true
	MV1DrawModel(m_collsionHanmdle);
#endif
	}

/// <summary>
/// 読み込んだマップデータをリセットする
/// </summary>
void MapDataLoad::ResetStageData()
{
	for (auto& data : m_data)
	{
		MV1DeleteModel(data.handle);
	}

	m_data.clear();

	MV1DeleteModel(m_collisionHandle);
	MV1DeleteModel(m_enemyCollisionHandle);

	m_angle = 0l;
	m_trapPos.clear();
	m_enemyRoute.start.clear();
	m_enemyRoute.pos.clear();
	m_enemyRoute.pos.resize(10);
	m_enemyRoute.end = MyLib::Vec3();
}

/// <summary>
/// 敵の移動予定ルートを読み込む
/// </summary>
/// <param name="data">座標情報</param>
void MapDataLoad::LoadEnemyRoute(LocationData& data)
{
	std::string route = "route";
	for (int i = 0; i < m_enemyRoute.pos.size(); i++)
	{
		if (data.tag == "enemyStartPos")
		{
			m_enemyRoute.pos[0].push_back(data.pos);
			return;
		}

		if (data.tag == "enemyEndPos")
		{
			m_enemyRoute.end = data.pos;
			return;
		}

		if (data.tag == route + std::to_string(i))
		{
			m_enemyRoute.pos[i].push_back(data.pos);
			return;
		}
	}
}
