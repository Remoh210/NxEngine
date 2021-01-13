#pragma once

#include "Core/ECS/ECS.h"
#include "Core/Components/TransformComponent/TransformComponent.h"
#include "Core/Components/SkinnedMeshComponent/SkinnedMeshComponent.h"
#include "Core/Components/AnimatorComponent/AnimatorComponent.h"

namespace ECS
{
	class AnimatorSystem : EntitySystem
	{
		AnimatorSystem()
		{

		}

		virtual void tick(class World *world, float deltaTime)
		{
			world->each<TransformComponent, SkinnedMeshComponent, AnimatorComponent>([&](Entity *ent, ComponentHandle<TransformComponent> transform
				, ComponentHandle<SkinnedMeshComponent> skinnedMesh, AnimatorComponent animatorComponent) -> void
			{
				//context->RenderSkinnedMesh();
			});
		}
	};
}