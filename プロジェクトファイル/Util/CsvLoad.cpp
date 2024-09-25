#include "CsvLoad.h"
#include <vector>
#include <sstream>
#include <fstream>
#include <cassert>

CsvLoad* CsvLoad::m_instance = nullptr;

namespace
{
	// csv�f�[�^��,�ŕ�����𕪊�����
	std::vector<std::string> Split(std::string& input, char delimiter)
	{
		std::istringstream stream(input);
		std::string field;
		std::vector<std::string> result;
		while (getline(stream, field, delimiter)) {
			result.push_back(field);
		}
		return result;
	}

	const std::string kFrontPathName = "data/csv/";
	const std::string kBackPathName = ".csv";
}

void CsvLoad::StatusLoad(CharacterBase::Status& data, const char* charcterName)
{
	// �ꎞ�ۑ��pstring
	std::string strBuf;
	// �J���}�����ꎞ�ۑ��pstring
	std::vector<std::string> strConmaBuf;

	// �t�@�C���ǂݍ���
	std::ifstream ifs("data/csv/status.csv");
	if (!ifs)
	{
		assert(false);
		return;
	}

	//�����擾�ł������ǂ����̃t���O
	bool isGet = false;

	//�ŏ��͑Ή��\��񂪓����Ă��邾���Ȃ̂Ŗ�������
	std::getline(ifs, strBuf);

	while (getline(ifs, strBuf))
	{
		//�擾������������J���}��؂�̔z��(���Q)�ɂ���
		strConmaBuf = Split(strBuf, ',');

		//[0]:�L�����N�^�[��
		//[1]:�̗�
		//[2]:�U����
		//[3]:�h���
		//[4]:�ړ����x
		//[5]:���S���̃|�C���g

		//�w�肵���L�����N�^�[���ƈ�v����f�[�^������Ώ����擾����
		if (strConmaBuf[LoadData::eStatusOrder::name] == charcterName)
		{
			isGet = true;
			data.hp = std::stoi(strConmaBuf[LoadData::eStatusOrder::hp]);
			data.atk = std::stoi(strConmaBuf[LoadData::eStatusOrder::atk]);
			data.def = std::stoi(strConmaBuf[LoadData::eStatusOrder::def]);
			data.speed = std::stof(strConmaBuf[LoadData::eStatusOrder::speed]);
			data.point = std::stoi(strConmaBuf[LoadData::eStatusOrder::point]);
		}
	}

	//�����擾�ł��Ȃ��������A�G���[��f���悤�ɂ���
	//�擾�ł��Ȃ��������R�z��
	//�@�L�����N�^�[�����X�y���~�X��啶���������̈Ⴂ�ňقȂ�
	//�Acsv�ɏ���ǉ����Y��Ă���
	if (!isGet)
	{
		assert(0 && "�w�肵���L�����N�^�[���̏����擾�ł��܂���ł���");
	}

	return;
}

void CsvLoad::StageEnenyDataLoad(const char* stageName, std::list<std::shared_ptr<EnemyManager::EnemyGenerateInfo>>& pGenerateInfo)
{
	// �ꎞ�ۑ��pstring
	std::string strBuf;
	// �J���}�����ꎞ�ۑ��pstring
	std::vector<std::string> strConmaBuf;

	std::string csvName = kFrontPathName + stageName + kBackPathName;

	// �t�@�C���ǂݍ���
	std::ifstream ifs(csvName);
	if (!ifs)
	{
		assert(false);
		return;
	}

	//�ŏ��͑Ή��\��񂪓����Ă��邾���Ȃ̂Ŗ�������
	std::getline(ifs, strBuf);

	while (getline(ifs, strBuf))
	{
		//�擾������������J���}��؂�̔z��(���Q)�ɂ���
		strConmaBuf = Split(strBuf, ',');

		//[0]:��������G��ID(���O)
		//[1]:�����t�F�[�Y
		//[2]:�����t���[��
		//[3]:�����ʒu

		std::shared_ptr<EnemyManager::EnemyGenerateInfo> add = std::make_shared<EnemyManager::EnemyGenerateInfo>();
		add->enemyName = strConmaBuf[LoadData::eStageOrder::ID];
		add->appearPhase = std::stoi(strConmaBuf[LoadData::eStageOrder::phase]);
		add->appearFrame = std::stoi(strConmaBuf[LoadData::eStageOrder::frame]);
		add->appearPos = std::stoi(strConmaBuf[LoadData::eStageOrder::AppearanceLocation]);

		pGenerateInfo.emplace_back(add);
	}

	return;
}
