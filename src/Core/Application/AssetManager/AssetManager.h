#include "Common/CommonTypes.h"
#include "Common/dataStructs/NxArray.h"
#include "Common/dataStructs/Map.h"
#include "Common/dataStructs/String.h"
#include "Core/Graphics/Mesh/MeshInfo.h"
#include "Core/Graphics/SkinnedMesh/SkinnedMeshInfo.h"
#include "Core/Graphics/Animation/AnimationInfo.h"

//Material specs
#include "rendering/IndexedModel.h"
#include "rendering/Material.h"

class AssetManager 
{
public:
    AssetManager() {};

	static NxArray<MeshInfo*> ImportModel(RenderDevice* renderDevice, NString file);
	static SkinnedMeshInfo* ImportModelSkeletal(RenderDevice* renderDevice, NString file);
	static AnimationInfo* ImportAnimation(NString file, NString name);


private:

	static Material* LoadMaterial(RenderDevice* renderDevic, MaterialSpec& matSpec);

	static NxMap<NString, NxArray<MeshInfo*>> importedModels;
	static NxMap<NString, SkinnedMeshInfo*> importedSkinnedModels;
	static NxMap<NString, Texture*> importedTextures;
	static NxMap<NString, AnimationInfo*> importedAnimations;

	inline static Texture* FindTexture(NString textureFile)
	{
		auto imported_texture_it = importedTextures.find(textureFile);
		if (imported_texture_it != importedTextures.end())
		{
			return imported_texture_it->second;
		}
		else
		{
			return nullptr;
		}
	}
};