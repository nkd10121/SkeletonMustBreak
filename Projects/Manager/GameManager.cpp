#include "GameManager.h"
#include "EnemyManager.h"
#include "Camera.h"
#include "Player.h"
#include "TrapManager.h"
#include "HealPortion.h"

#include "Input.h"
#include "SceneGame.h"
#include "MapDataLoad.h"
#include "Font.h"
#include "EffectManager.h"
#include "SoundManager.h"

#include "SceneManager.h"	//scoreInfoを扱うために必要

namespace
{
	//オブジェクトの最大HP
	constexpr int kObjectHPMax = 20;
	constexpr int kTrapNum = 2;

	constexpr float kHpBarScale = 0.225f;

	//フォントのパス
	const char* kFontPath = "data/font/Dela-Gothic-One.ttf";
	constexpr int kFontSize = 24;

	const char* kStageName = "stage2";
	//const char* kStageName = "stageTest";
	
	constexpr int kMaxPortionNum = 5;

	//ミニマップ関係
	const char* kMiniMapFrontName = "data/img/miniMap_";
	const char* kMiniMapBackName = ".png";
	constexpr int kMiniMapX = 1080;
	constexpr int kMiniMapWidth = 180;
	constexpr int kMiniMapY = 70;
	constexpr int kMiniMapHeight = 180;

	const int kMiniMapOffsetY[3] = { 30,60,34 };

	constexpr int kCrystalHpGraph = 1205;
}

/// <summary>
/// コンストラクタ
/// </summary>
/// <param name="stageName">ステージ名</param>
GameManager::GameManager(std::string stageName) :
	m_stageName(stageName),
	m_pPlayer(nullptr),
	m_pEnemyManager(nullptr),
	m_pCamera(nullptr),
	m_pTrapManager(nullptr),
	m_pPortion(),
	m_potionNum(0),
	m_score(nullptr),
	m_playerKillCount(nullptr),
	m_trapKillCount(nullptr),
	m_uiHandle(-1),
	m_pPhysics(nullptr),
	m_objectHP(kObjectHPMax),
	m_objectUIBgHandle(-1),
	m_nowPhase(ePhaseName::FirstPrePhase),
	m_phaseFrame(0),
	m_allFrame(0),
	m_playerDownCount(0),
	m_isThisSceneEnd(false),
	m_trapPoint(1000),
	m_playerSlotIdx(0),
	m_hpBarWidth(0),
	m_hpBarHeight(0),
	m_playerHp(0),
	m_playerMaxHp(40),
	m_fontHandle(-1),
	m_isClear(false),
	m_effectFrame(0),
	m_miniMapHandle(-1),
	m_crystalPos(),
#ifdef _DEBUG
	m_drawPos(),
	m_frame(0),
#endif
	m_miniMapPlayerPos(),
	m_offsetY(0)
{
	MapDataLoad::GetInstance().ResetStageData();

	//MapDataLoad::GetInstance().Load("stageTest");
	MapDataLoad::GetInstance().Load(m_stageName.c_str());
	m_crystalPos = MapDataLoad::GetInstance().GetCrystalPos();

	m_pPhysics = std::make_shared<MyLib::Physics>(MapDataLoad::GetInstance().GetStageCollisionHandle(), MapDataLoad::GetInstance().GetStageEnemyCollisionHandle());

	m_pPlayer = std::make_shared<Player>();
	m_pPlayer->Init(m_pPhysics);
	m_pPlayer->SetSlotNumMax(kTrapNum);


	m_pCamera = std::make_shared<Camera>();
	m_pCamera->Init();

	std::list<MyLib::Vec3> pos = MapDataLoad::GetInstance().GetTrapPos();

	m_pTrapManager = std::make_shared<TrapManager>(kTrapNum + 1, pos);
	m_pTrapManager->Init();

	m_objectUIBgHandle = LoadGraph("data/img/ui_button.png");

	//アイテムスロットの画像をロード
	m_slotHandle.push_back(LoadGraph("data/img/ui_slot_crossbow.png"));
	m_slotHandle.push_back(LoadGraph("data/img/ui_slot_spikeTrap.png"));
	m_slotHandle.push_back(LoadGraph("data/img/ui_slot_cutterTrap.png"));

	//HPバーの画像をロード
	m_hpBarHandle.push_back(LoadGraph("data/img/ui_hpBar_base.png"));
	m_hpBarHandle.push_back(LoadGraph("data/img/ui_hpBar_bar.png"));
	m_hpBarHandle.push_back(LoadGraph("data/img/ui_hpBar_bar_bg.png"));
	m_hpBarHandle.push_back(LoadGraph("data/img/ui_hpBar_cover.png"));

	m_uiHandle = LoadGraph("data/img/ui_trapPointBG.png");

	if(m_stageName == "stage1")
	{
		m_offsetY = kMiniMapOffsetY[0];
	}
	else if (m_stageName == "stage2")
	{
		m_offsetY = kMiniMapOffsetY[1];
	}
	else if (m_stageName == "stage3")
	{
		m_offsetY = kMiniMapOffsetY[2];
	}
	m_miniMapHandle = LoadGraph((kMiniMapFrontName + m_stageName + kMiniMapBackName).c_str());

	//HPバーの画像サイズを取得する
	GetGraphSize(m_hpBarHandle[1], &m_hpBarWidth, &m_hpBarHeight);

	m_fontHandle = Font::GetInstance().GetFontHandle(kFontPath, "Dela Gothic One", kFontSize);
	m_trapPointHandle = Font::GetInstance().GetFontHandle(kFontPath, "Dela Gothic One", 36);

	std::string front = "data/effect/warningArea_";
	std::string  back = ".efk";
	EffectManager::GetInstance().Load("WarningArea", (front + m_stageName + back).c_str(),120);

	SoundManager::GetInstance().Load("enemyAppear", "data/sound/se/enemyAppear.mp3", false);
	SoundManager::GetInstance().Load("playerHit", "data/sound/se/playerHit.mp3", false);
	SoundManager::GetInstance().Load("heal", "data/sound/se/heal.mp3", false);
	SoundManager::GetInstance().Load("spike", "data/sound/se/spikeTrap.mp3", false);
	SoundManager::GetInstance().Load("trap", "data/sound/se/trap.mp3", false);
	SoundManager::GetInstance().Load("shot", "data/sound/se/shot.mp3", false);
	SoundManager::GetInstance().Load("walk", "data/sound/se/walk.mp3", false);
	SoundManager::GetInstance().Load("jump", "data/sound/se/jump.mp3", false);
	SoundManager::GetInstance().Load("revival", "data/sound/se/revival.mp3", false);
	SoundManager::GetInstance().Load("crystal", "data/sound/se/crystal.mp3", false);
	SoundManager::GetInstance().Load("clear", "data/sound/se/clear.mp3", false);

	SoundManager::GetInstance().Load("inGameBgm", "data/sound/bgm/inGame.mp3", true);

	//タイトルのbgmがなっていたら止める
	SoundManager::GetInstance().StopBGM("titleBgm");
}


/// <summary>
/// デストラクタ
/// </summary>
GameManager::~GameManager()
{
	m_stageName.clear();
	m_pPortion.clear();

	m_playerKillCount = nullptr;
	delete m_playerKillCount;
	m_trapKillCount = nullptr;
	delete m_trapKillCount;

	DeleteGraph(m_uiHandle);
	
	DeleteGraph(m_objectUIBgHandle);

	for (auto& h : m_slotHandle)
	{
		DeleteGraph(h);
	}
	m_slotHandle.clear();

	for (auto& h : m_hpBarHandle)
	{
		DeleteGraph(h);
	}
	m_hpBarHandle.clear();

	SoundManager::GetInstance().StopBGM("inGameBgm");

	DeleteGraph(m_miniMapHandle);
	//MV1DeleteModel(m_crystalHandle);

#ifdef _DEBUG
	m_drawPos.clear();
#endif
}

/// <summary>
/// 初期化
/// </summary>
/// <param name="pScore">スコア計算用変数ポインタ</param>
void GameManager::Init(std::shared_ptr<ScoreInfo> pScore)
{
	m_score = pScore;
	m_playerKillCount = &m_score->KilledcounterByPlayer;
	m_trapKillCount = &m_score->KilledcounterByTrap;

	m_pEnemyManager = std::make_shared<EnemyManager>();
	m_pEnemyManager->Init(m_playerKillCount, m_trapKillCount, &m_score->MaxCombo, m_stageName.c_str());
}

/// <summary>
/// 更新
/// </summary>
/// <param name="input">入力クラスポインタ</param>
/// <param name="pSceneGame"></param>
void GameManager::Update(std::shared_ptr<Input>& input, SceneGame* pSceneGame)
{
	SoundManager::GetInstance().PlayBGM("inGameBgm",true);

	if (m_effectFrame % 120 == 0)
	{
		EffectManager::GetInstance().CreateEffect("WarningArea", MyLib::Vec3());
	}
	m_effectFrame++;

	m_pCamera->SetPlayerPos(m_pPlayer->GetPos());
	m_pCamera->Update(input, MapDataLoad::GetInstance().GetStageCollisionHandle());

	m_pPlayer->SetCameraAngle(m_pCamera->GetDirection());
	m_pPlayer->Update(input);


	if (m_nowPhase == ePhaseName::GameClear)
	{
		if (!m_isClear)
		{
			m_isClear = true;

			m_score->frame = m_allFrame;
			m_score->crystalHp = m_objectHP;
			m_pPlayer->SetGameClear();
			m_pCamera->SetClear();

			SoundManager::GetInstance().StopBGM("inGameBgm");
			SoundManager::GetInstance().PlaySE("clear");
		}

		pSceneGame->ToResultScene(true);
		return;
	}
	else if (m_nowPhase == ePhaseName::Gameover)
	{
		pSceneGame->ToResultScene(false);
		return;
	}

	m_pEnemyManager->Update(m_pPhysics, this, m_pPlayer->GetPos(), m_pCamera->GetDirection(), !m_pPlayer->IsGetPlayerDown());

	m_pTrapManager->Update(input, m_pPlayer->GetNowSlotNum(), m_pPlayer->GetPos(), m_pCamera->GetDirection(), m_pPlayer->IsGetPlayerDown(), &m_trapPoint, m_nowPhase, m_pPhysics);

	for (auto& portion : m_pPortion)
	{
		portion->Update();

		if (!portion->GetIsExist())
		{
			portion->Finalize(m_pPhysics);
		}
	}

	//isExistがfalseのオブジェクトを削除
	auto it = std::remove_if(m_pPortion.begin(), m_pPortion.end(), [](auto& v)
		{
			return v->GetIsExist() == false;
		});
	m_pPortion.erase(it, m_pPortion.end());

	// 物理更新
	m_pPhysics->Update();

	//プレイヤーの情報を取得
	m_playerSlotIdx = m_pPlayer->GetNowSlotNum();	//現在選択しているスロットの番号
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
				m_nowPhase = ePhaseName::Gameover;
			}

			//オブジェクトHPが5以上あれば5消費してプレイヤーを復活させる
			if (m_objectHP > 5)
			{
				m_objectHP -= 5;


				m_pPlayer = nullptr;

				m_pPlayer = std::make_shared<Player>();
				m_pPlayer->Init(m_pPhysics);
				m_pPlayer->SetSlotNumMax(kTrapNum);

				SoundManager::GetInstance().PlaySE("revival");
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
		m_nowPhase = ePhaseName::Gameover;
	}

	//現在のフェーズのカウントを進める
	m_phaseFrame++;

	//最初にOKボタンを押したら最初の準備フェーズを終了する
	if (input->IsTriggered("Y"))
	{
		//本来は最初のフェーズのみプレイヤーの任意のタイミングで次のフェーズに進行させる
		//TODO:フェーズごとに敵を生成して、そのフェーズの敵を全部倒したら次のフェーズに進行する処理を作る
		//		そしたらこの部分の処理を戻す
		//if (m_nowPhaseName != ePhaseName::SecondPrePhase && m_nowPhaseName != ePhaseName::ThirdPrePhase)
#ifdef _DEBUG
#else
		if (m_nowPhase == ePhaseName::FirstPrePhase)
#endif
		{
			ChangeNextPhase();
		}
	}

	//準備フェーズに入ったら10秒後に戦闘フェーズに入るようにする
	if (m_nowPhase == ePhaseName::SecondPrePhase || m_nowPhase == ePhaseName::ThirdPrePhase)
	{
		if (m_phaseFrame >= 600)
		{
			ChangeNextPhase();
		}
	}

	EffectManager::GetInstance().Update();

	auto playerPos = m_pPlayer->GetPos();

	m_miniMapPlayerPos.x = kMiniMapX + kMiniMapWidth / 2 + playerPos.x * 0.45f;
	m_miniMapPlayerPos.y = kMiniMapY + kMiniMapHeight - m_offsetY - playerPos.z * 0.45f;

#ifdef _DEBUG
	if (m_frame % 5 == 0)
	{
		//m_drawPos.push_back(MyLib::Vec3(m_miniMapPlayerPosX, m_miniMapPlayerPosY, 0.0f));
	}
	m_frame++;
#endif
}

/// <summary>
/// 描画
/// </summary>
void GameManager::Draw()
{
	MapDataLoad::GetInstance().Draw();

	m_pTrapManager->PreviewDraw();
	m_pTrapManager->Draw();

	m_pPlayer->Draw();
	m_pEnemyManager->Draw();

	for (auto& portion : m_pPortion)
	{
		portion->Draw();
	}

	EffectManager::GetInstance().Draw();

	DrawCircle(640, 390, m_pPlayer->GetDifAngle(), 0xff0000, false);

	//スロットアイテムのカーソル描画
	int center = 540 + m_playerSlotIdx * 100;
	DrawBox(center + 34, 660 + 34, center - 34, 660 - 34, 0xdd0000, false);

	//スロットアイテムの描画
	for (int i = 0; i < m_slotHandle.size(); i++)
	{
		DrawRotaGraph(540 + i * 100, 660, 1.0f, 0.0f, m_slotHandle[i], true);
	}
	DrawFormatStringToHandle(640-kFontSize-kFontSize/2, 690,0xffffff,m_fontHandle,"400");
	DrawFormatStringToHandle(740 - kFontSize - kFontSize / 2, 690,0xffffff,m_fontHandle,"250");

	//HPバーの計算
	//プレイヤーのHP÷プレイヤーの最大HPで現在のHPの割合を出す
	float widthScale = static_cast<float>(m_playerHp) / static_cast<float>(m_playerMaxHp);
	//HPの割合分の画像のX幅を出す
	int width = static_cast<int>(m_hpBarWidth * widthScale);
	//何もしないと画像が中心によっていくため画像の中心座標をHPに応じてずらす
	int posX = 240 - static_cast<int>((m_hpBarWidth - width) * kHpBarScale / 2);

	//Hpバーの描画
	DrawRotaGraph(220, 30, kHpBarScale, 0.0f, m_hpBarHandle[0], true);
	DrawRotaGraph(236, 30, kHpBarScale, 0.0f, m_hpBarHandle[2], true);
	DrawRectRotaGraph(posX, 30, 0, 0, width, m_hpBarHeight, kHpBarScale, 0.0f, m_hpBarHandle[1], true);
	DrawRotaGraph(236, 25, kHpBarScale, 0.0f, m_hpBarHandle[3], true);

	//プレイヤーの正面にいる敵のHPバーの描画
	if (m_pEnemyManager->GetFrontEnemyHp() != 0)
	{
		//HPバーの計算
		//プレイヤーのHP÷プレイヤーの最大HPで現在のHPの割合を出す
		float widthScale = static_cast<float>(m_pEnemyManager->GetFrontEnemyHp()) / static_cast<float>(m_pEnemyManager->GetFrontEnemyMaxHp());
		//HPの割合分の画像のX幅を出す
		int width = static_cast<int>(m_hpBarWidth * widthScale);
		//何もしないと画像が中心によっていくため画像の中心座標をHPに応じてずらす
		int posX = 620 - static_cast<int>((m_hpBarWidth - width) * kHpBarScale/ 1.4 / 2);

		//Hpバーの描画
		DrawRotaGraph(600, 30, kHpBarScale / 1.4, 0.0f, m_hpBarHandle[0], true);
		DrawRotaGraph(616, 30, kHpBarScale / 1.4, 0.0f, m_hpBarHandle[2], true);
		DrawRectRotaGraph(posX, 30, 0, 0, width, m_hpBarHeight, kHpBarScale/ 1.4, 0.0f, m_hpBarHandle[1], true);
		DrawRotaGraph(616, 27, kHpBarScale / 1.4, 0.0f, m_hpBarHandle[3], true);
	}

	//罠ポイントの描画
	DrawRotaGraph(102, 720 - 28, 0.8f, 0.0f, m_uiHandle, true);
	DrawFormatStringToHandle(40, 670, 0xffffff, m_trapPointHandle, "%d", m_trapPoint);

	//最初の準備フェーズならチュートリアルテキストを表示する
	if (m_nowPhase == 0)
	{
		DrawStringToHandle(640 - 36 *3-12, 16, "準備ができたら", 0xffffff, m_trapPointHandle);
		DrawStringToHandle(640 - 36 *4, 48, "Yボタン", 0xffff00, m_trapPointHandle);
		DrawStringToHandle(640, 48, "を押そう!", 0xffffff, m_trapPointHandle);

	}

	if (m_nowPhase == ePhaseName::SecondPrePhase || m_nowPhase == ePhaseName::ThirdPrePhase)
	{
		DrawFormatStringToHandle(640 - kFontSize *5, 48, 0xffffff, m_fontHandle, "次のウェーブまで%d", (600 - m_phaseFrame)/60);
	}

	//ウェーブ数の表示
	int waveNum = static_cast<int>(1 + m_nowPhase / 2);
	if (waveNum > 3)
	{
		waveNum = 3;
	}
	

	//ミニマップの描画
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 200);
	DrawBox(kMiniMapX, kMiniMapY, kMiniMapX + kMiniMapWidth, kMiniMapY + kMiniMapHeight, 0x000000, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	DrawRotaGraph(kMiniMapX + kMiniMapWidth / 2, kMiniMapY + kMiniMapHeight / 2, 1.0f, 0.0f, m_miniMapHandle, true);

	DrawCircle(kMiniMapX + static_cast<int>(kMiniMapWidth / 2 + m_crystalPos.x * 0.45f), kMiniMapY + kMiniMapHeight + static_cast<int>(m_crystalPos.z * 0.45f), 4, 0xaaaaff, true);
	


	for (auto& enemyPos : m_pEnemyManager->GetEnemyPos())
	{
		int y = kMiniMapY + kMiniMapHeight - m_offsetY - static_cast<int>(enemyPos.z * 0.45f);

		if (y < kMiniMapY)
		{
			continue;
		}

		DrawCircle(kMiniMapX + kMiniMapWidth / 2 + static_cast<int>(enemyPos.x * 0.45f), y, 2, 0xff0000, true);
	}

	DrawFormatStringToHandle(1105, 40, 0xffffff, m_fontHandle, "%d/3", waveNum);

	DrawCircle(static_cast<int>(m_miniMapPlayerPos.x), static_cast<int>(m_miniMapPlayerPos.y), 2, 0x00ff00, true);


	//オブジェクトの残りHPの描画
	DrawRotaGraph(kCrystalHpGraph, 40, 0.3f, 0.0f, m_objectUIBgHandle, true);
	//文字列の桁数が変わると中心がずれるため補正
	int offset = - 2;
	if (m_objectHP < 20)
	{
		offset = ((1 - (m_objectHP / 10)) * 8);
	}
	 
	DrawFormatStringToHandle(kCrystalHpGraph - 18 + offset, 27, 0xffffff, m_fontHandle, "%d", m_objectHP);

#ifdef _DEBUG
	DebugDraw();

	//座標を核にするためだけ
	m_pCamera->DebugDraw();
	DrawFormatString(1000, 0, 0xff0000, "オブジェクトHP:%d", m_objectHP);
	DrawFormatString(1000, 16, 0xff0000, "罠ポイント:%d", m_trapPoint);
	//DrawFormatString(1000, 16, 0xff0000, "プレイヤーダウンカウント:%d", m_playerDownCount);

	//ミニマップを作成する用
	for (auto& pos : m_drawPos)
	{
		DrawBox(static_cast<int>(pos.x) - 2, static_cast<int>(pos.y) - 2, static_cast<int>(pos.x) + 2, static_cast<int>(pos.y) + 2, 0x0000ff, true);
	}

#endif
}

/// <summary>
/// デバッグ描画
/// </summary>
void GameManager::DebugDraw()
{
	if (m_nowPhase == ePhaseName::FirstPrePhase)
	{
		DrawFormatString(560, 10, 0xff0000, "第1準備フェーズ");
	}
	else if (m_nowPhase == ePhaseName::FirstBattlePhase)
	{
		DrawFormatString(560, 10, 0xff0000, "第1戦闘フェーズ");
	}
	else if (m_nowPhase == ePhaseName::SecondPrePhase)
	{
		DrawFormatString(560, 10, 0xff0000, "第2準備フェーズ");
	}
	else if (m_nowPhase == ePhaseName::SecondBattlePhase)
	{
		DrawFormatString(560, 10, 0xff0000, "第2戦闘フェーズ");
	}
	else if (m_nowPhase == ePhaseName::ThirdPrePhase)
	{
		DrawFormatString(560, 10, 0xff0000, "第3準備フェーズ");
	}
	else if (m_nowPhase == ePhaseName::ThirdBattlePhase)
	{
		DrawFormatString(560, 10, 0xff0000, "第3戦闘フェーズ");
	}
	else if (m_nowPhase == ePhaseName::GameClear)
	{
		DrawFormatString(560, 10, 0xff0000, "ゲームクリア");
	}
	else if (m_nowPhase == ePhaseName::Gameover)
	{
		DrawFormatString(560, 10, 0xff0000, "ゲームオーバー");
	}

	DrawFormatString(560, 30, 0xff0000, "現在のフェーズのタイム : %d", m_phaseFrame);
	DrawFormatString(560, 50, 0xff0000, "開始からのタイム : %d", m_allFrame);

}

/// <summary>
/// フェーズを進める
/// </summary>
void GameManager::ChangeNextPhase()
{
	SoundManager::GetInstance().PlaySE("enemyAppear");

	m_nowPhase = static_cast<ePhaseName>(m_nowPhase + 1);
	m_allFrame += m_phaseFrame;
	m_phaseFrame = 0;
}

/// <summary>
/// クリスタルHPを減らす
/// </summary>
/// <returns></returns>
const void GameManager::SubCrystalHP()
{
	m_objectHP--;

	SoundManager::GetInstance().PlaySE("crystal");
}

/// <summary>
/// プレイヤー座標を取得
/// </summary>
/// <returns></returns>
const MyLib::Vec3 GameManager::GetPlayerPos()const
{
	return m_pPlayer->GetPos();
}

/// <summary>
/// 敵をキルしたときに罠ポイントを取得
/// </summary>
/// <param name="point"></param>
void GameManager::AddTrapPoint(int point)
{
	m_trapPoint += point;
}

/// <summary>
/// 回復ポーションを生成
/// </summary>
/// <param name="pos"></param>
void GameManager::CreatePortion(MyLib::Vec3 pos)
{
	if (m_potionNum >= kMaxPortionNum)
	{
		return;
	}

	std::shared_ptr<HealPortion> add = std::make_shared<HealPortion>();
	add->Init(m_pPhysics);
	add->SetPosition(pos);
	m_pPortion.emplace_back(add);

	m_potionNum++;
}