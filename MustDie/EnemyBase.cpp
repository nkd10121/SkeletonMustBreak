#include "EnemyBase.h"
#include "Shot.h"
#include "SpikeTrap.h"
#include <string>

EnemyBase::EnemyBase(Priority priority)
	:CharacterBase(priority, GameObjectTag::Enemy),
	m_isExist(false),
	m_isReach(false),
	m_isAttack(false),
	m_isKnock(false)
{
}

EnemyBase::~EnemyBase()
{
}

void EnemyBase::OnCollideEnter(const std::shared_ptr<Collidable>& colider)
{
#ifdef _DEBUG
	std::string message = "�G��";
#endif
	auto tag = colider->GetTag();
	switch (tag)
	{
	case GameObjectTag::Player:		//�v���C���[�Ɠ���������
#ifdef _DEBUG
		message += "�v���C���[";
#endif
		m_isAttack = true;


		break;
	case GameObjectTag::Shot:	//�v���C���[���������e�Ɠ���������
#ifdef _DEBUG
		message += "�e";
#endif
		break;
	}

#ifdef _DEBUG
	message += "�Ɠ��������I\n";
	printfDx(message.c_str());
#endif
}

void EnemyBase::OnCollideStay(const std::shared_ptr<Collidable>& colider)
{
#ifdef _DEBUG
	std::string message = "�G��";
#endif
	auto tag = colider->GetTag();
	switch (tag)
	{
	case GameObjectTag::Player:		//�v���C���[�Ɠ���������
		m_isAttack = true;

		break;
	case GameObjectTag::Shot:	//�v���C���[���������e�Ɠ���������
		break;
	}
}

void EnemyBase::OnTriggerEnter(const std::shared_ptr<Collidable>& colider)
{
#ifdef _DEBUG
	std::string message = "�G��";
#endif
	auto tag = colider->GetTag();
	switch (tag)
	{
	case GameObjectTag::Player:		//�v���C���[�Ɠ���������
#ifdef _DEBUG
		message += "�v���C���[";
#endif

		break;
	case GameObjectTag::Shot:	//�v���C���[���������e�Ɠ���������
#ifdef _DEBUG
		message += "�e";
#endif
		if (!m_isAttack)
		{
			m_isKnock = true;
		}

		{	//�Ȃ�{}���Ȃ��ƃG���[�f��
			Shot* col = dynamic_cast<Shot*>(colider.get());
			m_status.hp -= col->GetAtk() - m_status.def;
		}
		break;

	case GameObjectTag::SpikeTrap:	//㩂Ɠ���������
		if (!m_isAttack)
		{
			m_isKnock = true;
		}
		SpikeTrap* c = dynamic_cast<SpikeTrap*>(colider.get());
		m_status.hp -= c->GetAtk() - m_status.def;
		break;
	}

#ifdef _DEBUG
	message += "�Ɠ��������I\n";
	printfDx(message.c_str());
#endif
}
