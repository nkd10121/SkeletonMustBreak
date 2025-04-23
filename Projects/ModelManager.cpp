#include "ModelManager.h"
#include "DxLib.h"
#include <cassert>

ModelManager* ModelManager::m_instance = nullptr;

/// <summary>
/// デストラクタ
/// </summary>
ModelManager::~ModelManager()
{
	//持っているハンドルを全部Deleteする
	for (auto& h : m_handles)
	{
		MV1DeleteModel(h.handle);
	}

	m_handles.clear();
}

/// <summary>
/// モデルハンドルを取得する
/// </summary>
/// <param name="path">モデルパス</param>
/// <param name="isEternal">永久フラグ</param>
/// <returns>モデルハンドル</returns>
int ModelManager::GetModelHandle(std::string path, bool isEternal)
{
	//すでにロードされていたら複製ハンドルを返す
	for (auto& h : m_handles)	
	{
		if (h.path == path)
		{
			return MV1DuplicateModel(h.handle);
		}
	}

	//ここに来たということはすでにロードされていなかった
	//→新しくロードする必要がある
	Handle add;
	add.handle = MV1LoadModel(path.c_str());
	add.path = path;
	add.isEternal = isEternal;

	m_handles.emplace_back(add);

	//ロードしたハンドルの複製ハンドルを返す
	return MV1DuplicateModel(add.handle);
}

/// <summary>
/// 永久フラグがfalseのハンドルを削除する
/// </summary>
void ModelManager::Clear()
{
	//isEternalがfalseのハンドルをDeleteする
	for (auto& h : m_handles)
	{
		if (!h.isEternal)
		{
			MV1DeleteModel(h.handle);
			h.handle = 0;
		}
	}

	//不要になったハンドルをここで削除処理する
	auto it = remove_if(m_handles.begin(), m_handles.end(), [](auto& v) {
		return v.handle == 0;
		});
	m_handles.erase(it, m_handles.end());
}
