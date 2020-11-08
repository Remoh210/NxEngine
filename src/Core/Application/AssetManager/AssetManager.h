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

    static Map<NString, Array<MeshInfo*>> importedModels;
    static Map<NString, Texture*> importedTextures;
};