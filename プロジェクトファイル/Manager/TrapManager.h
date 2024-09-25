#pragma once
#include <map>
#include <memory>
#include <vector>
#include <list>
#include "MyLib.h"
#include "Vec3.h"

class TrapBase;
class Input;

class TrapManager
{
public:
	TrapManager(int trapNum,std::list<MyLib::Vec3>& trapPoss);
	~TrapManager();

	void Init();
	void Update(std::shared_ptr<Input>& input,int slotNum,MyLib::Vec3 playerPos, MyLib::Vec3 playerVec,bool isPlayerSurvival,int* trapPoint,int nowPhase, std::shared_ptr<MyLib::Physics> physics);
	void Draw();

	void PreviewDraw();

private:
	bool CreateTrap(int slotNum, std::shared_ptr<MyLib::Physics> physics, int* trapPoint);

private:
	//���f���n���h�����i�[����
	std::vector<int> m_modelHandles;

	//���W��㩂��i�[����
	std::list<std::shared_ptr<TrapBase>> m_traps;

	//㩐ݒu�\���W���i�[����
	std::list<MyLib::Vec3> m_trapPositions;

	//�ݒu�\����W���i�[
	MyLib::Vec3 m_previewPos;

	int m_slotNum;

	//���ł�㩂��u����Ă��邩�ǂ���
	bool m_isAlreadyPlaced;

	float m_angle;
	float m_transparency;	//�����x

	int m_pushZLCount;

};

