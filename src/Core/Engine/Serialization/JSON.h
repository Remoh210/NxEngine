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
	* Deserialize scene from json file
	*/
	std::string scene_to_json(Scene scene);
	

	Scene parse_scene(ECS::World* World, NString sceneFile);
	
}
