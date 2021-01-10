#include "AssetManager.h"
#include "rendering/AssetLoader.h"
#include "Core/FileSystem/FileSystem.h"

NxMap<NString, NxArray<MeshInfo*>> AssetManager::importedModels;
NxMap<NString, Texture*> AssetManager::importedTextures;
NxMap<NString, SkinnedMeshInfo*> AssetManager::importedSkinnedModels;

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

		curMesh->material = LoadMaterial(renderDevice, materials[i]);

		loadedMeshes.push_back(curMesh);
	}

	importedModels[file] = loadedMeshes;

	return loadedMeshes;
}

SkinnedMeshInfo* AssetManager::ImportModelSkeletal(RenderDevice* renderDevice, NString file)
{
	auto iterator = importedSkinnedModels.find(file);
	if (iterator != importedSkinnedModels.end())
	{
		return iterator->second;
	}

	NxArray<IndexedModel> models;
	NxArray<MaterialSpec> materials;
	NxArray<uint32> materialIndices;
	SkeletalData skeletalData;


	const aiScene* scene = AssetLoader::LoadModelSkeletal(file, models, skeletalData, materialIndices, materials);

	if (scene != nullptr)
	{
		SkinnedMeshInfo* skinnedMeshInfo = new SkinnedMeshInfo();
		skinnedMeshInfo->AiScene = scene;
		//TODO: Hack, only load first mesh in scene
		MeshInfo* curMesh = new MeshInfo();
		curMesh->vertexArray = new VertexArray(renderDevice, models[0], USAGE_STATIC_DRAW);
		skinnedMeshInfo->mesh = curMesh;
		skinnedMeshInfo->skeletalData = skeletalData;
	}
}

Material * AssetManager::LoadMaterial(RenderDevice* renderDevice, MaterialSpec& matSpec)
{
	Material* curMaterial = new Material();

	for (auto textureTypeToFile : matSpec.textureNames)
	{
		//TODO: Add check for empty string?
		NString textureType = textureTypeToFile.first;
		NString textureFileName = textureTypeToFile.second;

		Texture* texture = MapFuncs::Find(importedTextures, textureFileName);
		//Texture* texture = importedTextures.Find(textureFileName);
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
	for (auto textureTypeToTex : matSpec.textures)
	{
		curMaterial->textures[textureTypeToTex.first] = textureTypeToTex.second;
	}

	return curMaterial;
}
