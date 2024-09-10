#pragma once

#include <cereal/types/memory.hpp>    // for std::shared_ptr
#include <cereal/types/vector.hpp>    // for std::vector

#include "GameObject.h"
#include "Common/dataStructs/String.h"
#include "Common/CommonTypes.h"
#include "Core/ECS/ECS.h"
#include "rendering/RenderDevice.h"
#include "rendering/Shader.h"

struct SceneObject
{
	SceneObject(NString nameIn, ECS::Entity* entityIn)
		:objectName(nameIn)
		,entity(entityIn)
	{}

	NString objectName;
	ECS::Entity* entity;
};

struct Scene
{
	NString sceneName;
	void MakeNewObject(NString name, ECS::Entity* entity)
	{
		gameObjects.push_back(std::make_shared<GameObject>(name, entity));
		//sceneObjects.push_back(new GameObject(name, entity));
	}

	void AddObject(GameObject* GameObject)
	{
		if(nullptr == GameObject)
		{
			return;
		}
		
		sceneObjects.push_back(GameObject);
	}

	inline uint32 GetNumObjects() { return sceneObjects.size(); }
	
	void Clear();

	template <class Archive>
	void serialize(Archive & ar)
	{
		ar(CEREAL_NVP(gameObjects));
	}
	
	NxArray<GameObject*> sceneObjects;
	NxArray<std::shared_ptr<GameObject>> gameObjects;
};

class SceneManager
{
public:

	//SceneManager();
	static Scene currentScene;
	static bool SaveScene(NString filename);
	static bool LoadScene(NString filename);
	//inline static sScene GetCurrentScene() { return currentScene; };
	static void ClearScene();

	inline static void SetECS(ECS::World* worldIn)
	{
		world = worldIn;
	}

	inline static void SetRenderDevice(RenderDevice* renderDeviceIn)
	{
		renderDevice = renderDeviceIn;
	}

	inline static void SetCamera(Camera* inEditorCamera)
	{
		editorCamera = inEditorCamera;
	}

private:
	static ECS::World* world;
	static RenderDevice* renderDevice;
	static Camera* editorCamera;

	static NxMap<NString, Shader*> shaders;
};
