#pragma once
#include "CharacterBase.h"
#include "EnemyManager.h"

#include <string>
#include <memory>

namespace LoadData
{
	//�X�e�[�^�X���̕��ї񋓌^
	enum eStatusOrder : int
	{
		name,	//�L�����N�^�[��
		hp,		//�̗�
		atk,		//�U����
		def		//�h���
	};

	//�X�e�[�^�X���̕��ї񋓌^
	enum eStageOrder : int
	{
		ID,						//�GID
		phase,					//�o���t�F�[�Y
		frame,					//�o���t���[��
		AppearanceLocation,		//�o���ꏊ
	};
}

class CsvLoad
{
private:
	// �V���O���g���p�^�[���Ȃ̂ŃR���X�g���N�^��private�ɒu��
	CsvLoad() {};

public:
	virtual ~CsvLoad() {};

	//�R�s�[�R���X�g���N�^������̂̐������ł��Ă��܂�����
	//�R�s�[�R���X�g���N�^���֎~����
	CsvLoad(const CsvLoad&) = delete;
	CsvLoad& operator=(const CsvLoad&) = delete;
	CsvLoad(CsvLoad&&) = delete;
	CsvLoad& operator= (const CsvLoad&&) = delete;

	/// <summary>
	/// CsvLoad��GetInstance()��ʂ����Q�Ƃ��炵�����p�ł��Ȃ�
	/// </summary>
	/// <returns></returns>
	static CsvLoad& GetInstance()
	{
		if (!m_instance)
		{
			m_instance = new CsvLoad;
		}

		return *m_instance;
	}

	//��������Y���ƕ��ʂɃ��������[�N
	static void Destroy()
	{
		delete m_instance;
		m_instance = nullptr;
	}

public:
	/// <summary>
	/// �X�e�[�^�X��񃍁[�h
	/// </summary>
	/// <param name="Data">�X�e�[�^�X���</param>
	/// <param name="charcterName">�L�����N�^�[�̖��O</param>
	void StatusLoad(CharacterBase::Status& data, const char* charcterName);

	/// <summary>
	/// �X�e�[�W��񃍁[�h
	/// </summary>
	/// <param name="Data">�X�e�[�^�X���</param>
	/// <param name="charcterName">�L�����N�^�[�̖��O</param>
	void StageEnenyDataLoad(const char* stageName, std::list<std::shared_ptr<EnemyManager::EnemyGenerateInfo>>& pGenerateInfo);

private:
	//static�ɂ��邱�Ƃ�
	//Singleton�̃|�C���^���v���O�����N�����Ɉ�����悤�ɂ���
	static CsvLoad* m_instance;

};

