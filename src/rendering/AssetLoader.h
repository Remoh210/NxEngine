#pragma once


#include "IndexedModel.h"
#include "Material.h"

//TODO: HAX?
#include "Core/Graphics/SkinnedMesh/SkinnedMeshInfo.h"
#include "Core/Graphics/Animation/AnimationInfo.h"


#include <assimp/scene.h>
#include <array>

const int MAX_BONES_PER_VERTEX = 4;

struct VertexBoneData
{
	std::array<float, MAX_BONES_PER_VERTEX> ids;
	std::array<float, MAX_BONES_PER_VERTEX> weights;

	inline void AddBoneData(unsigned int BoneID, float Weight)
	{
		for (unsigned int Index = 0; Index < MAX_BONES_PER_VERTEX; Index++) {
			if (this->weights[Index] == 0.0f) {
				ids[Index] = (float)BoneID;
				weights[Index] = Weight;
				return;
			}
		}

	}
};

class AssetLoader
{
public:

	static void SetShouldFlipVTexture(bool bValue);

	static unsigned int TextureFromFile(NString path);

	static unsigned int TextureFromAssimp(const aiTexture* texture);

	static bool LoadModel(const NString& fileName,
		NxArray<IndexedModel>& models, NxArray<uint32>& modelMaterialIndices,
		NxArray<MaterialSpec>& materials);

	static const aiScene* LoadModelSkeletal(const NString& fileName,
		NxArray<IndexedModel>& models, SkeletalData& skelData, NxArray<uint32>& modelMatIndices,
		NxArray<MaterialSpec>& matArray);

	static AnimationInfo* LoadMeshAnimation(const std::string &friendlyName,
		const std::string &filename, bool hasExitTime);

	//static LoadAnimationScene(AnimationInfo* animationInfoIn);

private:

	static void ProcessNode(aiNode *node, const aiScene *scene, const NString& fileName,
		NxArray<IndexedModel>& models, NxArray<uint32>& modelMaterialIndices,
		NxArray<MaterialSpec>& materials);

	static void ProcessMesh(aiMesh* mesh, const aiScene* scene, const NString& fileName,
		NxArray<IndexedModel>& models, NxArray<uint32>& modelMaterialIndices,
		NxArray<MaterialSpec>& materials);

	static void ProcessMeshSkeletal(aiMesh * mesh, const aiScene * scene, const NString & fileName, 
		NxArray<IndexedModel>& models, NxArray<uint32>& modelMaterialIndices, 
		NxArray<MaterialSpec>& materials, const NxArray<VertexBoneData>& boneData);

	static void LoadMaterialTextures(const NString& filePath, aiMaterial* mat, const aiScene* scene, aiTextureType type, MaterialSpec& material, NString typeName);

	static void LoadMRTextures(const NString & filePath, aiMaterial * mat, const aiScene * scene, MaterialSpec & spec);

	static void LoadBones(const aiMesh* Mesh, SkeletalData& skelData, NxArray<VertexBoneData>& boneData);

	static NString GetFileExtension(NString fileName)
	{
		std::size_t found = fileName.find_last_of('.');
		return fileName.substr(found);
	}

	static NString mModelDirectory;
	static NxArray<NString*> mLoadedTextures;
};
