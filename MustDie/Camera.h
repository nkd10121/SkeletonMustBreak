#pragma once
#include "DxLib.h"
#include "Vec3.h"
#include <memory>
class Input;

class Camera
{
public:
	Camera();
	~Camera();

	void Init();
	void Update(std::shared_ptr<Input>& input,int stageHandle);

	void DebugDraw();

	const MyLib::Vec3 GetDirection()const;

	void SetPlayerPos(MyLib::Vec3 playerPos) { m_playerPos = playerPos; }

private:
	float m_cameraAngleX;
	float m_cameraAngleY;
	MyLib::Vec3 m_cameraPos;
	MyLib::Vec3 m_aimPos;
	MyLib::Vec3 m_playerPos;

	float m_angleMoveScale;

	VECTOR TestPosition;


	MV1_COLL_RESULT_POLY_DIM m_hitDim{};	//ìñÇΩÇËîªíËåãâ ç\ë¢ëÃ
};

