#pragma once

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
	void AddObject(NString name, ECS::Entity* entity)
	{
		sceneObjects.push_back(new SceneObject(name, entity));
	};

	inline uint32 GetNumObjects() { return sceneObjects.size(); }
	
	void Clear();
	NxArray<SceneObject*> sceneObjects;
	
	
};

class SceneManager
{
public:

	//SceneManager();
	static Scene currentScene;
	static bool SaveScene(NString filename, class Camera& camera);
	static bool LoadScene(NString filename, class Camera& camera);
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

private:
	static ECS::World* world;
	static RenderDevice* renderDevice;

	static NxMap<NString, Shader*> shaders;
};
