#pragma once
#include <tuple>

#include "Common/Common.h"
#include "Common/dataStructs/Array.h"


struct BaseComponent;

#define NULL_ENTITY_HANDLE nullptr;

typedef void* EntityHandle;
//Array<uint8>& memory, EntityHandle entity, BaseECSComponent* comp
typedef uint32 (*CreateComponentFunction)(Array<uint8>& memory, EntityHandle entity, BaseComponent* comp);
typedef void (*FreeComponentFunction)(BaseComponent* comp);

struct BaseComponent
{
	static uint32 RegisterComponentType(CreateComponentFunction createfn, 
		FreeComponentFunction freefn, size_t size);
	EntityHandle entity = NULL_ENTITY_HANDLE;

	inline static CreateComponentFunction GetTypeCreateFunction(uint32 id)
	{
		return std::get<0>((*componentsTypes)[id]);
	}

	inline static FreeComponentFunction GetTypeFreeFunction(uint32 id)
	{
		return std::get<1>((*componentsTypes)[id]);
	}

	inline static size_t GetTypeSize(uint32 id)
	{
		return std::get<2>((*componentsTypes)[id]);
	}

	inline static bool IsTypeValid(uint32 id)
	{
		return id < componentsTypes->size();
	}
private:
	static Array<std::tuple<CreateComponentFunction, FreeComponentFunction, size_t>>* componentsTypes;

};

template<typename T>
struct Component : public BaseComponent
{
	static const CreateComponentFunction CREATE_FUNCTION;
	static const FreeComponentFunction FREE_FUNCTION;
	static const uint32 ID;
	static const size_t SIZE;
};

template<typename Component>
uint32 CreateComponent(Array<uint8>& memory, EntityHandle entity, BaseComponent* comp)
{
	uint32 index = memory.size();
	memory.resize(index + Component::SIZE);
	Component* component = new(&memory[index])Component(*(Component*)comp);
	component->entity = entity;
	return index;
}

template<typename Component>
void FreeComponent(BaseComponent* comp)
{
	Component* component = (Component*)comp;
	component->~Component();
}

template<typename T>
const uint32 Component<T>::ID(BaseComponent::RegisterComponentType(CreateComponent<T>, FreeComponent<T>, sizeof(T)));

template<typename T>
const size_t Component<T>::SIZE(sizeof(T));

template<typename T>
const CreateComponentFunction Component<T>::CREATE_FUNCTION(CreateComponent<T>);

template<typename T>
const FreeComponentFunction Component<T>::FREE_FUNCTION(FreeComponent<T>);
