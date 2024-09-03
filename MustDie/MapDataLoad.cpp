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

MapDataLoad::MapDataLoad():
	m_collsionHanmdle(-1)
{
	m_models["floor"] = MV1LoadModel((kModelPathFront + "floor" + kModelPathBack).c_str());
	m_models["wall"] = MV1LoadModel((kModelPathFront + "wall" + kModelPathBack).c_str());
	m_models["doorFrame"] = MV1LoadModel((kModelPathFront + "doorFrame" + kModelPathBack).c_str());
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

	MV1DeleteModel(m_collsionHanmdle);
}

void MapDataLoad::Load(const char* stageName)
{
	//�X�e�[�W�̓����蔻�胂�f�����擾�@
	std::string check = kStageDataPathFront + stageName + "_col" + kModelPathBack;
	m_collsionHanmdle = MV1LoadModel(check.c_str());
	MV1SetPosition(m_collsionHanmdle, VECTOR(0.0f, 0.0f, 0.0f));
	MV1SetScale(m_collsionHanmdle, VECTOR(0.01f, 0.01f, 0.01f));

	//�J���t�@�C���̃n���h�����擾
	int handle = FileRead_open((kStageDataPathFront + stageName + kStageDataPathBack).c_str());

	//�ǂݍ��ރI�u�W�F�N�g���������邩�擾
	int dataCnt = 0;
	FileRead_read(&dataCnt, sizeof(dataCnt), handle);
	//�ǂݍ��ރI�u�W�F�N�g�����̔z��ɕύX����
	m_data.resize(dataCnt);

	//�z��̐�����
	for (auto& loc : m_data)
	{
		//���O�̃o�C�g�����擾����
		byte nameCnt = 0;
		FileRead_read(&nameCnt, sizeof(nameCnt), handle);
		//���O�̃T�C�Y��ύX����
		loc.name.resize(nameCnt);
		//���O���擾����
		FileRead_read(loc.name.data(), sizeof(char) * loc.name.size(), handle);

		//�^�O�̃o�C�g�����擾����
		byte tagCnt = 0;
		FileRead_read(&tagCnt, sizeof(tagCnt), handle);
		//�^�O�̃T�C�Y��ύX����
		loc.tag.resize(tagCnt);
		//�^�O���擾����
		FileRead_read(loc.tag.data(), sizeof(char) * loc.tag.size(), handle);

		//���W���擾����
		FileRead_read(&loc.pos, sizeof(loc.pos), handle);
		//��]���擾����
		FileRead_read(&loc.rot, sizeof(loc.rot), handle);
		//�傫�����擾����
		FileRead_read(&loc.scale, sizeof(loc.scale), handle);

		if (loc.tag == "trapPos")
		{
			m_trapPos.push_back(loc.pos);
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
	int y = 0;
	for (auto& loc : m_data)
	{
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
		MV1DrawModel(loc.handle);
	}

#ifdef true
	MV1DrawModel(m_collsionHanmdle);
#endif
}

void MapDataLoad::ResetStageData()
{
	m_data.clear();
	m_trapPos.clear();
	m_enemyRoute.start = MyLib::Vec3();
	m_enemyRoute.pos1.clear(); 
	m_enemyRoute.pos2.clear(); 
	m_enemyRoute.pos3.clear(); 
	m_enemyRoute.pos4.clear(); 
	m_enemyRoute.pos5.clear(); 
	m_enemyRoute.end = MyLib::Vec3();
}

void MapDataLoad::LoadEnemyRoute(LocationData& data)
{
	if (data.tag == "enemyStartPos")
	{
		m_enemyRoute.start = data.pos;
	}
	if (data.tag == "route1")
	{
		m_enemyRoute.pos1.push_back(data.pos);
	}
	if (data.tag == "route2")
	{
		m_enemyRoute.pos2.push_back(data.pos);
	}
	if (data.tag == "route3")
	{
		m_enemyRoute.pos3.push_back(data.pos);
	}
	if (data.tag == "route4")
	{
		m_enemyRoute.pos4.push_back(data.pos);
	}
	if (data.tag == "route5")
	{
		m_enemyRoute.pos5.push_back(data.pos);
	}
	if (data.tag == "enemyEndPos")
	{
		m_enemyRoute.end = data.pos;
	}
}
