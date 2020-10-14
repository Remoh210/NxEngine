#pragma once

#include "IndexedModel.h"
#include <assimp/scene.h>
#include "Material.h"

class AssetLoader
{
public:

	static bool LoadModels(const String& fileName,
			Array<IndexedModel>& models, Array<uint32>& modelMaterialIndices,
			Array<Material>& materials);

	static unsigned int TextureFromFile(String path);

	static void AssetLoader::LoadModel(const String& fileName,
		Array<IndexedModel>& models, Array<uint32>& modelMaterialIndices,
		Array<Material>& materials);

private:

	static void AssetLoader::ProcessNode(aiNode *node, const aiScene *scene, const String& fileName,
		Array<IndexedModel>& models, Array<uint32>& modelMaterialIndices,
		Array<Material>& materials);

	static void AssetLoader::ProcessMesh(aiMesh* mesh, const aiScene* scene, const String& fileName,
		Array<IndexedModel>& models, Array<uint32>& modelMaterialIndices,
		Array<Material>& materials);

	static void AssetLoader::LoadMaterialTextures(const String& filePath, aiMaterial *mat, aiTextureType type, Material& material, std::string typeName);

	static String mModelDirectory;
	static Array<String*> mLoadedTextures;
};
