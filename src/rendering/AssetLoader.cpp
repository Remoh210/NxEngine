#include "AssetLoader.h"

#include <iostream>

#include "Common/CommonTypes.h"
#include "Core/FileSystem/FileSystem.h"

#include "assimp/Importer.hpp"
#include "assimp/pbrmaterial.h"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include "stb_image.h"


Array<NString*> AssetLoader::mLoadedTextures;

void AssetLoader::SetShouldFlipVTexture(bool bValue)
{
	stbi_set_flip_vertically_on_load(bValue);
}

bool AssetLoader::LoadModel(const NString& fileName,
	Array<IndexedModel>& models, Array<uint32>& modelMaterialIndices,
	Array<MaterialSpec>& materials)
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
;

	//process ASSIMP's root node recursively
	ProcessNode(scene->mRootNode, scene, fileName, models, modelMaterialIndices, materials);

	return true;
}


void AssetLoader::ProcessNode(aiNode *node, const aiScene *scene, const NString& fileName,
	Array<IndexedModel>& models, Array<uint32>& modelMaterialIndices,
	Array<MaterialSpec>& materials)
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
	Array<IndexedModel>& models, Array<uint32>& modelMaterialIndices,
	Array<MaterialSpec>& materials)
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
	for (uint32 i = 0; i < mesh->mNumVertices; i++) {
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

	//TODO: Put in func
	//Load PRB glFT
	const NString& fileExtension = GetFileExtension(fileName);
	if(fileExtension == ".glb" || fileExtension == ".gltf")
	{
		aiString fileMetallicRoughness;
		material->GetTexture(AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_METALLICROUGHNESS_TEXTURE, &fileMetallicRoughness);
		auto texture = scene->GetEmbeddedTexture(fileMetallicRoughness.C_Str());
		if (texture != nullptr)
		{
			NString filename = fileName.substr(fileName.find_last_of('/') + 1);
			//filename = filePath.substr(filename.size(), filePath.find_last_of('.'));
			Texture* newTexture = new Texture(TextureFromAssimp(texture));
			spec.textures[TEXTURE_MR] = newTexture;
		}
		else
		{
			NString myDirectory = fileName.substr(0, fileName.find_last_of('/'));
			NString TexturePath = myDirectory + "/" + fileMetallicRoughness.C_Str();
			spec.textureNames[TEXTURE_MR] = TexturePath;
			DEBUG_LOG_TEMP("Texture Name: %s", fileMetallicRoughness.C_Str());;
		}

		aiString fileAlbedo;
		material->GetTexture(AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_BASE_COLOR_TEXTURE, &fileAlbedo);
		auto textureAlbedo = scene->GetEmbeddedTexture(fileAlbedo.C_Str());
		if (textureAlbedo != nullptr)
		{
			NString filename = fileName.substr(fileName.find_last_of('/') + 1);
			//filename = filePath.substr(filename.size(), filePath.find_last_of('.'));
			Texture* newTexture = new Texture(TextureFromAssimp(textureAlbedo));
			spec.textures[TEXTURE_ALBEDO] = newTexture;
		}
		else
		{
			NString myDirectory = fileName.substr(0, fileName.find_last_of('/'));
			NString TexturePath = myDirectory + "/" + fileAlbedo.C_Str();
			spec.textureNames[TEXTURE_ALBEDO] = TexturePath;
			DEBUG_LOG_TEMP("Texture Name: %s", fileAlbedo.C_Str());;
		}
	}





	materials.push_back(spec);

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

