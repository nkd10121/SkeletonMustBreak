#pragma once
#include <DxLib.h>
#include <cmath>

namespace MyLib
{
	//�O�����x�N�g���������N���X
	class Vec3
	{
	public:
		float x, y, z;

	public:
		//�����Ȃ��R���X�g���N�^
		Vec3() :
			x(0.0f),
			y(0.0f),
			z(0.0f)
		{

		}

		//�����L��R���X�g���N�^
		Vec3(const float x, const float y, const float z)
		{
			this->x = x;
			this->y = y;
			this->z = z;
		}

		//�����Z
		Vec3 operator+(const Vec3& other)const
		{
			Vec3 ret;
			ret.x = this->x + other.x;
			ret.y = this->y + other.y;
			ret.z = this->z + other.z;
			return ret;
		}

		//�����Z
		Vec3 operator+=(const Vec3& other)
		{
			this->x += other.x;
			this->y += other.y;
			this->z += other.z;
			return *this;
		}

		//�����Z
		Vec3 operator-(const Vec3& other)const
		{
			Vec3 ret;
			ret.x = this->x - other.x;
			ret.y = this->y - other.y;
			ret.z = this->z - other.z;
			return ret;
		}

		//�����Z
		Vec3 operator-=(const Vec3& other)
		{
			this->x -= other.x;
			this->y -= other.y;
			this->z -= other.z;
			return *this;
		}

		Vec3 operator*(const float scale)
		{
			Vec3 ret;
			ret.x = this->x * scale;
			ret.y = this->y	* scale;
			ret.z = this->z * scale;
			return ret;
		}

		Vec3 operator=(const VECTOR& vec)
		{
			Vec3 ret;
			ret.x = vec.x;
			ret.y = vec.y;
			ret.z = vec.z;
			return ret;
		}

		//��r
		bool operator==(const Vec3& other)const
		{
			bool ret;
			ret = this->x == other.x && this->y == other.y && this->z == other.z;
			return ret;
		}

		//��r
		bool operator!=(const Vec3& other)const
		{
			bool ret;
			ret = this->x != other.x || this->y != other.y || this->z != other.z;
			return ret;
		}

		//�x�N�g���̃T�C�Y���擾����
		float Size()const
		{
			float ret;
			ret = sqrt(this->x * this->x + this->y * this->y + this->z * this->z);
			return ret;
		}

		//�x�N�g���̃T�C�Y�̂Q����擾����
		float SquareSize()const
		{
			float ret;
			ret = this->x * this->x + this->y * this->y + this->z * this->z;
			return ret;
		}

		//���K���x�N�g�����擾����
		Vec3 Normalize()const
		{
			Vec3 ret;
			float size = this->Size();
			if (size == 0.0f)
			{
				return ret;
			}
			ret.x = this->x / size;
			ret.y = this->y / size;
			ret.z = this->z / size;
			return ret;
		}

		//��̃x�N�g���̓��ς��擾����
		float Dot(const Vec3& other)const
		{
			float ret;
			ret = this->x * other.x + this->y * other.y + this->z * other.z;
			return ret;
		}

		//��̃x�N�g���̊O�ς��擾����
		Vec3 Cross(const Vec3& other)const
		{
			Vec3 ret;
			ret.x = this->y * other.z - this->z * other.y;
			ret.y = this->z * other.x - this->x * other.z;
			ret.z = this->x * other.y - this->y * other.x;
			return ret;
		}

		//DxLib��VECTOR�^�ɕϊ�
		DxLib::VECTOR ConvertToVECTOR()
		{
			DxLib::VECTOR ret = VGet(0.0f, 0.0f, 0.0f);
			ret.x = this->x;
			ret.y = this->y;
			ret.z = this->z;
			return ret;
		}
	};
}