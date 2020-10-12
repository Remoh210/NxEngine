//#ifndef _cSceneManager_HG_
//#define _cSceneManager_HG_
//#include <string>
//#include <Core/ECS/ECS.h>
//
//struct sScene
//{
//	string sceneName;
//	Array<EntityHandle> sceneObjects;
//};
//
//class cSceneManager
//{
//public:
//
//	cSceneManager();
//	static sScene currentScene;
//	static bool SaveScene(std::string filename, ECS& ecs, class Camera& camera);
//	static bool LoadScene(std::string filename, ECS& ecs, class Camera& camera);
//	//inline static sScene GetCurrentScene() { return currentScene; };
//	static void RemoveObjectFromScene(EntityHandle entity, ECS& ecs);
//	//bool loadScene(std::string filename);
//	void setBasePath(std::string basepath);
//
//
//private:
//	static sScene ClearCurrentScene();
//	
//
//};
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//#endif