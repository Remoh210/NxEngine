#pragma once

#include "IndexedModel.h"
#include <assimp/scene.h>
#include "Material.h"

class AssetLoader
{
public:

	static void SetShouldFlipVTexture(bool bValue);


	static bool LoadModels(const String& fileName,
			Array<IndexedModel>& models, Array<uint32>& modelMaterialIndices,
			Array<Material>& materials);

	static unsigned int TextureFromFile(String path);

	static void LoadModel(const String& fileName,
		Array<IndexedModel>& models, Array<uint32>& modelMaterialIndices,
		Array<Material>& materials);

private:

	static void ProcessNode(aiNode *node, const aiScene *scene, const String& fileName,
		Array<IndexedModel>& models, Array<uint32>& modelMaterialIndices,
		Array<Material>& materials);

	static void ProcessMesh(aiMesh* mesh, const aiScene* scene, const String& fileName,
		Array<IndexedModel>& models, Array<uint32>& modelMaterialIndices,
		Array<Material>& materials);

	static void LoadMaterialTextures(const String& filePath, aiMaterial *mat, aiTextureType type, Material& material, std::string typeName);

	static String mModelDirectory;
	static Array<String*> mLoadedTextures;
};
