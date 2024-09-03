#pragma once
#include <map>
#include <string>
#include <list>

struct FontData
{

};

class Font
{
private:
	// �V���O���g���p�^�[���Ȃ̂ŃR���X�g���N�^��private�ɒu��
	Font() {};

public:
	virtual ~Font();

	//�R�s�[�R���X�g���N�^������̂̐������ł��Ă��܂�����
	//�R�s�[�R���X�g���N�^���֎~����
	Font(const Font&) = delete;
	Font& operator=(const Font&) = delete;
	Font(Font&&) = delete;
	Font& operator= (const Font&&) = delete;

	/// <summary>
	/// Font��GetInstance()��ʂ����Q�Ƃ��炵�����p�ł��Ȃ�
	/// </summary>
	/// <returns></returns>
	static Font& GetInstance()
	{
		if (!m_instance)
		{
			m_instance = new Font;
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
	//�t�H���g�̃n���h�����擾����
	int GetFontHandle(std::string path,std::string name, int fontSize);

private:
	//static�ɂ��邱�Ƃ�
	//Singleton�̃|�C���^���v���O�����N�����Ɉ�����悤�ɂ���
	static Font* m_instance;

private:
	std::map<std::pair<std::string, int>, int> m_handles;
	std::list < std::string> m_paths;
};

