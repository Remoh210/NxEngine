#pragma once

#include "IndexedModel.h"
#include <assimp/scene.h>
#include "Material.h"

class AssetLoader
{
public:

	static void SetShouldFlipVTexture(bool bValue);


	static bool LoadModels(const NString& fileName,
			Array<IndexedModel>& models, Array<uint32>& modelMaterialIndices,
			Array<Material>& materials);

	static unsigned int TextureFromFile(NString path);

	static void LoadModel(const NString& fileName,
		Array<IndexedModel>& models, Array<uint32>& modelMaterialIndices,
		Array<Material>& materials);

private:

	static void ProcessNode(aiNode *node, const aiScene *scene, const NString& fileName,
		Array<IndexedModel>& models, Array<uint32>& modelMaterialIndices,
		Array<Material>& materials);

	static void ProcessMesh(aiMesh* mesh, const aiScene* scene, const NString& fileName,
		Array<IndexedModel>& models, Array<uint32>& modelMaterialIndices,
		Array<Material>& materials);

	static void LoadMaterialTextures(const NString& filePath, aiMaterial *mat, aiTextureType type, Material& material, NString typeName);

	static NString mModelDirectory;
	static Array<NString*> mLoadedTextures;
};
