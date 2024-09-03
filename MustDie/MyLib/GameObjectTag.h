#pragma once

// 当たったものの判別を行うためのタグ
enum class GameObjectTag
{
	Player,			// プレイヤー
	Enemy,			// 敵
	Search,
	Shot,			//弾
	Sword,			//武器
	SpikeTrap,
};