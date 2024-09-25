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
	//ƒ‚ƒfƒ‹ƒnƒ“ƒhƒ‹‚ğŠi”[‚·‚é
	std::vector<int> m_modelHandles;

	//À•W‚Æã©‚ğŠi”[‚·‚é
	std::list<std::shared_ptr<TrapBase>> m_traps;

	//ã©İ’u‰Â”\À•W‚ğŠi”[‚·‚é
	std::list<MyLib::Vec3> m_trapPositions;

	//İ’u—\’èÀ•W‚ğŠi”[
	MyLib::Vec3 m_previewPos;

	int m_slotNum;

	//‚·‚Å‚Éã©‚ª’u‚©‚ê‚Ä‚¢‚é‚©‚Ç‚¤‚©
	bool m_isAlreadyPlaced;

	float m_angle;
	float m_transparency;	//“§–¾“x

	int m_pushZLCount;

};

