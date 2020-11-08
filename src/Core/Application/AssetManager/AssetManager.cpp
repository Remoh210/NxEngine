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

	AssetLoader::LoadModel(Nx::FileSystem::GetPath(file), models, materialIndices, materials);

	Array<MeshInfo*> loadedMeshes;
	for (int i; i < loadedMeshes.size(); i++)
	{
		IndexedModel& model = models[i];
		MeshInfo* newMesh = new MeshInfo();
		MaterialSpec& materialSpec = materials[materialIndices[i]];
		newMesh->material = new Material();

		newMesh->vertexArray = new VertexArray(*renderDevice, model, BufferUsage::USAGE_DYNAMIC_DRAW);

		//Textures
		for (auto texture_it : materialSpec.textureNames)
		{
			if (texture_it.first == "Diffuse")
			{
				auto imported_texture_it = importedTextures.find(texture_it.second);
				if (imported_texture_it != importedTextures.end())
				{
					newMesh->material->diffuseTextures.push_back(imported_texture_it->second);
				}
				else
				{
					NString textureFile = texture_it.second;
					ArrayBitmap bitmap;
					bitmap.Load(imported_texture_it->first);
					Texture* texture = new Texture(*renderDevice, bitmap, PixelFormat::FORMAT_RGBA, false, false);
					newMesh->material->diffuseTextures.push_back(texture);
					importedTextures[textureFile] = texture;
				}
			}

		}

		loadedMeshes.push_back(newMesh);
	}

	importedModels[file] = loadedMeshes;
}