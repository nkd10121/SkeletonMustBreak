#include "TrapGrid.h"

TrapGrid::TrapGrid()
{
	for (int i = 0; i <= 2; i++)
	{
		for (int j = 0; j <= 2; j++)
		{
			Grid add;
			add.centerPos = MyLib::Vec3(static_cast<float>(-10 + i * 10),0.0f, static_cast<float>(-10 + j * 10));
			add.isEmpty = false;
			m_grids.emplace_back(add);
		}
	}


}

TrapGrid::~TrapGrid()
{
}

void TrapGrid::Init()
{
}

void TrapGrid::Update()
{

}

void TrapGrid::DebugDraw()
{
	for (auto& grid : m_grids)
	{
		DrawSphere3D(grid.centerPos.ConvertToVECTOR(), 2, 4, 0x0000ff, 0x0000ff, false);
	}

	for (int x = -55; x <= 55; x += 10)
	{
		DrawLine3D(VGet(static_cast<float>(x), 0, -50), VGet(static_cast<float>(x), 0, 50), 0xffff00);
	}
	for (int z = -55; z <= 55; z += 10)
	{
		DrawLine3D(VGet(-50, 0, static_cast<float>(z)), VGet(50, 0, static_cast<float>(z)), 0xff0000);
	}
}
