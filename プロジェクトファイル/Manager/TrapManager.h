#pragma once
#include <map>
#include <memory>
#include <vector>
#include <list>
#include "MyLib.h"
#include "Vec3.h"

class TrapBase;
class Input;

/// <summary>
/// トラップ関係を管理するクラス
/// </summary>
class TrapManager
{
public:
	//コンストラクタ
	TrapManager(int trapNum,std::list<MyLib::Vec3>& trapPoss);
	//デストラクタ
	~TrapManager();

	//初期化
	void Init();
	//更新
	void Update(std::shared_ptr<Input>& input,int slotNum,MyLib::Vec3 playerPos, MyLib::Vec3 playerVec,bool isPlayerSurvival,int* trapPoint,int nowPhase, std::shared_ptr<MyLib::Physics> physics);
	//描画
	void Draw();

	//トラップのどこに設置するか見るための描画
	void PreviewDraw();

private:
	//トラップの生成
	bool CreateTrap(int slotNum, std::shared_ptr<MyLib::Physics> physics, int* trapPoint);

private:
	//モデルハンドルを格納する
	std::vector<int> m_modelHandles;

	//座標と罠を格納する
	std::list<std::shared_ptr<TrapBase>> m_traps;

	//罠設置可能座標を格納する
	std::list<MyLib::Vec3> m_trapPositions;

	//設置予定座標を格納
	MyLib::Vec3 m_previewPos;

	//現在のスロットインデックス
	int m_nowSlotIdx;

	//すでに罠が置かれているかどうか
	bool m_isAlreadyPlaced;

	//罠を点滅させるためのアングル
	float m_angle;
	//罠の透明度
	float m_transparency;	

	//ZLを押している時間
	int m_pushZLCount;

};

