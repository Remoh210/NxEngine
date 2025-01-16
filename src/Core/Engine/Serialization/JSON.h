#pragma once
#include <string>
#include <rttr/type>

struct Scene;

namespace Nx
{
	/*!
	 * Serialize the given instance to a json encoded string.
	 */
	std::string to_json(rttr::instance obj);

	/*!
	* Deserialize the given json string \p json to the given instance \p obj.
	*/
	bool from_json(const std::string& json, rttr::instance obj);

	/*!
	* Serialize the given instance to a json encoded string.
	*/
	std::string scene_to_json(Scene scene);

	Scene parse_scene(ECS::World* World, NString sceneFile);

	// Scene CreateSceneFromFile(ECS::World* world, const NString name);
	

	
	
}
