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
	void Update(std::shared_ptr<Input>& input,int slotNum,MyLib::Vec3 playerPos, MyLib::Vec3 playerVec,int* trapPoint,int nowPhase, std::shared_ptr<MyLib::Physics> physics);
	void Draw();

	void PreviewDraw();

private:
	//���f���n���h�����i�[����
	std::vector<int> m_modelHandles;

	//���W��㩂��i�[����
	std::list<std::shared_ptr<TrapBase>> m_traps;

	//㩐ݒu�\���W���i�[����
	std::list<MyLib::Vec3> m_trapPoss;

	//�ݒu�\����W���i�[
	MyLib::Vec3 m_previewPos;

	int m_slotNum;
};

