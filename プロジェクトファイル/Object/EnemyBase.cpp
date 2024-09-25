#include "EnemyBase.h"
#include "Shot.h"
#include "SpikeTrap.h"
#include "CutterTrap.h"



#include <string>



EnemyBase::EnemyBase(Priority priority)
	:CharacterBase(priority, GameObjectTag::Enemy),
	m_maxHp(0),
	m_isExist(false),
	m_isReach(false),
	m_isHit(false),
	m_isDead(false),
	m_dropPoint(0),
	m_isDroped(false),
	m_isAttack(false),
	m_isKnock(false),
	m_centerPos()
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
//#ifdef _DEBUG
//	std::string message = "�G��";
//#endif
//	auto tag = colider->GetTag();
//	switch (tag)
//	{
//	case GameObjectTag::Player:		//�v���C���[�Ɠ���������
//#ifdef _DEBUG
//		message += "�v���C���[";
//#endif
//
//		break;
//	case GameObjectTag::Shot:	//�v���C���[���������e�Ɠ���������
//#ifdef _DEBUG
//		message += "�e";
//#endif
//		if (!m_isAttack)
//		{
//			m_isKnock = true;
//		}
//
//		m_isHit = true;
//
//		{	//�Ȃ�{}���Ȃ��ƃG���[�f��
//			Shot* col = dynamic_cast<Shot*>(colider.get());
//			m_status.hp -= col->GetAtk() - m_status.def;
//		}
//		break;
//
//	case GameObjectTag::SpikeTrap:	//㩂Ɠ���������
//		if (!m_isAttack)
//		{
//			m_isKnock = true;
//		}
//		m_isHit = true;
//		{
//			SpikeTrap* spike = dynamic_cast<SpikeTrap*>(colider.get());
//			m_status.hp -= spike->GetAtk() - m_status.def;
//		}
//
//
//
//		break;
//
//	case GameObjectTag::CutterTrap:	//㩂Ɠ���������
//		if (!m_isAttack)
//		{
//			m_isKnock = true;
//		}
//		m_isHit = true;
//		{
//			CutterTrap* cutter = dynamic_cast<CutterTrap*>(colider.get());
//			m_status.hp -= cutter->GetAtk() - m_status.def;
//		}
//		break;
//	}
//
//#ifdef _DEBUG
//	message += "�Ɠ��������I\n";
//	printfDx(message.c_str());
//#endif
}

bool EnemyBase::GetIsHit()
{
	bool log = m_isHit;
	m_isHit = false;
	return log;
}

int EnemyBase::GetDropPoint()
{
	m_isDroped = true;
	return m_dropPoint;
}