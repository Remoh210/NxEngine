#pragma once

#include "Core/ECS/ECS.h"
#include "Core/Components/Input/InputComponent.h"
#include "Common/Input/Input.h"
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
			});
		}

		InputKey GetKey()
		{
			if (glfwGetKey(window->GetWindowHandle(), GLFW_KEY_SPACE) == GLFW_PRESS)
			{
				return InputKey::KEY_SPACE;
			}
			if (glfwGetKey(window->GetWindowHandle(), GLFW_KEY_A) == GLFW_PRESS)
			{
				return InputKey::KEY_A;
			}
			if (glfwGetKey(window->GetWindowHandle(), GLFW_KEY_D) == GLFW_PRESS)
			{
				return InputKey::KEY_D;
			}
			else
			{
				return InputKey::KEY_NONE;
			}
		}


	private:
		Window* window;
	};

}