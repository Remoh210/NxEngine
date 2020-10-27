#include "SceneManager.h"
//#include <cstdio>
//#include <fstream>
//#include <iostream>
//
#include "Common/Transform/Transform.h"
//
#include "Core/Systems/RenderSystem.h"
#include "Core/Camera/Camera.h"
#include "Core/FileSystem/FileSystem.h"

#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>
#include <rapidjson/filewritestream.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>

//#include <cstdio>
//

Scene SceneManager::currentScene;
ECS* SceneManager::ecs = nullptr;
//
//
void SceneManager::RemoveObjectFromScene(EntityHandle entity)
{
	if (!ecs)
	{
		//TODO: Log
		return;
	}
	if (currentScene.sceneObjects.Find(entity))
	{
		currentScene.sceneObjects.Remove(entity);
		ecs->RemoveEntity(entity);
	}

}


bool SceneManager::SaveScene(NString filename, Camera& camera) 
{
	if(!ecs)
	{
		//TODO: Log
		return false;
	}

	rapidjson::Document doc;
	rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();
	doc.SetObject();
	rapidjson::Value MeshArray(rapidjson::kArrayType);
	
	 //Camera Output
	 rapidjson::Value CameraObj(rapidjson::kObjectType);
	 rapidjson::Value CameraSpeed(camera.MovementSpeed);
	 rapidjson::Value CameraPosArray(rapidjson::kArrayType);

	 for (int i = 0; i < 3; i++)
	 {
	 	rapidjson::Value temp(camera.Position[i]);
	 	CameraPosArray.PushBack(temp, allocator);
	 }

	 rapidjson::Value CamYaw(camera.Yaw);

	 rapidjson::Value CamPitch(camera.Pitch);
	
	 CameraObj.AddMember("Speed", CameraSpeed, allocator);
	 CameraObj.AddMember("Position", CameraPosArray, allocator);
	 CameraObj.AddMember("Yaw", CamYaw, allocator);
	 CameraObj.AddMember("Pitch", CamPitch, allocator);



	 //GameObjects
	 /*or rapidjson::Value myArray; ;
	      myArray.SetArray() */
	 for (const auto& entity : currentScene.sceneObjects)
	 {
	 	//if (/*!Entity->bIsDebug*/) {continue;}
	 	rapidjson::Value ObjValue(rapidjson::kObjectType);

		RenderableMeshComponent* RendComp = ecs->GetComponent<RenderableMeshComponent>(entity);
	 	if (!RendComp) { DEBUG_LOG("ENGINE", "ERROR", "No RenderComponent"); return false; }
		
	 	//rapidjson::Value FriendlyName(RendComp->.c_str(), allocator);
	 	//rapidjson::Value MeshName(RendComp->meshPath.c_str(), allocator);
	 	//rapidjson::Value Visible(RendComp->bIsVisible);
	 	//rapidjson::Value Shader(RendComp->shader.c_str(), allocator);
	 	//rapidjson::Value WireFrame(RendComp->bIsWireFrame);
	 	//rapidjson::Value DiffuseRGBArray(rapidjson::kArrayType);

	 	//for (int i = 0; i < 4; i++) {
	 	//	rapidjson::Value temp(RendComp->diffuse[i]);
	 	//	DiffuseRGBArray.PushBack(temp, allocator);
	 	//}


	 	TransformComponent* TransComp = ecs->GetComponent<TransformComponent>(entity);
	 	if (!TransComp) { DEBUG_LOG("ENGINE", "ERROR", "No TransformComponent"); return false; }
		
	 	rapidjson::Value PositionArray(rapidjson::kArrayType);
	 	rapidjson::Value Rotation(rapidjson::kArrayType);
	 	rapidjson::Value Scale(rapidjson::kArrayType);
		
	 	for (int i = 0; i < 3; i++) {
	 		rapidjson::Value temp(TransComp->transform.position[i]);
	 		PositionArray.PushBack(temp, allocator);
	 	}

	 	for (int i = 0; i < 4; i++) {
	 		quat rot = TransComp->transform.rotation;
	 		rapidjson::Value temp(rot[i]);
	 		Rotation.PushBack(temp, allocator);
	 	}

	 	for (int i = 0; i < 3; i++) {
	 		rapidjson::Value temp(TransComp->transform.scale[i]);
	 		Scale.PushBack(temp, allocator);
	 	}


	 	//ObjValue.AddMember("Name", FriendlyName, allocator);
	 	//ObjValue.AddMember("Mesh", MeshName, allocator);
	 	//ObjValue.AddMember("Visible", Visible, allocator);
	 	//ObjValue.AddMember("Shader", Shader, allocator);
	 	//ObjValue.AddMember("Wireframe", WireFrame, allocator);
	 	ObjValue.AddMember("Position", PositionArray, allocator);
	 	//ObjValue.AddMember("DiffuseRGB_Alpha", DiffuseRGBArray, allocator);
	 	ObjValue.AddMember("Rotation", Rotation, allocator);
	 	ObjValue.AddMember("Scale", Scale, allocator);

	 	MeshArray.PushBack(ObjValue, allocator);

	 }



	 doc.AddMember("Camera", CameraObj, allocator);
	 doc.AddMember("GameObjects", MeshArray, allocator);

	
	
	 std::string fileToLoadFullPath = Nx::FileSystem::GetRoot() + "/res/Scenes/" + filename + ".json";
	 DEBUG_LOG_TEMP("SCENE: %s", fileToLoadFullPath.c_str());

	 FILE* fp = fopen(fileToLoadFullPath.c_str(), "wb"); // non-Windows use "w"
	 char writeBuffer[65536];
	 rapidjson::FileWriteStream os(fp, writeBuffer, sizeof(writeBuffer));
	 rapidjson::PrettyWriter<rapidjson::FileWriteStream> writer(os);
	 doc.Accept(writer);
	 fclose(fp);

	return true;
}
//
//
//
//
bool SceneManager::LoadScene(NString filename, class Camera& camera) 
{
	if (!ecs)
	{
		//TODO: Log
		return false;
	}

	int count = currentScene.sceneObjects.size();
	for (int i = 0; i < count; i++)
	{
		//RemoveObjectFromScene(currentScene.sceneObjects[i]);
	}


    //currentScene.sceneObjects.clear();

	std::string fileToLoadFullPath = Nx::FileSystem::GetRoot() + "/res/Scenes/" + filename;


	rapidjson::Document doc;
	FILE* fp = fopen(fileToLoadFullPath.c_str(), "rb"); // non-Windows use "r"
	if (!fp)
	{
        DEBUG_LOG("SceneMager", "ERROR", "No such scene: %s", fileToLoadFullPath.c_str());
		return false;
	}
	char readBuffer[65536];
	rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));
	doc.ParseStream(is);
	fclose(fp);

	//Camera
	float Speed;
	glm::vec3 CameraPos;
	rapidjson::Value& CameraPosArray = doc["Camera"]["Position"];

	camera.MovementSpeed = doc["Camera"]["Speed"].GetFloat();
	for (rapidjson::SizeType i = 0; i < CameraPosArray.Size(); i++)
	{
		CameraPos[i] = CameraPosArray[i].GetFloat();
		std::cout << "CameraPos: [ " << CameraPos[i] << " ]" << std::endl;
	}
	camera.Position = CameraPos;

	camera.Yaw = doc["Camera"]["Yaw"].GetFloat();
	camera.Pitch = doc["Camera"]["Pitch"].GetFloat();

	camera.updateCameraVectors();

	////HACK HACK HACK CLEAR ALL ENTITIES BEFORE LOADING THE SCENE
	//for (const auto entity : ecs->GetEntities())
	//{




	//	//Game Objects
	//	const rapidjson::Value& GameObjects = doc["GameObjects"];


	//	for (rapidjson::SizeType i = 0; i < GameObjects.Size(); i++)
	//	{

	//		//RenderComponent renderComponent;
	//		//renderComponent.diffuse = vec4f(0.2f, 0.4f, 0.5f, 1.f);

	//		//renderComponent.mesh = GameObjects[i]["Name"].GetString();
	//		//renderComponent.bIsVisible = GameObjects[i]["Visible"].GetBool();
	//		//renderComponent.bIsWireFrame = GameObjects[i]["Wireframe"].GetBool();
	//		//renderComponent.meshPath = GameObjects[i]["Mesh"].GetString();
	//		//renderComponent.shader = GameObjects[i]["Shader"].GetString();

	//		//const rapidjson::Value& DiffuseArray = GameObjects[i]["DiffuseRGB_Alpha"];
	//		//for (rapidjson::SizeType i = 0; i < DiffuseArray.Size(); i++) {

	//		//	renderComponent.diffuse[i] = DiffuseArray[i].GetFloat();

	//		//}

	//		TransformComponent transformComponent;

	//		const rapidjson::Value& PositionArray = GameObjects[i]["Position"];
	//		for (rapidjson::SizeType i = 0; i < PositionArray.Size(); i++) {
	//			transformComponent.transform.position[i] = PositionArray[i].GetFloat();
	//		}


	//		const rapidjson::Value& RotationArray = GameObjects[i]["Rotation"];
	//		for (rapidjson::SizeType i = 0; i < RotationArray.Size(); i++) {
	//			transformComponent.transform.rotation[i] = RotationArray[i].GetFloat();
	//		}


	//		const rapidjson::Value& ScaleArray = GameObjects[i]["Scale"];
	//		for (rapidjson::SizeType i = 0; i < ScaleArray.Size(); i++) {

	//			transformComponent.transform.scale[i] = ScaleArray[i].GetFloat();

	//		}

	//		//EntityHandle entity = ecs->MakeEntity(renderComponent, transformComponent);

	//		currentScene.sceneObjects.Add(entity);
	//	}
	//}
	return true;
}
