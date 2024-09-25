#pragma once
#include "SceneBase.h"
class SceneOption : public SceneBase
{
public:
	SceneOption(SceneManager& mgr);
	virtual ~SceneOption();

	/// <summary>
	/// 更新処理
	/// </summary>
	/// <param name="input"></param>
	void Update(std::shared_ptr<Input>& input);

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();

private:
	enum Item : int
	{
		BGM,
		SE,
		Sensitivity,
		FullScreen
	};

private:
	int m_fontHandle;
	int m_bgHandle;

	std::vector<int> m_handle;
	float m_angle;
	int m_cursorOffsetX;

	Item m_nowItem;

	int m_bgmScale;
	int m_seScale;
	int m_sensitivityScale;
	bool m_isFullScreen;

	int m_pushCount;

	//更新メンバ関数ポインタ
	void (SceneOption::* m_updateFunc)(std::shared_ptr<Input> input);
	//BGM
	void BgmUpdate(std::shared_ptr<Input> input);
	//BGM
	void SeUpdate(std::shared_ptr<Input> input);
	//BGM
	void SensitivityUpdate(std::shared_ptr<Input> input);
	//BGM
	void FullScreenUpdate(std::shared_ptr<Input> input);
};

