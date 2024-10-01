#pragma once
#include <List>
#include "Vec3.h"

namespace MyLib
{

/// <summary>
/// �f�o�b�O�p�̕`������܂Ƃ߁A��ŕ\�����邽�߂̃N���X
/// </summary>
class DebugDraw
{
public:
	//�`����̍폜
	static void Clear();
	//�`��
	static void Draw3D();

	//���̂̕`�惊�X�g�ɒǉ�����
	static void AddDrawSphere(const Vec3& center,const float& radius,const unsigned int& color);
private:
	//���̂̕`����
	struct SphereInfo
	{
		Vec3 center;		//���S���W
		float radius;		//���a
		unsigned int color; //�F
	};

	//���̂̕`���񃊃X�g
	static std::list<SphereInfo> m_sphereInfo;
};

}