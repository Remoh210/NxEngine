#include "AssetLoader.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>
#include <stb_image.h>


//stbi_set_flip_vertically_on_load(true);

String AssetLoader::mModelDirectory = "NULL_DIR";
Array<String*> AssetLoader::mLoadedTextures;

bool AssetLoader::LoadModels(const String& fileName,
	Array<IndexedModel>& models, Array<uint32>& modelMaterialIndices,
	Array<Material>& materials)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(fileName.c_str(),
		aiProcess_Triangulate |
		aiProcess_GenSmoothNormals |
		aiProcess_FlipUVs |
		aiProcess_CalcTangentSpace);

	if (!scene) {
		DEBUG_LOG(LOG_TYPE_IO, LOG_ERROR, "Mesh load failed!: %s",
			fileName.c_str());
		return false;
	}

	for (uint32 j = 0; j < scene->mNumMeshes; j++) {
		const aiMesh* model = scene->mMeshes[j];
		modelMaterialIndices.push_back(model->mMaterialIndex);

		IndexedModel newModel;
		newModel.AllocateElement(3); // Positions
		newModel.AllocateElement(2); // TexCoords
		newModel.AllocateElement(3); // Normals
		newModel.AllocateElement(3); // Tangents
		newModel.SetInstancedElementStartIndex(4); // Begin instanced data
		newModel.AllocateElement(16); // Transform matrix

		const aiVector3D aiZeroVector(0.0f, 0.0f, 0.0f);
		for (uint32 i = 0; i < model->mNumVertices; i++) {
			const aiVector3D pos = model->mVertices[i];
			const aiVector3D normal = model->mNormals[i];
			const aiVector3D texCoord = model->HasTextureCoords(0)
				? model->mTextureCoords[0][i] : aiZeroVector;
			const aiVector3D tangent = model->mTangents[i];

			newModel.AddElement3f(0, pos.x, pos.y, pos.z);
			newModel.AddElement2f(1, texCoord.x, texCoord.y);
			newModel.AddElement3f(2, normal.x, normal.y, normal.z);
			newModel.AddElement3f(3, tangent.x, tangent.y, tangent.z);
		}
		for (uint32 i = 0; i < model->mNumFaces; i++)
		{
			const aiFace& face = model->mFaces[i];
			assert(face.mNumIndices == 3);
			newModel.AddIndices3i(face.mIndices[0], face.mIndices[1],
				face.mIndices[2]);
		}

		models.push_back(newModel);
	}

	for (uint32 i = 0; i < scene->mNumMaterials; i++) {
		const aiMaterial* material = scene->mMaterials[i];
		Material spec;

		// Currently only handles diffuse textures.
		aiString texturePath;
		if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0 &&
			material->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath)
			!= AI_SUCCESS) {
			String str(texturePath.data);
			spec.textureNames["diffuse"] = str;
		}
		//materials.push_back(spec);
	}

	return true;
}


unsigned int AssetLoader::TextureFromFile(String path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
	stbi_set_flip_vertically_on_load(true);
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








void AssetLoader::LoadModel(const String& fileName,
	Array<IndexedModel>& models, Array<uint32>& modelMaterialIndices,
	Array<Material>& materials)
{
	//Create static mesh 

	// read file via ASSIMP
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(fileName, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
	// check for errors
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
	{
		std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
		return;
	}
;

	//process ASSIMP's root node recursively
	ProcessNode(scene->mRootNode, scene, fileName, models, modelMaterialIndices, materials);

	//cStaticMesh* staticMesh = new cStaticMesh(vec_mesh_temp);
	//vec_mesh_temp.clear();

	//vec_static_mesh.push_back(staticMesh);
}


void AssetLoader::ProcessNode(aiNode *node, const aiScene *scene, const String& fileName,
	Array<IndexedModel>& models, Array<uint32>& modelMaterialIndices,
	Array<Material>& materials)
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

void AssetLoader::ProcessMesh(aiMesh* mesh, const aiScene* scene, const String& fileName,
	Array<IndexedModel>& models, Array<uint32>& modelMaterialIndices,
	Array<Material>& materials)
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
		const aiVector3D normal = mesh->mNormals[i];
		const aiVector3D texCoord = mesh->HasTextureCoords(0)
			? mesh->mTextureCoords[0][i] : aiZeroVector;
		const aiVector3D tangent = mesh->mTangents[i];

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
	Material spec;

	LoadMaterialTextures(fileName, material, aiTextureType_DIFFUSE, spec, "texture_diffuse");
	LoadMaterialTextures(fileName, material, aiTextureType_SPECULAR, spec, "texture_specular");
	LoadMaterialTextures(fileName, material, aiTextureType_NORMALS, spec, "texture_normal");
	LoadMaterialTextures(fileName, material, aiTextureType_HEIGHT, spec, "texture_normal");
	LoadMaterialTextures(fileName, material, aiTextureType_AMBIENT, spec, "texture_height");

	materials.push_back(spec);

}


void AssetLoader::LoadMaterialTextures(const String& filePath, aiMaterial *mat, aiTextureType type, Material& material, std::string typeName)
{

	// retrieve the directory path of the filepath
	String myDirectory = filePath.substr(0, filePath.find_last_of('/'));

	String TexturePath;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		std::string mystr(str.C_Str());
		DEBUG_LOG_TEMP("Texture Name: %s", str.C_Str());
		String TexturePath = myDirectory + "/" + str.C_Str();
		material.textureNames[typeName] = TexturePath;
	}
}