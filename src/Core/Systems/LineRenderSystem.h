#include "Core/ECS/ECS.h"
#include "Core/ECS/Component.h"
//#include "utilComponents.hpp"
#include <Editor/EditorRenderContext.h>
#include <Core/Components/RenderComponent.h>
#include <Core/Components/TransformComponent.h>


struct LineRenderComponent : public Component<LineRenderComponent>
{
	VertexArray* vertexArray = nullptr;
	uint32 numVertecies  = 0;
	Shader* shader;
	Texture* texture;
};


class LineRenderSystem : public BaseSystem
{
public:
	LineRenderSystem(EditorRenderContext& contextIn, ECS& ecsIn)
		:BaseSystem()
		, context(contextIn)
		, ecs(ecsIn)
	{
		addComponentType(TransformComponent::ID);
		addComponentType(LineRenderComponent::ID);
	}

	virtual void UpdateComponents(float delta, BaseComponent** components)
	{
		TransformComponent* transform = (TransformComponent*)components[0];
	
		
		LineRenderComponent* mesh = ecs.GetComponent<LineRenderComponent>(transform->entity);

		
		//transform->transform.rotation.y += 0.01f;
		////transform->transform.position = vec3(0.0f);
		//Array<mat4> transforms;
		//transforms.push_back(transform->transform.ToMatrix());

		//mesh->vertexArray->UpdateBuffer(4, &transforms[0], sizeof(mat4));
		context.RenderPrimitives(mesh->vertexArray, mesh->shader, mesh->texture, 3, transform->transform.ToMatrix());
		

		
	}
private:
	bool fist = 1;
	EditorRenderContext& context;
	Array<Transform> transformArray;
	ECS& ecs;
};