#include "AssetManager.h"
#include "rendering/AssetLoader.h"
#include "Core/FileSystem/FileSystem.h"

Map<NString, NxArray<MeshInfo*>> AssetManager::importedModels;
Map<NString, Texture*> AssetManager::importedTextures;

NxArray<MeshInfo*> AssetManager::ImportModel(RenderDevice* renderDevice, NString file)
{
	auto iterator = importedModels.find(file);
	if (iterator != importedModels.end())
	{
		return iterator->second;
	}

	NxArray<IndexedModel> models;
	NxArray<MaterialSpec> materials;
	NxArray<uint32> materialIndices;

	AssetLoader::LoadModel(file, models, materialIndices, materials);
	NxArray<MeshInfo*> loadedMeshes;

	for (int i = 0; i < models.size(); i++)
	{
		MeshInfo* curMesh = new MeshInfo();
		curMesh->vertexArray = new VertexArray(renderDevice, models[i], USAGE_STATIC_DRAW);
		loadedMeshes.push_back(curMesh);

		Material* curMaterial = new Material();

		//HACK!
		if (materials.size() == 0)
		{
			//Use default material
			curMesh->material = new Material();
			continue;
		}

		for (int j = 0; j < materials.size(); j++)
		{
			for (auto textureTypeToFile : materials[i].textureNames)
			{
				//TODO: Add check for empty string?
				NString textureType = textureTypeToFile.first;
				NString textureFileName = textureTypeToFile.second;

				Texture* texture = importedTextures.Find(textureFileName);
				if (texture != nullptr)
				{
					curMaterial->textures[textureType] = texture;
				}
				else
				{
					ArrayBitmap bitmap;
					bitmap.Load(textureTypeToFile.second);
					texture = new Texture(renderDevice, bitmap, PixelFormat::FORMAT_RGB, true, false);
					curMaterial->textures[textureTypeToFile.first] = texture;

					importedTextures[textureFileName] = texture;
				}

			}
			//TODO: HACK for embedded texture
			for (auto textureTypeToTex : materials[i].textures)
			{
				curMaterial->textures[textureTypeToTex.first] = textureTypeToTex.second;
			}
		}
		curMesh->material = curMaterial;

		loadedMeshes.push_back(curMesh);
	}

	importedModels[file] = loadedMeshes;

	return loadedMeshes;
}