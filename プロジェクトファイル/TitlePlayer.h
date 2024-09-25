#pragma once
#include "Vec3.h"

class TitlePlayer
{
public:
	TitlePlayer();
	~TitlePlayer();

	void Init(MyLib::Vec3 pos);
	void Update();
	void Draw();

private:
	/// <summary>
	/// アニメーションの更新処理
	/// </summary>
	/// <param name="attachNo">進行させたいアニメーション番号</param>
	/// <returns>ループしたかどうか</returns>
	bool UpdateAnim(int attachNo, float startTime = 0.0f);

	/// <summary>
	///	アニメーションの変更
	/// </summary>
	/// <param name="animIndex">変更後のアニメーション番号</param>
	void ChangeAnim(int animIndex, float animSpeed = 0.5f);

private:
	int m_modelHandle;
	MyLib::Vec3 m_pos;

	/*アニメーション情報*/
	int m_currentAnimNo;
	int m_prevAnimNo;
	float m_animBlendRate;
	float m_animSpeed;		//アニメーション再生速度

};

