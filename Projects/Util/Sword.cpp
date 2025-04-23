#include "Sword.h"
#include "ModelManager.h"

Sword::Sword()
{
	m_modelHandle = ModelManager::GetInstance().GetModelHandle("data/model/SkeletonBlade.mv1");
}

Sword::~Sword()
{
	MV1DeleteModel(m_modelHandle);
}
