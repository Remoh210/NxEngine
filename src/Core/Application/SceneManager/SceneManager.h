#pragma once
#include <string>
#include "Core/ECS/ECS.h"
#include "rendering/RenderDevice.h"
#include "Common/dataStructs/String.h"

struct Scene
{
	NString sceneName;
	Array<EntityHandle> sceneObjects;
};

class SceneManager
{
public:

	//SceneManager();
	static Scene currentScene;
	static bool SaveScene(NString filename, class Camera& camera);
	static bool LoadScene(NString filename, class Camera& camera);
	//inline static sScene GetCurrentScene() { return currentScene; };
	static void RemoveObjectFromScene(EntityHandle entity);

	static void SetECS(ECS& ecsIn)
	{
		ecs = &ecsIn;
	}
	inline static void SetRenderDevice(ECS& ecsIn)
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
};
