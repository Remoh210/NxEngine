#include "ECS.h"
#include <algorithm>
#include <cstring>

ECS::~ECS()
{

	for (Map<uint32, Array<uint8>>::iterator it = components.begin(); it != components.end(); ++it)
	{
		size_t TypeSize = BaseComponent::GetTypeSize(it->first);
		FreeComponentFunction freefn = BaseComponent::GetTypeFreeFunction(it->first);

		for (uint32 i = 0; i < it->second.size(); i += TypeSize)
		{
			freefn((BaseComponent*)&it->second[i]);
		}
	}

	for (uint32 i = 0; i < entities.size(); i++)
	{
		delete entities[i];
	}

}

EntityHandle ECS::MakeEntity(BaseComponent** entityComponents, const uint32* componentIDs, size_t numComponents)
{
	
	std::pair<uint32, Array<std::pair<uint32, uint32> > >* newEntity = new std::pair<uint32, Array<std::pair<uint32, uint32> > >();
	EntityHandle handle = (EntityHandle)newEntity;
	for (uint32 i = 0; i < numComponents; i++) {
		if (!BaseComponent::IsTypeValid(componentIDs[i])) {
			DEBUG_LOG("ECS", LOG_ERROR, "'%u' is not a valid component type.", componentIDs[i]);
			delete newEntity;
			return NULL_ENTITY_HANDLE;
		}

		AddComponentInternal(handle, newEntity->second, componentIDs[i], entityComponents[i]);
	}

	newEntity->first = entities.size();
	entities.push_back(newEntity);

	for (uint32 i = 0; i < listeners.size(); i++) {
		bool isValid = true;
		if (listeners[i]->ShouldNotifyOnAllEntityOperations()) {
			listeners[i]->OnMakeEntity(handle);
		}
		else {
			for (uint32 j = 0; j < listeners[i]->GetComponentIDs().size(); j++) {
				bool hasComponent = false;
				for (uint32 k = 0; k < numComponents; k++) {
					if (listeners[i]->GetComponentIDs()[j] == componentIDs[k]) {
						hasComponent = true;
						break;
					}
				}
				if (!hasComponent) {
					isValid = false;
					break;
				}
			}
			if (isValid) {
				listeners[i]->OnMakeEntity(handle);
			}
		}
	}

	return handle;
}

void ECS::RemoveEntity(EntityHandle handle)
{
	Array<std::pair<uint32, uint32> >& entity = handleToEntity(handle);

	for (uint32 i = 0; i < listeners.size(); i++) {
		const Array<uint32>& componentIDs = listeners[i]->GetComponentIDs();
		bool isValid = true;
		if (listeners[i]->ShouldNotifyOnAllEntityOperations()) {
			listeners[i]->OnRemoveEntity(handle);
		}
		else {
			for (uint32 j = 0; j < componentIDs.size(); j++) {
				bool hasComponent = false;
				for (uint32 k = 0; k < entity.size(); k++) {
					if (componentIDs[j] == entity[k].first) {
						hasComponent = true;
						break;
					}
				}
				if (!hasComponent) {
					isValid = false;
					break;
				}
			}
			if (isValid) {
				listeners[i]->OnRemoveEntity(handle);
			}
		}
	}

	for (uint32 i = 0; i < entity.size(); i++) {
		DeleteComponent(entity[i].first, entity[i].second);
	}

	uint32 destIndex = handleToEntityIndex(handle);
	uint32 srcIndex = entities.size() - 1;
	delete entities[destIndex];
	entities[destIndex] = entities[srcIndex];
	entities[destIndex]->first = destIndex;
	entities.pop_back();
}

void ECS::UpdateSystems(SystemList& systems, float DeltaTime)
{
	Array<BaseComponent*> componentParam;
	Array<Array<uint8>*> componentArrays;
	for (uint32 i = 0; i < systems.size(); i++)
	{
		const Array<uint32>& componentTypes = systems[i]->GetComponentTypes();
		if (componentTypes.size() == 1)
		{
			size_t typeSize = BaseComponent::GetTypeSize(componentTypes[0]);
			Array<uint8>& array = components[componentTypes[0]];
			for (uint32 j = 0; j < array.size(); j += typeSize)
			{
				BaseComponent* component = (BaseComponent*)&array[j];
				systems[i]->UpdateComponents(DeltaTime, &component);
			}
		}
		else
		{
			updateSystemWithMultComponents(i, systems, DeltaTime, componentTypes, componentParam, componentArrays);
		}
	}
}

void ECS::updateSystemWithMultComponents(uint32 index, SystemList systems, float DeltaTime, const Array<uint32>& componentTypes, Array<BaseComponent*>& componentParam, Array<Array<uint8>*>& componentArrays)
{
	const Array<uint32>& componentFlags = systems[index]->GetComponentFlags();

	componentParam.resize(std::max(componentParam.size(), componentTypes.size()));
	componentArrays.resize(std::max(componentParam.size(), componentTypes.size()));
	for (uint32 i = 0; i < componentTypes.size(); i ++)
	{
		componentArrays[i] = &components[componentTypes[i]];
	}

	uint32 minSizeIndex = findLeastCommonComponent(componentTypes, componentFlags);

	size_t typeSize = BaseComponent::GetTypeSize(componentTypes[minSizeIndex]);
	Array<uint8>& array = *componentArrays[minSizeIndex];
	for (uint32 i = 0; i < array.size(); i += typeSize)
	{
		BaseComponent* component = (BaseComponent*)&array[i];
		componentParam[minSizeIndex] = component;
		Array <std::pair<uint32, uint32>>& entityComponents = handleToEntity(componentParam[minSizeIndex]->entity);
		bool isValid = true;
		for (int j = 0; j < componentTypes.size(); j++)
		{
			if (j == minSizeIndex)
			{
				continue;
			}

			componentParam[j] = GetComponentInternal(entityComponents, *componentArrays[j],  componentTypes[j]);
			if (!componentParam[j] && (componentFlags[j] & BaseSystem::FLAG_OPTIONAL) == 0)
			{
				isValid = false;
				break;
			}

		}

		if (isValid)
		{
			systems[index]->UpdateComponents(DeltaTime, &componentParam[0]);
		}

	}
}

uint32 ECS::findLeastCommonComponent(const Array<uint32>& componentTypes, const Array<uint32>& componentFlags)
{
	uint32 minSize = (uint32)-1;
	uint32 minIndex = (uint32)-1;
	for (uint32 i = 0; i < componentTypes.size(); i++)
	{
		if ((componentFlags[i] & BaseSystem::FLAG_OPTIONAL) != 0)
		{
			continue;
		}
		size_t typeSize = BaseComponent::GetTypeSize(componentTypes[i]);
		uint32 size = components[componentTypes[i]].size()/typeSize;
		if (size <= minSize)
		{
			minSize = size;
			minIndex = i;
		}
	}
	return minIndex;
}

void ECS::AddComponentInternal(EntityHandle handle, Array<std::pair<uint32, uint32>>& entity, uint32 componentID, BaseComponent * component)
{
	CreateComponentFunction createfn = BaseComponent::GetTypeCreateFunction(componentID);
	std::pair<uint32, uint32> NewPair;
	NewPair.first = componentID;
	NewPair.second = createfn(components[componentID], handle, component);
	entity.push_back(NewPair);
}

BaseComponent* ECS::GetComponentInternal(Array<std::pair<uint32, uint32>>& entityComponents, Array<uint8>& array, uint32 componentID)
{
	for (uint32 i = 0; i < entityComponents.size(); i++)
	{
		if (componentID == entityComponents[i].first)
		{
			return (BaseComponent*)&components[componentID][entityComponents[i].second];
		}
	}
	return nullptr;
}

bool ECS::RemoveComponentInternal(EntityHandle handle, uint32 componentID)
{
	Array<std::pair<uint32, uint32>>& entityComponents = handleToEntity(handle);
	for (uint32 i = 0; i < entityComponents.size(); i++)
	{
		if (componentID == entityComponents[i].first)
		{
			DeleteComponent(entityComponents[i].first, entityComponents[i].second);
			uint32 srcIndex = entityComponents.size() - 1;
			uint32 destIndex = i;
			entityComponents[destIndex] = entityComponents[srcIndex];
			entityComponents.pop_back();
			return true;
		}
	}
	return false;
}

void ECS::DeleteComponent(uint32 componentID, uint32 index)
{
	Array<uint8>& array = components[componentID];
	FreeComponentFunction freefn = BaseComponent::GetTypeFreeFunction(componentID);
	size_t typeSize = BaseComponent::GetTypeSize(componentID);
	uint32 srcIndex = array.size() - typeSize;

	BaseComponent* destComponent = (BaseComponent*)&array[index];
	BaseComponent* srcComponent = (BaseComponent*)&array[srcIndex];
	freefn(destComponent);

	if (index == srcIndex) {
		array.resize(srcIndex);
		return;
	}
	memcpy(destComponent, srcComponent, typeSize);

	Array<std::pair<uint32, uint32> >& srcComponents = handleToEntity(srcComponent->entity);
	for (uint32 i = 0; i < srcComponents.size(); i++) {
		if (componentID == srcComponents[i].first && srcIndex == srcComponents[i].second) {
			srcComponents[i].second = index;
			break;
		}
	}

	array.resize(srcIndex);
	
}

