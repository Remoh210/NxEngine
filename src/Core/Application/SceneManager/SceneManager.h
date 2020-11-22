#pragma once

#include "Common/dataStructs/String.h"
#include "Common/CommonTypes.h"
#include "Core/ECS/ECS.h"
#include "rendering/RenderDevice.h"
#include "rendering/Shader.h"



struct Scene
{
	NString sceneName;
	Array<ECS::Entity*> sceneObjects;
	inline uint32 GetNumObjects() { return sceneObjects.size(); }
	void Clear();
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

	inline static void SetRenderDevice(RenderDevice& renderDeviceIn)
	{
		renderDevice = &renderDeviceIn;
	}

private:
	static ECS::World* world;
	static RenderDevice* renderDevice;

	static Map<NString, Shader*> shaders;
};
