#include "Common/CommonTypes.h"
#include "Common/DataStructs/Array.h"
#include "Common/DataStructs/Map.h"
#include "Common/DataStructs/String.h"
#include "Core/Graphics/Mesh/MeshInfo.h"

//Material specs
#include "rendering/IndexedModel.h"
#include "rendering/Material.h"

class AssetManager 
{
    AssetManager();

	Array<MeshInfo*> ImportModel(RenderDevice* renderDevice, NString file);

    static Map<NString, Array<MeshInfo*>> importedModels;
    static Map<NString, Texture*> importedTextures;
};