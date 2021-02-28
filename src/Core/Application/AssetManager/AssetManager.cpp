#include "AssetManager.h"
#include "rendering/AssetLoader.h"
#include "Core/FileSystem/FileSystem.h"

#include "Core/Physcis/iShape.h"
#include "Core/Physcis/sRigidBodyDef.h"
#include "Core/Physcis/GL_Triangle.h"

RenderDevice* AssetManager::renderDevice = nullptr;
ECS::PhysicsSystem* AssetManager::physicsSystem = nullptr;
NxMap<NString, NxArray<MeshInfo*>> AssetManager::importedModels;
NxMap<NString, Texture*>           AssetManager::importedTextures;
NxMap<NString, SkinnedMeshInfo*>   AssetManager::importedSkinnedModels;
NxMap<NString, AnimationInfo*>     AssetManager::importedAnimations;

NxArray<MeshInfo*> AssetManager::ImportModel(NString file)
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

		curMesh->material = LoadMaterial(materials[i]);

		loadedMeshes.push_back(curMesh);
	}

	importedModels[file] = loadedMeshes;

	return loadedMeshes;
}

bool AssetManager::ImportModelGenerateCollider(NString file, NxArray<MeshInfo*>& meshArrayOut, nPhysics::eShapeType shapeType, nPhysics::iShape*& shapeOut)
{
	if (MapFuncs::Contains(importedModels, file))
	{
		return false;
	}

	auto iterator = importedModels.find(file);
	if (iterator != importedModels.end())
	{
		return false;
	}

	NxArray<IndexedModel> models;
	NxArray<MaterialSpec> materials;
	NxArray<uint32> materialIndices;

	if (!AssetLoader::LoadModel(file, models, materialIndices, materials))
	{
		return false;
	}

	NxArray<MeshInfo*> loadedMeshes;
	for (int i = 0; i < models.size(); i++)
	{
		MeshInfo* curMesh = new MeshInfo();
		curMesh->vertexArray = new VertexArray(renderDevice, models[i], USAGE_STATIC_DRAW);

		curMesh->material = LoadMaterial(materials[i]);

		loadedMeshes.push_back(curMesh);
	}

	importedModels[file] = loadedMeshes;

	//Generate Mesh collider
	if (shapeType == nPhysics::SHAPE_TYPE_MESH)
	{
		shapeOut = GenerateCollisionShapeFromModels(models);
	}

	if (shapeType == nPhysics::SHAPE_TYPE_CONVEX_HULL)
	{
		shapeOut = GenerateCollisionConvexHullFromModels(models);
	}

	if (shapeType == nPhysics::SHAPE_TYPE_BOX)
	{
		shapeOut = physicsSystem->GetFactory()->CreateBoxShape(vec3(models[0].aabb.max[0], models[0].aabb.max[1], models[0].aabb.max[2]));
	}

	meshArrayOut = loadedMeshes;

	return true;
}

SkinnedMeshInfo* AssetManager::ImportModelSkeletal(NString file)
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
		skinnedMeshInfo->mesh->material = LoadMaterial(materials[0]);
		return skinnedMeshInfo;
	}
	else
	{
		DEBUG_LOG("Asset Loader", "Warning", "Error loading skeletal mesh");
		return nullptr;
	}

	
}

AnimationInfo* AssetManager::ImportAnimation(NString file, NString name)
{
	auto iterator = importedAnimations.find(file);
	if (iterator != importedAnimations.end())
	{
		return iterator->second;
	}

	return AssetLoader::LoadMeshAnimation(name, file, false);
}

Texture* AssetManager::ImportTexture(RenderDevice* renderDevice, NString textureFileName, PixelFormat format)
{
	Texture* texture = MapFuncs::Find(importedTextures, textureFileName);
	if (texture != nullptr)
	{
		return texture;
	}

	ArrayBitmap bitmap;
	bitmap.Load(textureFileName);
	texture = new Texture(renderDevice, bitmap, format, true, false);

	importedTextures[textureFileName] = texture;

	return texture;
}

nPhysics::iShape* AssetManager::GenerateCollisionShapeFromModels(NxArray<IndexedModel>& indexedModels)
{
	NxArray<nPhysics::GL_Triangle> GLTriangleCombinedArray;
	for (IndexedModel& model : indexedModels)
	{
		std::vector<NxTriangle> triangles = model.triangles;
		std::vector<NxVertex> vertecies = model.vertices;

		NxArray<nPhysics::GL_Triangle> GLTriangle;
		GLTriangle.resize(triangles.size());

		for (int i = 0; i < triangles.size(); i++)
		{

			GLTriangle[i].vertex1[0] = vertecies[triangles[i].vertex_ID_0].x;
			GLTriangle[i].vertex1[1] = vertecies[triangles[i].vertex_ID_0].y;
			GLTriangle[i].vertex1[2] = vertecies[triangles[i].vertex_ID_0].z;

			GLTriangle[i].vertex2[0] = vertecies[triangles[i].vertex_ID_1].x;
			GLTriangle[i].vertex2[1] = vertecies[triangles[i].vertex_ID_1].y;
			GLTriangle[i].vertex2[2] = vertecies[triangles[i].vertex_ID_1].z;

			GLTriangle[i].vertex3[0] = vertecies[triangles[i].vertex_ID_2].x;
			GLTriangle[i].vertex3[1] = vertecies[triangles[i].vertex_ID_2].y;
			GLTriangle[i].vertex3[2] = vertecies[triangles[i].vertex_ID_2].z;

		}


		//for (int i = 0; i < indices.size() / 3; i += 3)
		//{
		//	GLTriangle[i].vertex1[0] = vertecies[indices[i]];
		//	GLTriangle[i].vertex1[1] = vertecies[indices[i] + 1];
		//	GLTriangle[i].vertex1[2] = vertecies[indices[i] + 2];

		//	GLTriangle[i].vertex2[0] = vertecies[indices[i + 1]];
		//	GLTriangle[i].vertex2[1] = vertecies[indices[i + 1] + 1];
		//	GLTriangle[i].vertex2[2] = vertecies[indices[i + 1] + 2];

		//	GLTriangle[i].vertex2[0] = vertecies[indices[i + 2]];
		//	GLTriangle[i].vertex2[1] = vertecies[indices[i + 2] + 1];
		//	GLTriangle[i].vertex2[2] = vertecies[indices[i + 2] + 2];

		//}

		GLTriangleCombinedArray.insert(GLTriangleCombinedArray.end(), GLTriangle.begin(), GLTriangle.end());
	}



	return physicsSystem->GetFactory()->CreateMeshCollider(&GLTriangleCombinedArray[0], GLTriangleCombinedArray.size());
}

nPhysics::iShape* AssetManager::GenerateCollisionShapeFromModel(IndexedModel& indexedModel)
{
	std::vector<uint32> indices = indexedModel.GetIndices();
	std::vector<float> vertecies = indexedModel.GetElement(0);

	NxArray<nPhysics::GL_Triangle> GLTriangle;
	GLTriangle.resize(indices.size()/3);

	for (int i = 0; i < indices.size() / 3; i += 3)
	{
		GLTriangle[i].vertex1[0] = vertecies[indices[i]];
		GLTriangle[i].vertex1[1] = vertecies[indices[i] + 1];
		GLTriangle[i].vertex1[2] = vertecies[indices[i] + 2];

		GLTriangle[i].vertex2[0] = vertecies[indices[i + 1]];
		GLTriangle[i].vertex2[1] = vertecies[indices[i + 1] + 1];
		GLTriangle[i].vertex2[2] = vertecies[indices[i + 1] + 2];

		GLTriangle[i].vertex2[0] = vertecies[indices[i + 2]];
		GLTriangle[i].vertex2[1] = vertecies[indices[i + 2] + 1];
		GLTriangle[i].vertex2[2] = vertecies[indices[i + 2] + 2];
	}

	return physicsSystem->GetFactory()->CreateMeshCollider(&GLTriangle[0], GLTriangle.size());
}

nPhysics::iShape* AssetManager::GenerateCollisionConvexHullFromModels(NxArray<IndexedModel>& indexedModels)
{
	NxArray<float> verteciesCombined;
	for (IndexedModel& model : indexedModels)
	{
		NxArray<float>vertecies;
		for (NxVertex vert : model.vertices)
		{
			vertecies.push_back(vert.x);
			vertecies.push_back(vert.y);
			vertecies.push_back(vert.z);
		}

		verteciesCombined.insert(verteciesCombined.end(), vertecies.begin(), vertecies.end());
	}
	return physicsSystem->GetFactory()->CreateConvexHullCollider(&verteciesCombined[0], verteciesCombined.size());
	return nullptr;
}

nPhysics::iShape* AssetManager::GenerateCollisionConvexHullFromModel(IndexedModel& indexedModel)
{
	NxArray<float>vertecies;
	for (NxVertex vert : indexedModel.vertices)
	{
		vertecies.push_back(vert.x);
		vertecies.push_back(vert.y);
		vertecies.push_back(vert.z);
	}
	return physicsSystem->GetFactory()->CreateConvexHullCollider(&vertecies[0], vertecies.size());
}

Material * AssetManager::LoadMaterial(MaterialSpec& matSpec)
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
