#include "SceneManager.h"
//#include <cstdio>
//#include <fstream>
//#include <iostream>
//
#include "Common/Transform/Transform.h"

//#include <rapidjson/document.h>
//#include <rapidjson/filereadstream.h>
//#include <rapidjson/filewritestream.h>
//#include <rapidjson/writer.h>
//#include <rapidjson/stringbuffer.h>
//#include <rapidjson/prettywriter.h>
//#include <fstream>
//
#include "Core/Systems/RenderSystem.h"
#include "Core/Components/StaticMeshComponent/StaticMeshComponent.h"
#include "Core/Camera/Camera.h"
#include "Core/FileSystem/FileSystem.h"
#include "Core/Application/AssetManager/AssetManager.h"
#include "Core/Graphics/ShaderManager/ShaderManager.h"
#include "Core/Engine/Serialization/JSON.h"



Scene 		          SceneManager::currentScene;
RenderDevice*         SceneManager::renderDevice = nullptr;
NxMap<NString, Shader*> SceneManager::shaders;
ECS::World*           SceneManager::world = nullptr;

void Scene::Clear()
{
	sceneObjects.clear();
}
 
void SceneManager::ClearScene()
{
	//Remove entitties from ECS
	if (!world)
	{
		return;
	}

	DEBUG_LOG_TEMP("Num SceneObj: %d", currentScene.sceneObjects.size());

	for (int i = 0; i < currentScene.GetNumObjects(); i++)
	{
		ECS::ComponentHandle<StaticMeshComponent> staticMeshComp = currentScene.sceneObjects[i]->get<StaticMeshComponent>();
		DEBUG_LOG_TEMP("About to delete: %s", staticMeshComp->meshAssetFile.c_str());
		world->destroy(currentScene.sceneObjects[i]);
	}

	currentScene.Clear();

}


bool SceneManager::SaveScene(NString filename, Camera& camera)
{
	//	if(!world)
	//	{
	//		//TODO: Log
	//		return false;
	//	}
	//
	//	rapidjson::Document doc;
	//	rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();
	//	doc.SetObject();
	//
	//#pragma region camera
	//	rapidjson::Value CameraObj(rapidjson::kObjectType);
	//	rapidjson::Value CameraSpeed(camera.MovementSpeed);
	//	rapidjson::Value CameraPosArray(rapidjson::kArrayType);
	//
	//	 for (int i = 0; i < 3; i++)
	//	 {
	//	 	rapidjson::Value temp(camera.Position[i]);
	//	 	CameraPosArray.PushBack(temp, allocator);
	//	 }
	//
	//	 rapidjson::Value CamYaw(camera.Yaw);
	//
	//	 rapidjson::Value CamPitch(camera.Pitch);
	//	
	//	 CameraObj.AddMember("Speed", CameraSpeed, allocator);
	//	 CameraObj.AddMember("Position", CameraPosArray, allocator);
	//	 CameraObj.AddMember("Yaw", CamYaw, allocator);
	//	 CameraObj.AddMember("Pitch", CamPitch, allocator);
	//#pragma endregion camera
	//
	//#pragma components
	//
	//	 rapidjson::Value MeshArray(rapidjson::kArrayType);
	//	 for (const auto& entity : currentScene.sceneObjects)
	//	 {
	//	 	//if (/*!Entity->bIsDebug*/) {continue;}
	//
	//	 	rapidjson::Value ObjValue(rapidjson::kObjectType);
	//
	//		ECS::ComponentHandle<StaticMeshComponent> staticMeshComp = entity->get<StaticMeshComponent>();
	//
	//	 	if (!staticMeshComp) { DEBUG_LOG("ENGINE", "ERROR", "No RenderComponent"); return false; }
	//
	//	 	rapidjson::Value meshFileName(staticMeshComp->meshAssetFile.c_str(), allocator);
	//
	//		rapidjson::Value subMeshArray(rapidjson::kArrayType);
	//		for(MeshInfo* meshInfo : staticMeshComp->meshes)
	//		{
	//			rapidjson::Value meshInfoObjectValue(rapidjson::kObjectType);
	//			rapidjson::Value materialValue(rapidjson::kObjectType);
	//
	//			//if(meshInfo->material->diffuseTextures.size() > 0)
	//			//{
	//			//	rapidjson::Value textureArray(rapidjson::kArrayType);
	//			//	for(Texture* texture : meshInfo->material->diffuseTextures)
	//			//	{
	//			//		rapidjson::Value textureObjectValue(rapidjson::kObjectType);
	//			//		rapidjson::Value textureNameValue(texture->GetFileName().c_str(), allocator);
	//			//		textureObjectValue.AddMember("TextureFile", textureNameValue, allocator);
	//			//		rapidjson::Value isCompressedValue(texture->IsCompressed());
	//			//		textureObjectValue.AddMember("IsCompressed", isCompressedValue, allocator);
	//			//		rapidjson::Value hasMipMapsValue(texture->HasMipmaps());
	//			//		textureObjectValue.AddMember("HasMipmaps", hasMipMapsValue, allocator);
	//			//		textureArray.PushBack(textureObjectValue, allocator);
	//			//	}
	//			//	materialValue.AddMember("DiffuseTexures", textureArray, allocator);
	//			//}
	//			//else
	//			//{
	//			//	rapidjson::Value colorValue(rapidjson::kArrayType);
	//	 	//		for (int i = 0; i < 3; i++) {
	//	 	//			rapidjson::Value temp(meshInfo->material->color[i]);
	//	 	//			colorValue.PushBack(temp, allocator);
	//	 	//		}
	//			//	materialValue.AddMember("DiffuseColor", colorValue, allocator);
	//			//}
	//			meshInfoObjectValue.AddMember("Material", materialValue, allocator);
	//			subMeshArray.PushBack(meshInfoObjectValue, allocator);
	//
	//		}
	//
	//
	//	 	//rapidjson::Value Visible(RendComp->bIsVisible);
	//	 	//rapidjson::Value Shader(RendComp->shader., allocator);
	//	 	//rapidjson::Value WireFrame(RendComp->bIsWireFrame);
	//	 	//rapidjson::Value DiffuseRGBArray(rapidjson::kArrayType);
	//
	//	 	//for (int i = 0; i < 4; i++) {
	//	 	//	rapidjson::Value temp(RendComp->diffuse[i]);
	//	 	//	DiffuseRGBArray.PushBack(temp, allocator);
	//	 	//}
	//
	//		
	//	 	ECS::ComponentHandle<TransformComponent> TransComp = entity->get<TransformComponent>();
	//	 	if (!TransComp) { DEBUG_LOG("ENGINE", "ERROR", "No TransformComponent"); return false; }
	//	 	rapidjson::Value PositionArray(rapidjson::kArrayType);
	//	 	rapidjson::Value Rotation(rapidjson::kArrayType);
	//	 	rapidjson::Value Scale(rapidjson::kArrayType);
	//	 	for (int i = 0; i < 3; i++) {
	//	 		rapidjson::Value temp(TransComp->transform.position[i]);
	//	 		PositionArray.PushBack(temp, allocator);
	//	 	}
	//	 	for (int i = 0; i < 3; i++) {
	//	 		vec3 rot = TransComp->transform.rotation;
	//	 		rapidjson::Value temp(rot[i]);
	//	 		Rotation.PushBack(temp, allocator);
	//	 	}
	//	 	for (int i = 0; i < 3; i++) {
	//	 		rapidjson::Value temp(TransComp->transform.scale[i]);
	//	 		Scale.PushBack(temp, allocator);
	//	 	}
	//
	//
	//	 	ObjValue.AddMember("MeshFile", meshFileName, allocator);
	//		ObjValue.AddMember("SubMeshArray", subMeshArray, allocator);
	//		//ObjValue.AddMember("ShaderFileName", Shader, allocator);
	//	 	//ObjValue.AddMember("Visible", Visible, allocator);
	//	 	
	//	 	//ObjValue.AddMember("Wireframe", WireFrame, allocator);
	//	 	ObjValue.AddMember("Position", PositionArray, allocator);
	//	 	//ObjValue.AddMember("DiffuseRGB_Alpha", DiffuseRGBArray, allocator);
	//	 	ObjValue.AddMember("Rotation", Rotation, allocator);
	//	 	ObjValue.AddMember("Scale", Scale, allocator);
	//
	//	 	MeshArray.PushBack(ObjValue, allocator);
	//
	//	 }
	//#pragma endregion components
	//
	//	 doc.AddMember("Camera", CameraObj, allocator);
	//	 doc.AddMember("GameObjects", MeshArray, allocator);




	//char writeBuffer[65536];
	//std::string fileToLoadFullPath = Nx::FileSystem::GetRoot() + "/res/Scenes/" + filename + ".json";
	//FILE* fp = fopen(fileToLoadFullPath.c_str(), "wb"); // non-Windows use "w"

	//std::ofstream outputFile;
	//outputFile.open(fileToLoadFullPath.c_str());

	//for (const auto& entity : currentScene.sceneObjects)
	//{
	//	

	//	ECS::ComponentHandle<TransformComponent> transformComp = entity->get<TransformComponent>();

	//	if(!transformComp)
	//	{
	//		continue;
	//	}

	//	NString testJSON = Nx::to_json(transformComp.get());
	//	outputFile << testJSON;
	//	DEBUG_LOG_TEMP("testJSON: %s", testJSON.c_str());

	//	NString str = rttr::type::get(transformComp.get()).get_name().to_string();
	//	auto props = rttr::type::get(transformComp.get()).get_properties();
	//	for (auto prop : props)
	//	{
	//		DEBUG_LOG_TEMP("%s", prop.get_type().get_name().to_string().c_str());

	//		auto childProp = rttr::type::get(prop).get_properties();
	//		for (auto propChild : childProp)
	//		{
	//			DEBUG_LOG_TEMP("%s", propChild.get_type().get_name().to_string().c_str());
	//		}
	//	}
	//}
	//outputFile.close();

	// 

	// DEBUG_LOG_TEMP("SCENE: %s", fileToLoadFullPath.c_str());

	// 

	// //rapidjson::FileWriteStream os(fp, writeBuffer, sizeof(writeBuffer));
	// //rapidjson::PrettyWriter<rapidjson::FileWriteStream> writer(os);
	// //doc.Accept(writer);
	// 

	return true;
}

bool SceneManager::LoadScene(NString filename, class Camera& camera) 
{
	//if (!world)
	//{
	//	//TODO: Log
	//	return false;
	//}

	//ClearScene();
	////return false;

	//std::string fileToLoadFullPath = Nx::FileSystem::GetRoot() + "/res/Scenes/" + filename;

	//rapidjson::Document doc;
	//FILE* fp = fopen(fileToLoadFullPath.c_str(), "rb"); // non-Windows use "r"
	//if (!fp)
	//{
 //       DEBUG_LOG("SceneMager", "ERROR", "No such scene: %s", fileToLoadFullPath.c_str());
	//	return false;
	//}
	//char readBuffer[65536];
	//rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));
	//doc.ParseStream(is);
	//fclose(fp);

	////Camera
	//float Speed;
	//glm::vec3 CameraPos;
	//rapidjson::Value& CameraPosArray = doc["Camera"]["Position"];

	//camera.MovementSpeed = doc["Camera"]["Speed"].GetFloat();
	//for (rapidjson::SizeType i = 0; i < CameraPosArray.Size(); i++)
	//{
	//	CameraPos[i] = CameraPosArray[i].GetFloat();
	//	std::cout << "CameraPos: [ " << CameraPos[i] << " ]" << std::endl;
	//}
	//camera.Position = CameraPos;

	//camera.Yaw = doc["Camera"]["Yaw"].GetFloat();
	//camera.Pitch = doc["Camera"]["Pitch"].GetFloat();

	//camera.updateCameraVectors();

	//const rapidjson::Value& GameObjects = doc["GameObjects"];
	//for (int i = 0; i < GameObjects.Size(); i++)
	//{
	//	StaticMeshComponent staticMeshComp;
	//	NString meshFile = GameObjects[i]["MeshFile"].GetString();
	//	if(meshFile == "") 
	//	{
	//		DEBUG_LOG(LOG_ERROR, "Error", "No mesh file specified");
	//		continue;
	//	}
	//	staticMeshComp.meshAssetFile = meshFile;
	//	staticMeshComp.meshes = AssetManager::ImportModel(renderDevice, meshFile);
	//	staticMeshComp.shader = ShaderManager::GetMainShader();

	//	TransformComponent transformComponent;
	//	const rapidjson::Value& PositionArray = GameObjects[i]["Position"];
	//	for (rapidjson::SizeType i = 0; i < PositionArray.Size(); i++) 
	//	{
	//		transformComponent.transform.position[i] = PositionArray[i].GetFloat();
	//	}
	//	const rapidjson::Value& RotationArray = GameObjects[i]["Rotation"];
	//	for (rapidjson::SizeType i = 0; i < RotationArray.Size(); i++)
	//	{
	//		transformComponent.transform.rotation[i] = RotationArray[i].GetFloat();
	//	}
	//	const rapidjson::Value& ScaleArray = GameObjects[i]["Scale"];
	//	for (rapidjson::SizeType i = 0; i < ScaleArray.Size(); i++) 
	//	{
	//		transformComponent.transform.scale[i] = ScaleArray[i].GetFloat();
	//	}
	//	
	//	ECS::Entity* ent = world->create();
	//	ent->assign<TransformComponent>(transformComponent);
	//	ent->assign<StaticMeshComponent>(staticMeshComp);

	//	currentScene.sceneObjects.push_back(ent);
	//}

	return true;
}
