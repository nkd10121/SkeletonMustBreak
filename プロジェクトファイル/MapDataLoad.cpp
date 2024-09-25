#include "MapDataLoad.h"
#include <fstream>
#include <sstream>

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

MapDataLoad::MapDataLoad() :
	m_collisionHandle(-1),
	m_enemyCollisionHandle(-1)
{
	m_models["floor"] = MV1LoadModel((kModelPathFront + "floor" + kModelPathBack).c_str());
	m_models["wall"] = MV1LoadModel((kModelPathFront + "wall" + kModelPathBack).c_str());
	m_models["doorFrame"] = MV1LoadModel((kModelPathFront + "doorFrame" + kModelPathBack).c_str());

	m_models["crystal"] = MV1LoadModel((kModelPathFront + "crystal" + kModelPathBack).c_str());
	m_models["Blocks"] = MV1LoadModel((kModelPathFront + "Blocks" + kModelPathBack).c_str());

	m_enemyRoute.pos.resize(10);
}

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

void MapDataLoad::Load(const char* stageName)
{
	//ステージの当たり判定モデルを取得　
	std::string check = kStageDataPathFront + stageName + "_col" + kModelPathBack;
	m_collisionHandle = MV1LoadModel(check.c_str());
	MV1SetPosition(m_collisionHandle, VECTOR(0.0f, 0.0f, 0.0f));
	MV1SetScale(m_collisionHandle, VECTOR(0.01f, 0.01f, 0.01f));
	MV1SetRotationXYZ(m_collisionHandle, VECTOR(0.0f, static_cast<float>(DX_PI), 0.0f));

	//ステージの当たり判定モデルを取得　
	std::string enemyCheck = kStageDataPathFront + stageName +"_enemy"+ "_col" + kModelPathBack;
	m_enemyCollisionHandle = MV1LoadModel(enemyCheck.c_str());
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


	//if (data.tag == "enemyStartPos")
	//{
	//	m_enemyRoute.pos[0].push_back(data.pos);
	//	return;
	//}
	//if (data.tag == "route1")
	//{
	//	m_enemyRoute.pos[1].push_back(data.pos);
	//	return;
	//}
	//if (data.tag == "route2")
	//{
	//	m_enemyRoute.pos[2].push_back(data.pos);
	//	return;
	//}
	//if (data.tag == "route3")
	//{
	//	m_enemyRoute.pos[3].push_back(data.pos);
	//	return;
	//}
	//if (data.tag == "route4")
	//{
	//	m_enemyRoute.pos[4].push_back(data.pos);
	//	return;
	//}
	//if (data.tag == "route5")
	//{
	//	m_enemyRoute.pos[6].push_back(data.pos);
	//	return;
	//}
	//if (data.tag == "route6")
	//{
	//	m_enemyRoute.pos[7].push_back(data.pos);
	//	return;
	//}
	//if (data.tag == "route7")
	//{
	//	m_enemyRoute.pos[8].push_back(data.pos);
	//	return;
	//}
	//if (data.tag == "route8")
	//{
	//	m_enemyRoute.pos[9].push_back(data.pos);
	//	return;
	//}
	//if (data.tag == "route9")
	//{
	//	m_enemyRoute.pos[10].push_back(data.pos);
	//	return;
	//}
	//if (data.tag == "route10")
	//{
	//	m_enemyRoute.pos[5].push_back(data.pos);
	//	return;
	//}

}
