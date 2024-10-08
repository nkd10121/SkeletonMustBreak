#pragma once
#include <List>
#include "Vec3.h"

namespace MyLib
{

/// <summary>
/// デバッグ用の描画情報をまとめ、後で表示するためのクラス
/// </summary>
class DebugDraw
{
public:
	//描画情報の削除
	static void Clear();
	//描画
	static void Draw3D();

	//球体の描画リストに追加する
	static void AddDrawSphere(const Vec3& center,const float& radius,const unsigned int& color);
private:
	//球体の描画情報
	struct SphereInfo
	{
		Vec3 center;		//中心座標
		float radius;		//半径
		unsigned int color; //色
	};

	//球体の描画情報リスト
	static std::list<SphereInfo> m_sphereInfo;
};

}