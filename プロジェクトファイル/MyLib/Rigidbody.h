#pragma once
#include <DxLib.h>
#include "Vec3.h"

namespace MyLib
{

/// <summary>
/// �����A�Փ˔���ɕK�v�ȃf�[�^�̉�
/// </summary>
class Rigidbody
{
public:
	//�R���X�g���N�^
	Rigidbody();
	// ������
	void Init(bool m_useGravity = false);	

	// velocity�ɗ͂�������
	void AddForce(const Vec3& force);

	/*Getter*/
	//���ݍ��W���擾
	const Vec3& GetPos() const { return m_pos; }
	//�ړ��\����W
	const Vec3& GetNextPos() const { return m_nextPos; }
	//�������擾
	const Vec3& GetDir() const { return m_dir; }
	//�ړ����x���擾
	const Vec3& GetVelocity() const { return m_velocity; }

	//���ݍ��W���擾
	const VECTOR GetPosVECTOR() { return m_pos.ConvertToVECTOR(); }
	//�ړ��\����W���擾
	const VECTOR GetNextPosVECTOR() { return m_nextPos.ConvertToVECTOR(); }
	//�������擾
	const VECTOR GetDirVECTOR() { return m_dir.ConvertToVECTOR(); }
	//�ړ����x���擾
	const VECTOR GetVelocityVECTOR(){ return m_velocity.ConvertToVECTOR(); }

	//�d�͂�^���邩�ǂ����擾
	bool GetUseGravity() const { return m_useGravity; }		

	/*Setter*/
	//���ݍ��W��ݒ�
	void SetPos(const Vec3& set) { m_pos = set; }
	//�ړ��\����W��ݒ�
	void SetNextPos(const Vec3& set) { m_nextPos = set; }
	//������ݒ�
	void SetVelocity(Vec3 set,float mul = 1.0f);
	//�ړ����x��ݒ�
	void SetUseGravity(bool set) { m_useGravity = set; }

private:
	//���ݍ��W
	Vec3 m_pos;
	//�ړ��\����W
	Vec3 m_nextPos;

	//����
	Vec3 m_dir;
	//�ړ����x
	Vec3 m_velocity;
	//�d�͂�^���邩�ǂ���
	bool m_useGravity;
};
}