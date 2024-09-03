#include "GameManager.h"
#include "EnemyManager.h"
#include "Camera.h"
#include "Player.h"
#include "TrapGrid.h"
#include "TrapManager.h"

#include "Input.h"
#include "SceneGame.h"
#include "MapDataLoad.h"
#include "Font.h"

namespace
{
	//オブジェクトの最大HP
	constexpr int kObjectHPMax = 10;
	
	constexpr float kHpBarScale = 0.225f;

	//フォントのパス
	const char* kFontPath = "data/font/Dela-Gothic-One.ttf";
}

GameManager::GameManager():
	m_objectHP(kObjectHPMax),
	m_nowPhaseName(ePhaseName::FirstPrePhase),
	m_phaseCount(0),
	m_allCount(0),
	m_playerDownCount(0),
	m_isThisSceneEnd(false),
	m_trapPoint(1000),
	m_playerSlotNum(0),
	m_hpBarWidth(0),
	m_hpBarHeight(0),
	m_playerHp(0),
	m_playerMaxHp(80)
{
	MapDataLoad::GetInstance().Load("stage1");

	m_pPhysics = std::make_shared<MyLib::Physics>(MapDataLoad::GetInstance().GetStageCollisionHandle());

	m_arrowHandle = MV1LoadModel("data/model/arrow.mv1");

	m_pPlayer = std::make_shared<Player>();
	m_pPlayer->Init(m_pPhysics, &m_arrowHandle);
	m_pPlayer->SetSlotNumMax(1);

	m_pEnemyManager = std::make_shared<EnemyManager>();
	m_pEnemyManager->Init();

	m_pCamera = std::make_shared<Camera>();
	m_pCamera->Init();

	m_pTrapGrid = std::make_shared<TrapGrid>();
	m_pTrapGrid->Init();

	std::list<MyLib::Vec3> pos = MapDataLoad::GetInstance().GetTrapPos();
	//pos.emplace_back(MyLib::Vec3(12.0f,0.0f,12.0f));
	//pos.emplace_back(MyLib::Vec3(-12.0f,0.0f,12.0f));
	//pos.emplace_back(MyLib::Vec3(12.0f,0.0f,-12.0f));
	//pos.emplace_back(MyLib::Vec3(-12.0f,0.0f,-12.0f));

	m_pTrapManager = std::make_shared<TrapManager>(2, pos);
	m_pTrapManager->Init();

	m_objectUIBgHandle = LoadGraph("data/img/ui_button.png");

	//アイテムスロットの画像をロード
	m_slotHandle.push_back(LoadGraph("data/img/ui_slot_crossbow.png"));
	m_slotHandle.push_back(LoadGraph("data/img/ui_slot_spikeTrap.png"));

	//HPバーの画像をロード
	m_hpBarHandle.push_back(LoadGraph("data/img/ui_hpBar_base.png"));
	m_hpBarHandle.push_back(LoadGraph("data/img/ui_hpBar_bar.png"));
	m_hpBarHandle.push_back(LoadGraph("data/img/ui_hpBar_cover.png"));

	//HPバーの画像サイズを取得する
	GetGraphSize(m_hpBarHandle[1],&m_hpBarWidth, &m_hpBarHeight);

	m_fontHandle = Font::GetInstance().GetFontHandle(kFontPath, "Dela Gothic One", 24);;
}

GameManager::~GameManager()
{
	MV1DeleteModel(m_arrowHandle);
}

void GameManager::Init()
{

}

void GameManager::Update(std::shared_ptr<Input>& input, SceneGame* pSceneGame)
{

	if (m_nowPhaseName == ePhaseName::GameClear)
	{
		pSceneGame->ToResultScene(true);
		return;
	}
	else if (m_nowPhaseName == ePhaseName::Gameover)
	{
		pSceneGame->ToResultScene(false);
		return;
	}
	
	m_pEnemyManager->Update(input, m_pPhysics,this,m_pPlayer->IsGetPlayerDown());


	m_pCamera->SetPlayerPos(m_pPlayer->GetPos());
	m_pCamera->Update(input, MapDataLoad::GetInstance().GetStageCollisionHandle());

	m_pPlayer->SetCameraAngle(m_pCamera->GetDirection());
	m_pPlayer->Update(input);

	m_pTrapManager->Update(input,m_pPlayer->GetNowSlotNum(), m_pPlayer->GetPos(), m_pCamera->GetDirection(), &m_trapPoint, m_nowPhaseName, m_pPhysics);

	// 物理更新
	m_pPhysics->Update();

	//プレイヤーの情報を取得
	m_playerSlotNum = m_pPlayer->GetNowSlotNum();	//現在選択しているスロットの番号
	m_playerHp = m_pPlayer->GetHp();				//現在のプレイヤーのHP

	//プレイヤーの最大Hpの値を取得
	if (m_playerMaxHp == 0)
	{
		m_playerMaxHp = m_playerHp;
	}

	if (m_pPlayer->IsGetPlayerDown())
	{
		m_playerDownCount++;

		//if (m_objectHP > 5)
		{
			if (m_playerDownCount >= 100)
			{
				pSceneGame->StartFadeOut();
			}
		}

		//プレイヤーが死んだとき
		if (m_pPlayer->IsGetPlayerDead())
		{
			if (m_objectHP <= 5)
			{
				//復活できない状況だったらゲームオーバーに
				m_nowPhaseName = ePhaseName::Gameover;
			}

			//オブジェクトHPが5以上あれば5消費してプレイヤーを復活させる
			if (m_objectHP > 5)
			{
				m_objectHP -= 5;


				m_pPlayer = nullptr;

				m_pPlayer = std::make_shared<Player>();
				m_pPlayer->Init(m_pPhysics, &m_arrowHandle);
			}


		}
	}
	else
	{
		m_playerDownCount = 0;
	}

	//オブジェクトHPが0以下になったらゲームオーバーにする
	if (m_objectHP <= 0)
	{
		m_nowPhaseName = ePhaseName::Gameover;
	}



	//現在のフェーズのカウントを進める
	m_phaseCount++;

	//最初にOKボタンを押したら最初の準備フェーズを終了する
	if (input->IsTriggered("Y"))
	{
		//本来は最初のフェーズのみプレイヤーの任意のタイミングで次のフェーズに進行させる
		//TODO:フェーズごとに敵を生成して、そのフェーズの敵を全部倒したら次のフェーズに進行する処理を作る
		//		そしたらこの部分の処理を戻す
		//if (m_nowPhaseName == ePhaseName::FirstBattlePhase)
		//if (m_nowPhaseName != ePhaseName::SecondPrePhase && m_nowPhaseName != ePhaseName::ThirdPrePhase)
		{
			ChangeNextPhase();
		}
	}

	//準備フェーズに入ったら10秒後に戦闘フェーズに入るようにする
	if (m_nowPhaseName == ePhaseName::SecondPrePhase || m_nowPhaseName == ePhaseName::ThirdPrePhase)
	{
		if (m_phaseCount >= 600)
		{
			ChangeNextPhase();
		}
	}


}

void GameManager::Draw()
{
	MapDataLoad::GetInstance().Draw();

	m_pTrapManager->PreviewDraw();
	m_pTrapManager->Draw();

	m_pPlayer->Draw();
	m_pEnemyManager->Draw();

	DrawCircle(640, 390, m_pPlayer->GetDifAngle(), 0xffffff, false);

	//スロットアイテムのカーソル描画
	int center = 320 + m_playerSlotNum * 80;
	DrawBox(center + 34, 660 + 34, center - 34, 660 - 34, 0x0000aa, false);

	//スロットアイテムの描画
	for (int i = 0; i < m_slotHandle.size(); i++)
	{
		DrawRotaGraph(320 + i*80, 660, 1.0f, 0.0f, m_slotHandle[i], true);
	}

	//HPバーの計算
	//プレイヤーのHP÷プレイヤーの最大HPで現在のHPの割合を出す
	float widthScale = static_cast<float>(m_playerHp) / static_cast<float>(m_playerMaxHp);
	//HPの割合分の画像のX幅を出す
	int width = m_hpBarWidth * widthScale;
	//何もしないと画像が中心によっていくため画像の中心座標をHPに応じてずらす
	int posX = 240 - (m_hpBarWidth - width) * kHpBarScale / 2;

	//Hpバーの描画
	DrawRotaGraph(220, 30, kHpBarScale, 0.0f, m_hpBarHandle[0], true);
	DrawRectRotaGraph(posX, 30, 0,0, width,m_hpBarHeight, kHpBarScale,0.0f, m_hpBarHandle[1], true);
	DrawRotaGraph(236, 25, kHpBarScale, 0.0f, m_hpBarHandle[2], true);

	//オブジェクトの残りHPの描画
	DrawRotaGraph(1200, 40, 0.28f, 0.0f, m_objectUIBgHandle, true);
	//文字列の桁数が変わると中心がずれるため補正
	auto offset = ((1 - (m_objectHP / 10)) * 8);
	DrawFormatStringToHandle(1182 + offset, 27, 0xffffff, m_fontHandle, "%d", m_objectHP);
	//罠ポイントの描画
	DrawFormatStringToHandle(80, 680,0xffffff, m_fontHandle,"%d",m_trapPoint);

#ifdef _DEBUG
	DebugDraw();

	//座標を核にするためだけ
	m_pCamera->DebugDraw();
	DrawFormatString(1000, 0, 0xff0000, "オブジェクトHP:%d", m_objectHP);
	DrawFormatString(1000, 16, 0xff0000, "罠ポイント:%d", m_trapPoint);
	//DrawFormatString(1000, 16, 0xff0000, "プレイヤーダウンカウント:%d", m_playerDownCount);

#endif
}

void GameManager::DebugDraw()
{
	if(m_nowPhaseName == ePhaseName::FirstPrePhase)
	{
		DrawFormatString(560, 10, 0xff0000, "第1準備フェーズ");
	} 
	else if(m_nowPhaseName == ePhaseName::FirstBattlePhase)
	{
		DrawFormatString(560, 10, 0xff0000, "第1戦闘フェーズ");
	}
	else if(m_nowPhaseName == ePhaseName::SecondPrePhase)
	{
		DrawFormatString(560, 10, 0xff0000, "第2準備フェーズ");
	}
	else if(m_nowPhaseName == ePhaseName::SecondBattlePhase)
	{
		DrawFormatString(560, 10, 0xff0000, "第2戦闘フェーズ");
	}
	else if(m_nowPhaseName == ePhaseName::ThirdPrePhase)
	{
		DrawFormatString(560, 10, 0xff0000, "第3準備フェーズ");
	}
	else if(m_nowPhaseName == ePhaseName::ThirdBattlePhase)
	{
		DrawFormatString(560, 10, 0xff0000, "第3戦闘フェーズ");
	}
	else if(m_nowPhaseName == ePhaseName::GameClear)
	{
		DrawFormatString(560, 10, 0xff0000, "ゲームクリア");
	}
	else if(m_nowPhaseName == ePhaseName::Gameover)
	{
		DrawFormatString(560, 10, 0xff0000, "ゲームオーバー");
	}

	DrawFormatString(560, 30, 0xff0000, "現在のフェーズのタイム : %d",m_phaseCount);
	DrawFormatString(560, 50, 0xff0000, "開始からのタイム : %d",m_allCount);

}

void GameManager::ChangeNextPhase()
{
	m_nowPhaseName = static_cast<ePhaseName>(m_nowPhaseName + 1);
	m_allCount += m_phaseCount;
	m_phaseCount = 0;
}

const MyLib::Vec3 GameManager::GetPlayerPos()const
{
	return m_pPlayer->GetPos();
}

void GameManager::KilledEnemy(int point)
{
	m_trapPoint += point;

	//コンボクールタイムとかを作りたい
}
