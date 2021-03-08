#pragma once

#include "Core/ECS/ECS.h"
#include "Core/Components/Input/InputComponent.h"
#include "Common/Input/Input.h"
#include "Common/dataStructs/String.h"
#include "rendering/Window.h"

namespace ECS
{
	class InputSystem : public EntitySystem
	{
	public:

		InputSystem(Window* windowIn)
			:window(windowIn)
		{

		}

		void tick(class World *world, float deltaTime)
		{
			world->each<InputComponent>([&](Entity *ent, ComponentHandle<InputComponent> inputComponent) -> void
			{
				inputComponent->UpdateKeyPressed(GetKey());
				inputComponent->UpdateAxis(window->GetHorizontalInputAxis(), window->GetVerticalInputAxis());
			});
		}

		InputKey GetKey();




	private:
		Window* window;
	};

}