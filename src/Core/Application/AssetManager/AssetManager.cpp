#include "AssetManager.h"
#include "rendering/AssetLoader.h"
#include "Core/FileSystem/FileSystem.h"

Map<NString, Array<MeshInfo*>> AssetManager::importedModels;
Map<NString, Texture*> AssetManager::importedTextures;

Array<MeshInfo*> AssetManager::ImportModel(RenderDevice* renderDevice, NString file)
{
	auto iterator = importedModels.find(file);
	if (iterator != importedModels.end())
	{
		return iterator->second;
	}

	Array<IndexedModel> models;
	Array<MaterialSpec> materials;
	Array<uint32> materialIndices;

	AssetLoader::LoadModel(file, models, materialIndices, materials);

	Array<MeshInfo*> loadedMeshes;
	for (int i = 0; i < models.size(); i++)
	{
		IndexedModel& model = models[i];
		MeshInfo* newMesh = new MeshInfo();

		newMesh->vertexArray = new VertexArray(renderDevice, model, BufferUsage::USAGE_DYNAMIC_DRAW);

		//HACK!
		if (materials.size() == 0)
		{
			//Use default material
			newMesh->material = new Material();
			continue;
		}
		
		MaterialSpec& materialSpec = materials[materialIndices[i] - 1];
		newMesh->material = new Material();
		//Textures
		for (auto texture_it : materialSpec.textureNames)
		{
			NString textureType = texture_it.first;
			if (textureType == "texture_diffuse")
			{
				if (Texture* cachedTex = FindTexture(texture_it.first))
				{
					newMesh->material->textures[TEXTURE_ALBEDO] = cachedTex;
				}
				else
				{
					NString textureFile = texture_it.second;
					ArrayBitmap bitmap;
					bitmap.Load(textureFile);
					Texture* texture = new Texture(renderDevice, bitmap, PixelFormat::FORMAT_RGBA, false, false);
					newMesh->material->textures[TEXTURE_ALBEDO] = texture;
					importedTextures[textureFile] = texture;
				}
			}
			else if (textureType == "texture_normal")
			{
				//if (Texture* cachedTex = FindTexture(texture_it.first))
				//{
				//	newMesh->material->diffuseTexture = cachedTex;
				//}
				//else
				//{
				//	NString textureFile = texture_it.second;
				//	ArrayBitmap bitmap;
				//	bitmap.Load(textureFile);
				//	Texture* texture = new Texture(renderDevice, bitmap, PixelFormat::FORMAT_RGBA, false, false);
				//	newMesh->material->normalMap = texture;
				//	importedTextures[textureFile] = texture;
				//}
			}

		}

		loadedMeshes.push_back(newMesh);
	}

	importedModels[file] = loadedMeshes;

	return loadedMeshes;
}