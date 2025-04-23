#include "EffectManager.h"
#include "EffekseerForDXLib.h"

#include <cassert>

EffectManager* EffectManager::m_instance = nullptr;

EffectManager::~EffectManager()
{
	//まずすでに同じパスのエフェクトがロードされていないか確認する
	for (auto& effect : m_effect)
	{
		DeleteEffekseerEffect(effect.second->emitterHandle);
	}
}

/// <summary>
/// エフェクトをロード
/// </summary>
/// <param name="name">エフェクト名</param>
/// <param name="path">パス</param>
/// <param name="endFrame">終了フレーム数</param>
/// <param name="scale">拡大率</param>
void EffectManager::Load(std::string name, const char* path, int endFrame, float scale)
{
	//まずすでに同じパスのエフェクトがロードされていないか確認する
	for (auto& effect : m_effect)
	{
		if (effect.second->effectPath == path)
		{
			return;
		}
	}

	//ここまで来たらエフェクトをロードする
	std::shared_ptr<EffectEmitter> add = std::make_shared<EffectEmitter>();
	add->emitterHandle = LoadEffekseerEffect(path, scale);
	assert(add->emitterHandle != -1 && "エフェクトロード失敗");
	add->endFrame = endFrame;

	m_effect[name] = add;

}

/// <summary>
/// 更新
/// </summary>
void EffectManager::Update()
{
	UpdateEffekseer3D();

	//エフェクトの更新
	for (auto& effects : m_effect)
	{
		for (auto& ef : effects.second->effects)
		{
			if (ef.frame > effects.second->endFrame)
			{
				StopEffekseer3DEffect(ef.handle);
				ef.isExist = false;
			}

			ef.frame++;
		}

		//isExistがfalseのオブジェクトを削除
		auto it = std::remove_if(effects.second->effects.begin(), effects.second->effects.end(), [](auto& v)
			{
				return v.isExist == false;
			});
		effects.second->effects.erase(it, effects.second->effects.end());
	}
}

/// <summary>
/// 描画
/// </summary>
void EffectManager::Draw()
{
	DrawEffekseer3D();
}

/// <summary>
/// エフェクトを生成
/// </summary>
/// <param name="name">エフェクト名</param>
/// <param name="pos">描画座標</param>
/// <param name="rot">回転角度</param>
void EffectManager::CreateEffect(std::string name, MyLib::Vec3 pos, MyLib::Vec3 rot)
{
	int handle = -1;
	for (auto& effects : m_effect)
	{
		if (effects.first == name)
		{
			handle = effects.second->emitterHandle;
		}
	}

	//もし追加しようとしているエフェクトの名前のハンドルがなければ終わる
	if (handle == -1)
	{
		return;
	}

	Effect add;
	add.handle = PlayEffekseer3DEffect(handle);
	add.frame = 0;
	add.x = pos.x;
	add.y = pos.y;
	add.z = pos.z;
	add.isExist = true;

	SetPosPlayingEffekseer3DEffect(add.handle, add.x, add.y, add.z);
	SetRotationPlayingEffekseer3DEffect(add.handle, rot.x, rot.y, rot.z);

	m_effect[name]->effects.emplace_back(add);

	return;
}

/// <summary>
/// 指定した名前のすべてのエフェクトの座標を移動させる
/// </summary>
/// <param name="name">移動させたいエフェクトの名前</param>
/// <param name="pos">移動先座標</param>
void EffectManager::SetPos(std::string name, MyLib::Vec3 pos)
{
	std::shared_ptr<EffectEmitter> emit;

	for (auto& effects : m_effect)
	{
		if (effects.first == name)
		{
			for (auto& ef : effects.second->effects)
			{
				ef.x = pos.x;
				ef.y = pos.y;
				ef.z = pos.z;

				SetPosPlayingEffekseer3DEffect(ef.handle, ef.x, ef.y, ef.z);

			}
		}
	}

	return;
}

/// <summary>
/// 指定した名前のすべてのエフェクトを回転させる
/// </summary>
/// <param name="name">エフェクト名</param>
/// <param name="rot">回転角度</param>
void EffectManager::SetRotation(std::string name, MyLib::Vec3 rot)
{
	for (auto& effects : m_effect)
	{
		if (effects.first == name)
		{
			for (auto& ef : effects.second->effects)
			{
				SetRotationPlayingEffekseer3DEffect(ef.handle,rot.x,rot.y,rot.z);
			}
		}
	}
}
