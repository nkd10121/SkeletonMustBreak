#pragma once
#include "SceneBase.h"

class UICursor;

/// <summary>
/// オプションシーン
/// </summary>
class SceneOption : public SceneBase
{
public:
	//コンストラクタ
	SceneOption(SceneManager& mgr);
	//デストラクタ
	virtual ~SceneOption();

	//更新
	void Update(std::shared_ptr<Input>& input);
	//描画
	void Draw();

private:
	//オプション項目名列挙型
	enum eItem : int
	{
		BGM,
		SE,
		Sensitivity,
		FullScreen
	};

private:
	//フォントのハンドル(必要ないかも)
	int m_fontHandle;

	//カーソルクラスのポインタ
	std::shared_ptr<UICursor> m_pCursor;

	//現在選択している項目
	eItem m_nowItem;

	//bgmの大きさ
	int m_bgmScale;
	//seの大きさ
	int m_seScale;
	//感度の大きさ
	int m_sensitivityScale;
	//フルスクリーンかどうか
	bool m_isFullScreen;

	//ボタンを押しているカウント(キーリピート用)
	int m_pushCount;

	//更新メンバ関数ポインタ
	void (SceneOption::* m_updateFunc)(std::shared_ptr<Input> input);
	//更新(BGM設定)
	void BgmUpdate(std::shared_ptr<Input> input);
	//更新(SE設定)
	void SeUpdate(std::shared_ptr<Input> input);
	//更新(感度設定)
	void SensitivityUpdate(std::shared_ptr<Input> input);
	//更新(フルスクリーン設定)
	void FullScreenUpdate(std::shared_ptr<Input> input);
};

