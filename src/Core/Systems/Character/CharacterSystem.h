#pragma once

#include "Core/ECS/ECS.h"
#include "Core/Camera/Camera.h"


namespace ECS
{
	class CharacterSystem : public EntitySystem
	{
	public:
		CharacterSystem(Camera* cameraIn)
			:camera(cameraIn)
		{}

		virtual void tick(class World *world, float deltaTime);

		void Test() {};

	private:
		Camera* camera;
	};
}