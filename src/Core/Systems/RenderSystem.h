#pragma once

#include "Core/ECS/ECS.h"
#include "Core/ECS/Component.h"
//#include "utilComponents.hpp"
#include <Editor/EditorRenderContext.h>
#include <Core/Components/RenderComponent.h>
#include <Core/Components/TransformComponent.h>



struct RenderableMeshComponent : public Component<RenderableMeshComponent>
{
	VertexArray* vertexArray = nullptr;
	Texture* texture = nullptr;
	int numInst = 0;
};

class RenderableMeshSystem : public BaseSystem
{
public:
	RenderableMeshSystem(EditorRenderContext& contextIn, ECS& ecsIn) 
		:BaseSystem()
		, context(contextIn)
		, ecs(ecsIn)
	{
		addComponentType(TransformComponent::ID);
		addComponentType(RenderableMeshComponent::ID);
	}

	virtual void UpdateComponents(float delta, BaseComponent** components)
	{
		TransformComponent* transform = (TransformComponent*)components[0];
	
		
		RenderableMeshComponent* mesh = ecs.GetComponent<RenderableMeshComponent>(transform->entity);

		Transform newTrasform;

		int num = mesh->numInst;

		
		if (num > 1 && fist)
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
				float scale = (rand() % 20) / 100.0f + 0.08;
				newTrasform.scale = glm::vec3(scale);

				// 3. rotation: add random rotation around a (semi)randomly picked rotation axis vector
				float rotAngle = (rand() % 360);
				newTrasform.rotation = glm::vec3(0.4f, 0.6f, 0.8f);
				transformArray.push_back(newTrasform);
				
			}
			fist = false;
		}
		else if (num > 1 && !fist)
		{
			for (auto item : transformArray)
			{
				context.RenderMesh(*mesh->vertexArray, *mesh->texture, item.ToMatrix());
			}
		}
		else
		{
			transform->transform.rotation.y += 0.01f;
			transform->transform.position = vec3(0.0f);


			context.RenderMesh(*mesh->vertexArray, *mesh->texture, transform->transform.ToMatrix());


			//newTrasform = transform->transform;
			//for (int i = 0; i < 1000; i++)
			//{
			//	
			//	newTrasform.position.x += 1.1f;
			//	context.RenderMesh(*mesh->vertexArray, *mesh->texture, newTrasform.ToMatrix());
			//}
		}

		
	}
private:
	bool fist = 1;
	EditorRenderContext& context;
	Array<Transform> transformArray;
	ECS& ecs;
};


