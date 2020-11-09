#include "Common/CommonTypes.h"
#include "Common/dataStructs/Array.h"
#include "Common/dataStructs/Map.h"
#include "Common/dataStructs/String.h"
#include "Core/Graphics/Mesh/MeshInfo.h"

//Material specs
#include "rendering/IndexedModel.h"
#include "rendering/Material.h"

class AssetManager 
{
public:
    AssetManager() {};

	static Array<MeshInfo*> ImportModel(RenderDevice* renderDevice, NString file);



private:
	static Map<NString, Array<MeshInfo*>> importedModels;
	static Map<NString, Texture*> importedTextures;

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