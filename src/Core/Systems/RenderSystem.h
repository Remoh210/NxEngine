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
	RenderableMeshSystem(EditorRenderContext& contextIn) :BaseSystem(), context(contextIn)
	{
		addComponentType(TransformComponent::ID);
		addComponentType(RenderableMeshComponent::ID);
	}

	virtual void UpdateComponents(float delta, BaseComponent** components)
	{
		TransformComponent* transform = (TransformComponent*)components[0];
		RenderableMeshComponent* mesh = (RenderableMeshComponent*)components[1];

		transform->transform.rotation.y += 0.01f;

		context.RenderMesh(*mesh->vertexArray, *mesh->texture, transform->transform.ToMatrix());
	}
private:
	EditorRenderContext& context;
};


