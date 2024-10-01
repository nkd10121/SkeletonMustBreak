#include "MyLib.h"
#include "DxLib.h"

std::list<MyLib::DebugDraw::SphereInfo>	MyLib::DebugDraw::m_sphereInfo;

/// <summary>
/// �`����̍폜
/// </summary>
void MyLib::DebugDraw::Clear()
{
	//���̂̕`���񃊃X�g�̒��g��S�폜
	m_sphereInfo.clear();
}

/// <summary>
/// �`��
/// </summary>
void MyLib::DebugDraw::Draw3D()
{
#ifdef _DEBUG
	//���̂̕`���񃊃X�g�ɂ����񕪕`�悷��
	for (auto& sphere : m_sphereInfo)
	{
		DrawSphere3D(sphere.center.ConvertToVECTOR(), sphere.radius, 8, sphere.color, sphere.color, false);
	}
#endif
}

/// <summary>
/// ���̂̕`�惊�X�g�ɒǉ�����
/// </summary>
/// <param name="center">���S���W</param>
/// <param name="radius">���a</param>
/// <param name="color">�F</param>
void MyLib::DebugDraw::AddDrawSphere(const Vec3& center, const float& radius, const unsigned int& color)
{
	//���̂̕`�惊�X�g�ɒǉ�����
	SphereInfo addInfo;
	addInfo.center = center;
	addInfo.radius = radius;
	addInfo.color = color;
	m_sphereInfo.emplace_back(addInfo);
}
