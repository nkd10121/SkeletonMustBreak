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

	// �Փ˂����Ƃ�
	void OnCollideEnter(const std::shared_ptr<Collidable>& colider) = 0;

protected:
	//�p�x
	MyLib::Vec3 m_rot;
	//���f���n���h��
	int m_modelHandle;
};

