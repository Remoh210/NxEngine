#pragma once

#include "IndexedModel.h"
#include "Material.h"

class AssetLoader
{
public:

	static bool LoadModels(const String& fileName,
			Array<IndexedModel>& models, Array<uint32>& modelMaterialIndices,
			Array<Material>& materials);

};
