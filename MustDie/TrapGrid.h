#pragma once
#include "DxLib.h"
#include "Vec3.h"
#include <list>

class TrapGrid
{
public:
	TrapGrid();
	~TrapGrid();

	void Init();
	void Update();
	void DebugDraw();
	
private:
	struct Grid
	{
		MyLib::Vec3 centerPos;
		bool isEmpty;
	};

	std::list<Grid> m_grids;

};

