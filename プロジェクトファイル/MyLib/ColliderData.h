#pragma once

namespace MyLib
{

	/// <summary>
	/// �����蔻��̃f�[�^���
	/// </summary>
	class ColliderData abstract
	{
	public:
		// �����蔻����
		enum class Kind
		{
			Sphere,
			Line,
		};

		// �R���X�g���N�^
		ColliderData(Kind kind, bool isTrigger)
		{
			this->kind = kind;
			this->isTrigger = isTrigger;
		}

		virtual ~ColliderData() {}

		// �����蔻���ʎ擾
		Kind GetKind() const { return kind; }

		// �g���K�[���ǂ���
		bool IsTrigger() const { return isTrigger; }

	private:
		Kind	kind;
		bool	isTrigger;
	};

}