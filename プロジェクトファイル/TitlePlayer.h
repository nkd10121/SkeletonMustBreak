#pragma once
#include "Vec3.h"

class TitlePlayer
{
public:
	TitlePlayer();
	~TitlePlayer();

	void Init(MyLib::Vec3 pos);
	void Update();
	void Draw();

private:
	/// <summary>
	/// �A�j���[�V�����̍X�V����
	/// </summary>
	/// <param name="attachNo">�i�s���������A�j���[�V�����ԍ�</param>
	/// <returns>���[�v�������ǂ���</returns>
	bool UpdateAnim(int attachNo, float startTime = 0.0f);

	/// <summary>
	///	�A�j���[�V�����̕ύX
	/// </summary>
	/// <param name="animIndex">�ύX��̃A�j���[�V�����ԍ�</param>
	void ChangeAnim(int animIndex, float animSpeed = 0.5f);

private:
	int m_modelHandle;
	MyLib::Vec3 m_pos;

	/*�A�j���[�V�������*/
	int m_currentAnimNo;
	int m_prevAnimNo;
	float m_animBlendRate;
	float m_animSpeed;		//�A�j���[�V�����Đ����x

};

