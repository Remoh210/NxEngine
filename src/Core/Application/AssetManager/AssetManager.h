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

    inline Array<MeshInfo*> ImportModel(NString file)
    {
        auto iterator = importedModels.find(file);
        if(iterator != importedModels.end())
        {
            return iterator->second();
        }

        Array<IndexedModel> models;
        Array<MaterialSpec> materials;
        Array<uint32> materialIndices;
        //AssetLoader::Load()

        Array<MeshInfo> loadedMeshes;
        for(int i; i < loadedMeshes.size(); i++)
        {
            IndexedModel& model = models[i];
            MeshInfo newMesh;
            newMesh.vertexArray = VertexArray(device, model, BufferUsage::USAGE_DYNAMIC_DRAW);
            MaterialSpec& materialSpec = materials[materialIndices[i]];
            //Textures
            for(auto texture_it : materialSpec.textureNames)
            {
                if(texture_it->first == "Diffuse")
                {
                    auto imported_texture_it = importedTextures.find(texture->second);
                    if(imported_texture_it != importedTextures.end())
                    {
                       newMesh.material->diffuseTextures.push_back(imported_texture_it->second());
                    }
                    else 
                    {
                        ArrayBitmap bitmap;
	                    testBitmap2.Load(imported_texture_it->second());
	                    newMesh.material->diffuseTextures.push_back(new Texture(renderDevice, bitmap, PixelFormat::FORMAT_RGBA, false, false));
                    }
                }

            }

            //newMesh.material = 
        }


        importedModels["File"] = loadedMeshes;

        return loadedMeshes;
    }

    static Map<NString, Array<MeshInfo*>> importedModels;
    static Map<NString, Texture*> importedTextures;
    static Map<NString, Shader*> shaders;
};