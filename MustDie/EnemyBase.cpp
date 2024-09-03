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
	std::string message = "敵が";
#endif
	auto tag = colider->GetTag();
	switch (tag)
	{
	case GameObjectTag::Player:		//プレイヤーと当たった時
#ifdef _DEBUG
		message += "プレイヤー";
#endif
		m_isAttack = true;


		break;
	case GameObjectTag::Shot:	//プレイヤーが撃った弾と当たった時
#ifdef _DEBUG
		message += "弾";
#endif
		break;
	}

#ifdef _DEBUG
	message += "と当たった！\n";
	printfDx(message.c_str());
#endif
}

void EnemyBase::OnCollideStay(const std::shared_ptr<Collidable>& colider)
{
#ifdef _DEBUG
	std::string message = "敵が";
#endif
	auto tag = colider->GetTag();
	switch (tag)
	{
	case GameObjectTag::Player:		//プレイヤーと当たった時
		m_isAttack = true;

		break;
	case GameObjectTag::Shot:	//プレイヤーが撃った弾と当たった時
		break;
	}
}

void EnemyBase::OnTriggerEnter(const std::shared_ptr<Collidable>& colider)
{
#ifdef _DEBUG
	std::string message = "敵が";
#endif
	auto tag = colider->GetTag();
	switch (tag)
	{
	case GameObjectTag::Player:		//プレイヤーと当たった時
#ifdef _DEBUG
		message += "プレイヤー";
#endif

		break;
	case GameObjectTag::Shot:	//プレイヤーが撃った弾と当たった時
#ifdef _DEBUG
		message += "弾";
#endif
		if (!m_isAttack)
		{
			m_isKnock = true;
		}

		{	//なんか{}がないとエラー吐く
			Shot* col = dynamic_cast<Shot*>(colider.get());
			m_status.hp -= col->GetAtk() - m_status.def;
		}
		break;

	case GameObjectTag::SpikeTrap:	//罠と当たった時
		if (!m_isAttack)
		{
			m_isKnock = true;
		}
		SpikeTrap* c = dynamic_cast<SpikeTrap*>(colider.get());
		m_status.hp -= c->GetAtk() - m_status.def;
		break;
	}

#ifdef _DEBUG
	message += "と当たった！\n";
	printfDx(message.c_str());
#endif
}
