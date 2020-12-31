#pragma once

#include "Core/ECS/ECS.h"
//#include "utilComponents.hpp"
#include "Core/Components/LightComponent/LightComponent.h"
#include "Editor/EditorRenderContext.h"
#include "Core/Components/TransformComponent/TransformComponent.h"
#include "Core/Graphics/Material/Material.h"
#include "Core/Components/StaticMeshComponent/StaticMeshComponent.h"

namespace ECS
{

	class RenderableMeshSystem : public EntitySystem
	{
	public:

		RenderableMeshSystem(EditorRenderContext* contextIn) 
			: EntitySystem(),
			context(contextIn)

		{
		}
		
		virtual void tick(class World *world, float deltaTime)
		{
			world->each<TransformComponent, LightComponent>([&](Entity *ent, ComponentHandle<TransformComponent> transform, ComponentHandle<LightComponent> lightComp) -> void 
			{
				context->RenderLight(lightComp, transform->transform.position.ToVec());
			});

			world->each<TransformComponent, StaticMeshComponent>([&](Entity *ent, ComponentHandle<TransformComponent> transform, ComponentHandle<StaticMeshComponent> mesh) -> void {

				if (!mesh->bIsVisible) { return; }

				Transform newTrasform = transform->transform;

				int num = mesh->numInst;

				if (num > 1 && first)
				{
					srand(glfwGetTime()); // initialize random seed
					float radius = 50.0;
					float offset = 10.5f;
					for (unsigned int i = 0; i < num; i++)
					{
						// 1. translation: displace along circle with 'radius' in range [-offset, offset]
						float angle = (float)i / (float)num * 360.0f;
						float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
						float x = sin(angle) * radius + displacement;
						displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
						float y = displacement * 0.4f; // keep height of field smaller compared to width of x and z
						displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
						float z = cos(angle) * radius + displacement;
						newTrasform.position = glm::vec3(x, y, z);

						// 2. scale: scale between 0.05 and 0.25f
						//float scale = (rand() % 20) / 100.0f + 0.08;
						//newTrasform.scale = glm::vec3(scale);

						// 3. rotation: add random rotation around a (semi)randomly picked rotation axis vector
						float rotAngle = (rand() % 360);
						newTrasform.rotation = glm::vec3(0.4f, 0.6f, 0.8f);
						transformNxArray.push_back(newTrasform);
					}
					first = false;
				}
				else if (num > 1 && !first)
				{
					for (auto item : transformNxArray)
					{

						context->RenderMesh(mesh->meshes, mesh->shader, item.ToMatrix());
						std::make_pair<int, float>(10, 10.0f);
					}
				}
				else
				{
					float rotSpeed = transform->rotSpeed;
					if (rotSpeed != 0.0f)
					{
						transform->transform.rotation.ToVec().y += rotSpeed * deltaTime;
					}
					//transform->transform.rotation.ToVec().y += 0.2f * deltaTime;
					//transform->transform.position = vec3(0.0f);

					context->RenderMesh(mesh->meshes, mesh->shader, transform->transform.ToMatrix());
				}
			});
		}

		virtual ~RenderableMeshSystem() {}

	private:
		bool first = true;
		NxArray<Transform> transformNxArray;
		EditorRenderContext* context;
	};
} // namespace ECS