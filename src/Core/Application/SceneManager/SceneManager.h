#pragma once

#include "Common/dataStructs/String.h"
#include "Common/CommonTypes.h"
#include "Core/ECS/ECS.h"
#include "rendering/RenderDevice.h"
#include "rendering/Shader.h"




struct Scene
{
	NString sceneName;
	Array<EntityHandle> sceneObjects;
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

	inline static void SetECS(ECS& ecsIn)
	{
		ecs = &ecsIn;
	}

	inline static void SetRenderDevice(RenderDevice& renderDeviceIn)
	{
		renderDevice = &renderDeviceIn;
	}

private:
	static ECS* ecs;
	static RenderDevice* renderDevice;

	static Map<NString, Shader*> shaders;
};