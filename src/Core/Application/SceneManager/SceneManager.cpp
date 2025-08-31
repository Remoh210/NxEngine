#include "SceneManager.h"

#include "Common/Transform/Transform.h"

#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/filewritestream.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>
#include <cereal/archives/json.hpp>

#include <rapidjson/ostreamwrapper.h>
#include <fstream>
//
#include "../../../../external/assimp/code/Blender/BlenderScene.h"
#include "Core/Systems/RenderSystem.h"
#include "Core/Components/StaticMeshComponent/StaticMeshComponent.h"
#include "Core/Camera/Camera.h"
#include "Core/FileSystem/FileSystem.h"
#include "Core/Application/AssetManager/AssetManager.h"
#include "Core/Graphics/ShaderManager/ShaderManager.h"
#include "Core/Engine/Serialization/JSON.h"



Scene 					SceneManager::currentScene;
RenderDevice*			SceneManager::renderDevice = nullptr;
NxMap<NString, Shader*> SceneManager::shaders;
ECS::World*				SceneManager::world = nullptr;
Camera* 				SceneManager::editorCamera = nullptr;


class BaseClass
{
public:
	BaseClass()
	{
		//BaseClassProperty = Prop;
	}
	//int BaseClassProperty = 0;

	//RTTR_ENABLE()
};

class DerivedClass : public BaseClass
{
public:
	DerivedClass(int Prop = 1)
	{
		DerivedClassProperty = Prop;
	}
	
	int DerivedClassProperty = 1;

	RTTR_ENABLE()
};

RTTR_REGISTRATION
{
	rttr::registration::class_<DerivedClass>("DerivedClass")
		.property("DerivedClassProperty", &DerivedClass::DerivedClassProperty)
	;
}


void Scene::Clear()
{
	for (int i = 0; i < sceneObjects.size(); i++)
	{
		delete sceneObjects[i];
	}
	sceneObjects.clear();

	// unique pointers will be reset
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
	 	DEBUG_LOG_TEMP("About to delete: %s", currentScene.sceneObjects[i]->name.c_str());
	 	world->destroy(currentScene.sceneObjects[i]->entity);
	 }

	currentScene.Clear();

}


bool SceneManager::SaveScene(NString filename)
{
	if(!world)
	{
		return false;
	}
	
	const NString fileToLoadFullPath = Nx::FileSystem::GetRoot() + "/res/Scenes/" + filename + ".json";
	
	std::ofstream outputFile;
	outputFile.open(fileToLoadFullPath.c_str());
	
	outputFile << Nx::scene_to_json(currentScene);
	
	outputFile.close();
	
	
	// for (GameObject* gameObject : currentScene.sceneObjects)
	// {
	// 	NString GameObjectAsJson = Nx::to_json(gameObject);
	// 	outputFile << GameObjectAsJson;
	// 	DEBUG_LOG_TEMP("testJSON: %s", GameObjectAsJson.c_str());
	//
	// 	
	// 	ECS::ComponentHandle<TransformComponent> transformComp = gameObject->entity->get<TransformComponent>();
	//
	// 	if(!transformComp)
	// 	{
	// 		continue;
	// 	}
	// 	
	// 	NxArray<ECS::ComponentHandle<BaseComponent>> components = gameObject->entity->getAllComponents<BaseComponent>();
	// 	for (ECS::ComponentHandle<BaseComponent>& component : components)
	// 	{
	// 		if(!component)
	// 		{
	// 			continue;
	// 		}
	//
	// 		NString ComponentAsJsonStr = Nx::to_json(component.get());
	// 		outputFile << ComponentAsJsonStr;
	// 		DEBUG_LOG_TEMP("testJSON: %s", ComponentAsJsonStr.c_str());
	// 	}
	// }

	return true;
}

bool SceneManager::LoadScene(NString filename) 
{
	if(!world)
	{
		return false;
	}
	
	const NString fileToLoadFullPath = Nx::FileSystem::GetRoot() + "/res/Scenes/" + filename + ".json";
	
	
	currentScene = Nx::parse_scene(world, fileToLoadFullPath);
	

	return true;
}
