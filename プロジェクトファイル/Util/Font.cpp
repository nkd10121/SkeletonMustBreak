#include "Font.h"
#include "DxLib.h"

Font* Font::m_instance = nullptr;

namespace
{
	const std::string kPathFront = "data/font/";
}

Font::~Font()
{
	for (auto path : m_paths)
	{
		// ********** �t�H���g�̃A�����[�h **********
		if (RemoveFontResourceEx(path.c_str(), FR_PRIVATE, NULL))
		{
		}
	}

	for (auto handle : m_handles)
	{
		DeleteFontToHandle(handle.second);
	}

	m_handles.clear();
	m_paths.clear();
}

/// <summary>
/// �t�H���g�̃n���h�����擾����
/// </summary>
/// <param name="path">�p�X</param>
/// <param name="name">���O</param>
/// <param name="fontSize">�t�H���g�T�C�Y</param>
/// <returns></returns>
int Font::GetFontHandle(std::string path, std::string name, int fontSize)
{
	for (const auto& handle : m_handles)
	{
		auto& data = handle.first;

		//���ɓ��������̃n���h�������݂��Ă����炻�̃n���h����Ԃ�
		if (data.first == name && data.second == fontSize)
		{
			return handle.second;
		}
	}

	//�������X�L�b�v���邩�ǂ���
	bool isPreSkip = false;

	for (auto path : m_paths)
	{
		if (path == path)
		{
			isPreSkip = true;
		}
	}

	if (!isPreSkip)
	{
		//�g�p����t�H���g����������
		if (AddFontResourceEx(path.c_str(), FR_PRIVATE, NULL) > 0) {
		}
		else {
			// �t�H���g�Ǎ��G���[����
			MessageBox(NULL, "�t�H���g�Ǎ����s", "", MB_OK);
		}

		m_paths.push_back(path);
	}
	

	//������ʂ�Ƃ��͓��������̃n���h�������݂��Ȃ������Ƃ�
	int ret = -1;
	ret = CreateFontToHandle(name.c_str(), fontSize, -1);

	std::pair<std::string, int> add;
	add.first = name;
	add.second = fontSize;

	m_handles[add] = ret;

	return ret;
}