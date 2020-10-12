#include "Component.h"


Array<std::tuple<CreateComponentFunction, FreeComponentFunction, size_t>>* BaseComponent::componentsTypes;

uint32 BaseComponent::RegisterComponentType(CreateComponentFunction createfn,
	FreeComponentFunction freefn, size_t size)
{
	if(!componentsTypes)
	{
		componentsTypes = new Array<std::tuple<CreateComponentFunction, FreeComponentFunction, size_t>>();
	}
	uint32 componentID = componentsTypes->size();
	componentsTypes->push_back(std::tuple<CreateComponentFunction, FreeComponentFunction, size_t>(createfn, freefn, size));
	return componentID++;
}

