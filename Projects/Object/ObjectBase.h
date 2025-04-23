#pragma once
#include "DxLib.h"
#include "MyLib.h"
#include "Vec3.h"
#include <memory>

class Input;

class ObjectBase : public MyLib::Collidable
{
public:
	ObjectBase(Priority priority, GameObjectTag tag);
	virtual ~ObjectBase();

	virtual void Init() {};
	virtual void Update() {};
	virtual void Update(std::shared_ptr<Input> input) {};
	virtual void Draw() {};
protected:
	//角度
	MyLib::Vec3 m_rot;
	//モデルハンドル
	int m_modelHandle;
};

