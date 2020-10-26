#pragma once
#include <string>
#include "Core/ECS/ECS.h"
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
	static bool SaveScene(NString filename, ECS& ecs, class Camera& camera);
	//static bool LoadScene(std::string filename, ECS& ecs, class Camera& camera);
	//inline static sScene GetCurrentScene() { return currentScene; };
	//static void RemoveObjectFromScene(EntityHandle entity, ECS& ecs);;
	//void setBasePath(std::string basepath);


private:
	//static Scene ClearCurrentScene();
};
