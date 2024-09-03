#pragma once
#include "Vec3.h"
#include <string>
#include <vector>
#include <unordered_map>

class MapDataLoad
{
private:
	// �V���O���g���p�^�[���Ȃ̂ŃR���X�g���N�^��private�ɒu��
	MapDataLoad();

	struct EnemyRoute
	{
		MyLib::Vec3 start;
		std::vector<MyLib::Vec3> pos1;
		std::vector<MyLib::Vec3> pos2;
		std::vector<MyLib::Vec3> pos3;
		std::vector<MyLib::Vec3> pos4;
		std::vector<MyLib::Vec3> pos5;
		MyLib::Vec3 end;
	};


public:
	virtual ~MapDataLoad();

	//�R�s�[�R���X�g���N�^������̂̐������ł��Ă��܂�����
	//�R�s�[�R���X�g���N�^���֎~����
	MapDataLoad(const MapDataLoad&) = delete;
	MapDataLoad& operator=(const MapDataLoad&) = delete;
	MapDataLoad(MapDataLoad&&) = delete;
	MapDataLoad& operator= (const MapDataLoad&&) = delete;

	/// <summary>
	/// MapDataLoad��GetInstance()��ʂ����Q�Ƃ��炵�����p�ł��Ȃ�
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

	//��������Y���ƕ��ʂɃ��������[�N
	static void Destroy()
	{
		delete m_instance;
		m_instance = nullptr;
	}

public:
	void Load(const char* stageName);

	void Draw();

	const int GetStageCollisionHandle()const { return m_collsionHanmdle; }

	std::list<MyLib::Vec3> GetTrapPos()const { return m_trapPos; }

	void ResetStageData();

	EnemyRoute GetEnemyRoute() { return m_enemyRoute; }

private:
	//�z�u���f�[�^
	struct LocationData
	{
		std::string name;	//���O
		std::string tag;	//�^�O
		MyLib::Vec3 pos;	//���W
		MyLib::Vec3 rot;	//��]
		MyLib::Vec3 scale;	//�傫��
		int handle;
	};

	void LoadEnemyRoute(LocationData& data);

private:
	//static�ɂ��邱�Ƃ�
	//Singleton�̃|�C���^���v���O�����N�����Ɉ�����悤�ɂ���
	static MapDataLoad* m_instance;

private:


	std::vector<LocationData> m_data;

	std::unordered_map<std::string, int> m_models;

	std::list<MyLib::Vec3> m_trapPos;

	int m_collsionHanmdle;



	EnemyRoute m_enemyRoute;
};

