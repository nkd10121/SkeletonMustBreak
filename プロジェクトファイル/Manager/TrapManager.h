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
	//�R���X�g���N�^
	TrapManager(int trapNum,std::list<MyLib::Vec3>& trapPoss);
	//�f�X�g���N�^
	~TrapManager();

	//������
	void Init();
	//�X�V
	void Update(std::shared_ptr<Input>& input,int slotNum,MyLib::Vec3 playerPos, MyLib::Vec3 playerVec,bool isPlayerSurvival,int* trapPoint,int nowPhase, std::shared_ptr<MyLib::Physics> physics);
	//�`��
	void Draw();

	//�g���b�v�̂ǂ��ɐݒu���邩���邽�߂̕`��
	void PreviewDraw();

private:
	//�g���b�v�̐���
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

	//���݂̃X���b�g�C���f�b�N�X
	int m_nowSlotIdx;

	//���ł�㩂��u����Ă��邩�ǂ���
	bool m_isAlreadyPlaced;

	//㩂�_�ł����邽�߂̃A���O��
	float m_angle;
	//㩂̓����x
	float m_transparency;	

	//ZL�������Ă��鎞��
	int m_pushZLCount;

};

