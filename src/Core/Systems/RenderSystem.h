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

		transform->transform.rotation.y += 0.01f;



		context.RenderMesh(*mesh->vertexArray, *mesh->texture, transform->transform.ToMatrix());

		Transform newTrasform;
		newTrasform = transform->transform;
		for (int i = 0; i < 1000; i++)
		{
			
			newTrasform.position.x += 1.1f;
			context.RenderMesh(*mesh->vertexArray, *mesh->texture, newTrasform.ToMatrix());
		}
		
	}
private:
	EditorRenderContext& context;
	ECS& ecs;
};


