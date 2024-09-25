#pragma once

/// <summary>
/// 3D���f�����Ǘ�����N���X
/// </summary>
class ModelManager
{
	//1.���̃V�[���ŗʎY����郂�f�������炩���߃��[�h���Ă���
	//2.�~�������f���̃n���h��������΂��̃N���X�ɐ\�����Ď擾����
	//3.�V�[���؂�ւ����ƂɃn���h����Delete����

private:
	// �V���O���g���p�^�[���Ȃ̂ŃR���X�g���N�^��private�ɒu��
	ModelManager() {};

public:
	virtual ~ModelManager() {};

	//�R�s�[�R���X�g���N�^������̂̐������ł��Ă��܂�����
	//�R�s�[�R���X�g���N�^���֎~����
	ModelManager(const ModelManager&) = delete;
	ModelManager& operator=(const ModelManager&) = delete;
	ModelManager(ModelManager&&) = delete;
	ModelManager& operator= (const ModelManager&&) = delete;

	/// <summary>
	/// ModelManager��GetInstance()��ʂ����Q�Ƃ��炵�����p�ł��Ȃ�
	/// </summary>
	/// <returns></returns>
	static ModelManager& GetInstance()
	{
		if (!m_instance)
		{
			m_instance = new ModelManager;
		}

		return *m_instance;
	}

	//��������Y���ƕ��ʂɃ��������[�N
	static void Destroy()
	{
		delete m_instance;
		m_instance = nullptr;
	}

private:
	//static�ɂ��邱�Ƃ�
	//Singleton�̃|�C���^���v���O�����N�����Ɉ�����悤�ɂ���
	static ModelManager* m_instance;
};

