#include "AssetLoader.h"

#include <iostream>

#include "Common/CommonTypes.h"
#include "Core/FileSystem/FileSystem.h"

#include "assimp/Importer.hpp"
#include "assimp/pbrmaterial.h"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include "stb_image.h"

glm::mat4 AIMatrixToGLMMatrix(const aiMatrix4x4 &mat)
{
	return glm::mat4(
		mat.a1, mat.b1, mat.c1, mat.d1,
		mat.a2, mat.b2, mat.c2, mat.d2,
		mat.a3, mat.b3, mat.c3, mat.d3,
		mat.a4, mat.b4, mat.c4, mat.d4);
}

NxArray<NString*> AssetLoader::mLoadedTextures;

void AssetLoader::SetShouldFlipVTexture(bool bValue)
{
	stbi_set_flip_vertically_on_load(bValue);
}

bool AssetLoader::LoadModel(const NString& fileName,
	NxArray<IndexedModel>& models, NxArray<uint32>& modelMaterialIndices,
	NxArray<MaterialSpec>& materials)
{
	NString absoluteFilePath = Nx::FileSystem::GetPath(fileName);
	//Create static mesh 

	// read file via ASSIMP
	Assimp::Importer importer;
	//Do not use aiProcess_FlipUVs since we flipping the texture with stbimage
	const aiScene* scene = importer.ReadFile(absoluteFilePath, aiProcess_Triangulate| aiProcess_CalcTangentSpace);
	// check for errors
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
	{
		std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
		return false;
	}

	//process ASSIMP's root node recursively
	ProcessNode(scene->mRootNode, scene, fileName, models, modelMaterialIndices, materials);

	return true;
}


const aiScene* AssetLoader::LoadModelSkeletal(const NString& fileName, NxArray<IndexedModel>& models, SkeletalData& skelData, NxArray<uint32>& modelMatIndices, NxArray<MaterialSpec>& matArray)
{
	NString absoluteFilePath = Nx::FileSystem::GetPath(fileName);
	//Create static mesh 

	// read file via ASSIMP
	Assimp::Importer importer;
	//Do not use aiProcess_FlipUVs since we flipping the texture with stbimage
	
	importer.ReadFile(absoluteFilePath, aiProcess_Triangulate | aiProcess_CalcTangentSpace);
	const aiScene* scene = importer.GetOrphanedScene();
	// check for errors
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
	{
		std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
		return nullptr;
	}

	skelData.m_numberOfVertices = scene->mMeshes[0]->mNumVertices;

	// This is the vertex information for JUST the bone stuff
	//skelData.vecVertexBoneData.resize(this->m_numberOfVertices);

	//process ASSIMP's root node recursively
	//ProcessNode(scene->mRootNode, scene, fileName, models, modelMatIndices, matArray, true);\

	NxArray<VertexBoneData> vecBoneData(skelData.m_numberOfVertices);
	LoadBones(scene->mMeshes[0], skelData, vecBoneData);

	ProcessMeshSkeletal(scene->mMeshes[0], scene, fileName, models, modelMatIndices, matArray, vecBoneData);

	return scene;
}

void AssetLoader::ProcessNode(aiNode *node, const aiScene *scene, const NString& fileName,
	NxArray<IndexedModel>& models, NxArray<uint32>& modelMaterialIndices,
	NxArray<MaterialSpec>& materials)
{
	// process each mesh located at the current node
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		// the node object only contains indices to index the actual objects in the scene. 
		// the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		ProcessMesh(mesh, scene, fileName, models, modelMaterialIndices, materials);
	}
	// after we've processed all of the meshes (if any) we then recursively process each of the children nodes
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		ProcessNode(node->mChildren[i], scene, fileName, models, modelMaterialIndices, materials);
	}
}

void AssetLoader::ProcessMesh(aiMesh* mesh, const aiScene* scene, const NString& fileName,
	NxArray<IndexedModel>& models, NxArray<uint32>& modelMaterialIndices,
	NxArray<MaterialSpec>& materials)
{

	modelMaterialIndices.push_back(mesh->mMaterialIndex);

	IndexedModel newModel;
	newModel.AllocateElement(3); // Positions
	newModel.AllocateElement(2); // TexCoords
	newModel.AllocateElement(3); // Normals
	newModel.AllocateElement(3); // Tangents

	newModel.SetInstancedElementStartIndex(4); // Begin instanced data
	
	newModel.AllocateElement(16); // Transform matrix

	const aiVector3D aiZeroVector(0.0f, 0.0f, 0.0f);
	for (uint32 i = 0; i < mesh->mNumVertices; i++)
	{
		const aiVector3D pos = mesh->mVertices[i];
		const aiVector3D normal = mesh->HasNormals()
			? mesh->mNormals[i] : aiZeroVector;
		const aiVector3D texCoord = mesh->HasTextureCoords(0)
			? mesh->mTextureCoords[0][i] : aiZeroVector;
		const aiVector3D tangent = mesh->HasTangentsAndBitangents()
			? mesh->mTangents[i] : aiZeroVector;

		newModel.AddElement3f(0, pos.x, pos.y, pos.z);
		newModel.AddElement2f(1, texCoord.x, texCoord.y);
		newModel.AddElement3f(2, normal.x, normal.y, normal.z);
		newModel.AddElement3f(3, tangent.x, tangent.y, tangent.z);
	}
	
	for (uint32 i = 0; i < mesh->mNumFaces; i++)
	{
		const aiFace& face = mesh->mFaces[i];
		assert(face.mNumIndices == 3);
		newModel.AddIndices3i(face.mIndices[0], face.mIndices[1],
			face.mIndices[2]);
	}

	models.push_back(newModel);

	// process materials
	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
	MaterialSpec spec;


	LoadMaterialTextures(fileName, material, scene, aiTextureType_DIFFUSE, spec, TEXTURE_ALBEDO);
    //TODO: Implement specular workflow, for now we treat specular texture as metallic
	LoadMaterialTextures(fileName, material, scene, aiTextureType_SPECULAR, spec, TEXTURE_METALLIC);
	LoadMaterialTextures(fileName, material, scene, aiTextureType_NORMALS, spec, TEXTURE_NORMAL);
	LoadMaterialTextures(fileName, material, scene, aiTextureType_DIFFUSE_ROUGHNESS, spec, TEXTURE_ROUGHNESS);
	LoadMaterialTextures(fileName, material, scene, aiTextureType_AMBIENT_OCCLUSION, spec, TEXTURE_AO);
	//LoadMaterialTextures(fileName, material, scene, aiTextureType_METALNESS, spec, TEXTURE_METALLIC);
	//LoadMaterialTextures(fileName, material, scene, aiTextureType_UNKNOWN, spec, TEXTURE_METALLIC);
	//LoadMaterialTextures(fileName, material, scene, aiTextureType_AMBIENT, spec, TEXTURE_AO);

	//For combined MR textures
	LoadMRTextures(fileName, material, scene, spec);


	materials.push_back(spec);
}


void AssetLoader::ProcessMeshSkeletal(aiMesh* mesh, const aiScene* scene, const NString& fileName,
	NxArray<IndexedModel>& models, NxArray<uint32>& modelMaterialIndices,
	NxArray<MaterialSpec>& materials, const NxArray<VertexBoneData>& boneData)
{

	modelMaterialIndices.push_back(mesh->mMaterialIndex);

	IndexedModel newModel;
	newModel.AllocateElement(3); // Positions
	newModel.AllocateElement(2); // TexCoords
	newModel.AllocateElement(3); // Normals
	newModel.AllocateElement(3); // Tangents


	newModel.AllocateElement(4); // Bone IDs
	newModel.AllocateElement(4); // Bone Weights
	newModel.SetInstancedElementStartIndex(6); // Begin instanced data

	newModel.AllocateElement(16); // Transform matrix
	newModel.AllocateElement(16); // TransformInvT matrix

	const aiVector3D aiZeroVector(0.0f, 0.0f, 0.0f);
	for (uint32 i = 0; i < mesh->mNumVertices; i++)
	{
		const aiVector3D pos = mesh->mVertices[i];
		const aiVector3D normal = mesh->HasNormals()
			? mesh->mNormals[i] : aiZeroVector;
		const aiVector3D texCoord = mesh->HasTextureCoords(0)
			? mesh->mTextureCoords[0][i] : aiZeroVector;
		const aiVector3D tangent = mesh->HasTangentsAndBitangents()
			? mesh->mTangents[i] : aiZeroVector;

		newModel.AddElement3f(0, pos.x, pos.y, pos.z);
		newModel.AddElement2f(1, texCoord.x, texCoord.y);
		newModel.AddElement3f(2, normal.x, normal.y, normal.z);
		newModel.AddElement3f(3, tangent.x, tangent.y, tangent.z);


		//// 4Bones: ids and weights
		newModel.AddElement4f
		(
			4,
			boneData[i].ids[0],
			boneData[i].ids[1],
			boneData[i].ids[2],
			boneData[i].ids[3]
		);
		//// 4Bones: ids and weights
		newModel.AddElement4f
		(
			5,
			boneData[i].weights[0],
			boneData[i].weights[1],
			boneData[i].weights[2],
			boneData[i].weights[3]
		);		
	}

	for (uint32 i = 0; i < mesh->mNumFaces; i++)
	{
		const aiFace& face = mesh->mFaces[i];
		assert(face.mNumIndices == 3);
		newModel.AddIndices3i(face.mIndices[0], face.mIndices[1],
			face.mIndices[2]);
	}

	models.push_back(newModel);

	// process materials
	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
	MaterialSpec spec;


	LoadMaterialTextures(fileName, material, scene, aiTextureType_DIFFUSE, spec, TEXTURE_ALBEDO);
	//TODO: Implement specular workflow, for now we treat specular texture as metallic
	LoadMaterialTextures(fileName, material, scene, aiTextureType_SPECULAR, spec, TEXTURE_METALLIC);
	LoadMaterialTextures(fileName, material, scene, aiTextureType_NORMALS, spec, TEXTURE_NORMAL);
	LoadMaterialTextures(fileName, material, scene, aiTextureType_DIFFUSE_ROUGHNESS, spec, TEXTURE_ROUGHNESS);
	LoadMaterialTextures(fileName, material, scene, aiTextureType_AMBIENT_OCCLUSION, spec, TEXTURE_AO);
	//LoadMaterialTextures(fileName, material, scene, aiTextureType_METALNESS, spec, TEXTURE_METALLIC);
	//LoadMaterialTextures(fileName, material, scene, aiTextureType_UNKNOWN, spec, TEXTURE_METALLIC);
	//LoadMaterialTextures(fileName, material, scene, aiTextureType_AMBIENT, spec, TEXTURE_AO);

	//For combined MR textures
	LoadMRTextures(fileName, material, scene, spec);

	//TODO: Put in func


	materials.push_back(spec);

}


void AssetLoader::LoadBones(const aiMesh* Mesh, SkeletalData& skelData, NxArray<VertexBoneData>& boneData)
{
	for (unsigned int boneIndex = 0; boneIndex != Mesh->mNumBones; boneIndex++)
	{
		unsigned int BoneIndex = 0;
		std::string BoneName(Mesh->mBones[boneIndex]->mName.data);

		std::map<std::string, unsigned int>::iterator it = skelData.m_mapBoneNameToBoneIndex.find(BoneName);
		if (it == skelData.m_mapBoneNameToBoneIndex.end())
		{
			BoneIndex = skelData.mNumBones;
			skelData.mNumBones++;
			BoneInfo bi;
			skelData.mBoneInfo.push_back(bi);

			skelData.mBoneInfo[BoneIndex].BoneOffset = AIMatrixToGLMMatrix(Mesh->mBones[boneIndex]->mOffsetMatrix);
			skelData.m_mapBoneNameToBoneIndex[BoneName] = BoneIndex;
		}
		else
		{
			BoneIndex = it->second;
		}

		for (unsigned int WeightIndex = 0; WeightIndex != Mesh->mBones[boneIndex]->mNumWeights; WeightIndex++)
		{
			unsigned int VertexID = /*mMeshEntries[MeshIndex].BaseVertex +*/ Mesh->mBones[boneIndex]->mWeights[WeightIndex].mVertexId;
			float Weight = Mesh->mBones[boneIndex]->mWeights[WeightIndex].mWeight;
			boneData[VertexID].AddBoneData(BoneIndex, Weight);
		}
	}
	return;
}

void AssetLoader::LoadMaterialTextures(const NString& filePath, aiMaterial *mat, const aiScene* scene, aiTextureType type, MaterialSpec& material, NString typeName)
{

	// retrieve the directory path of the filepath
	NString myDirectory = filePath.substr(0, filePath.find_last_of('/'));

	NString TexturePath;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		auto texture = scene->GetEmbeddedTexture(str.C_Str());
		if (texture != nullptr)
		{
			NString filename = filePath.substr(filePath.find_last_of('/') + 1);
			//filename = filePath.substr(filename.size(), filePath.find_last_of('.'));
			Texture* newTexture = new Texture(TextureFromAssimp(texture));
			material.textures[typeName] = newTexture;
		}
		else 
		{
			TexturePath = myDirectory + "/" + str.C_Str();
			material.textureNames[typeName] = TexturePath;
			DEBUG_LOG_TEMP("Texture Name: %s", str.C_Str());
		}
	}
}

void AssetLoader::LoadMRTextures(const NString& filePath, aiMaterial* mat, const aiScene* scene, MaterialSpec& spec)
{
	//Load PRB glFT
	const NString& fileExtension = GetFileExtension(filePath);
	if (fileExtension == ".glb" || fileExtension == ".gltf")
	{
		aiString fileMetallicRoughness;
		if (mat->GetTexture(AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_METALLICROUGHNESS_TEXTURE, &fileMetallicRoughness) == aiReturn_SUCCESS)
		{
			auto texture = scene->GetEmbeddedTexture(fileMetallicRoughness.C_Str());
			if (texture != nullptr)
			{
				NString filename = filePath.substr(filePath.find_last_of('/') + 1);
				//filename = filePath.substr(filename.size(), filePath.find_last_of('.'));
				Texture* newTexture = new Texture(TextureFromAssimp(texture));
				spec.textures[TEXTURE_MR] = newTexture;
			}
			else
			{
				NString myDirectory = filePath.substr(0, filePath.find_last_of('/'));
				NString TexturePath = myDirectory + "/" + fileMetallicRoughness.C_Str();
				spec.textureNames[TEXTURE_MR] = TexturePath;
				DEBUG_LOG_TEMP("Texture Name: %s", fileMetallicRoughness.C_Str());;
			}
		}

		aiString fileAlbedo;
		if (mat->GetTexture(AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_BASE_COLOR_TEXTURE, &fileAlbedo) == aiReturn_SUCCESS)
		{
			auto textureAlbedo = scene->GetEmbeddedTexture(fileAlbedo.C_Str());
			if (textureAlbedo != nullptr)
			{
				NString filename = filePath.substr(filePath.find_last_of('/') + 1);
				//filename = filePath.substr(filename.size(), filePath.find_last_of('.'));
				Texture* newTexture = new Texture(TextureFromAssimp(textureAlbedo));
				spec.textures[TEXTURE_ALBEDO] = newTexture;
			}
			else
			{
				NString myDirectory = filePath.substr(0, filePath.find_last_of('/'));
				NString TexturePath = myDirectory + "/" + fileAlbedo.C_Str();
				spec.textureNames[TEXTURE_ALBEDO] = TexturePath;
				DEBUG_LOG_TEMP("Texture Name: %s", fileAlbedo.C_Str());;
			}
		}
	}
	else
	{
		//TODO
	}
}

unsigned int AssetLoader::TextureFromAssimp(const aiTexture* texture)
{
	//TODO: Move To RenderDevice
	unsigned int textureID;
	glGenTextures(1, &textureID);

	texture->achFormatHint;


	stbi_set_flip_vertically_on_load(true);


	unsigned char *image_data = nullptr;
	int width, height, components_per_pixel;

	//glPixelStorei(GL_UNPACK_ALIGNMENT, 1); //JPG?

	if (texture->mHeight == 0)
	{
		image_data = stbi_load_from_memory(reinterpret_cast<unsigned char*>(texture->pcData), texture->mWidth, &width, &height, &components_per_pixel, 0);
	}
	else
	{
		image_data = stbi_load_from_memory(reinterpret_cast<unsigned char*>(texture->pcData), texture->mWidth * texture->mHeight, &width, &height, &components_per_pixel, 0);
	}

	GLenum format;
	if (components_per_pixel == 1)
		format = GL_RED;
	else if (components_per_pixel == 2)
		format = GL_RG;
	else if (components_per_pixel == 3)
		format = GL_RGB;
	else if (components_per_pixel == 4)
		format = GL_RGBA;


	glBindTexture(GL_TEXTURE_2D, textureID);
	//glTexImage2D(GL_TEXTURE_2D, 0, format, texture->mWidth, texture->mHeight, 0, format, GL_UNSIGNED_BYTE, texture->pcData);
	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE,
		image_data);
	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return textureID;
}

unsigned int AssetLoader::TextureFromFile(NString path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	uint8 *data = stbi_load(path.c_str(), &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

