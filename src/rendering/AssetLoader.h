#pragma once

#include "IndexedModel.h"
#include <assimp/scene.h>
#include "Material.h"

class AssetLoader
{
public:

	static void SetShouldFlipVTexture(bool bValue);

	static unsigned int TextureFromFile(NString path);
	static unsigned int TextureFromAssimp(const aiTexture* texture);

	static bool LoadModel(const NString& fileName,
		Array<IndexedModel>& models, Array<uint32>& modelMaterialIndices,
		Array<MaterialSpec>& materials);

private:

	static void ProcessNode(aiNode *node, const aiScene *scene, const NString& fileName,
		Array<IndexedModel>& models, Array<uint32>& modelMaterialIndices,
		Array<MaterialSpec>& materials);

	static void ProcessMesh(aiMesh* mesh, const aiScene* scene, const NString& fileName,
		Array<IndexedModel>& models, Array<uint32>& modelMaterialIndices,
		Array<MaterialSpec>& materials);

	static void LoadMaterialTextures(const NString& filePath, aiMaterial* mat, const aiScene* scene, aiTextureType type, MaterialSpec& material, NString typeName);

	static NString GetFileExtension(NString fileName)
	{
		std::size_t found = fileName.find_last_of('.');
		return fileName.substr(found);
	}

	static NString mModelDirectory;
	static Array<NString*> mLoadedTextures;
};
