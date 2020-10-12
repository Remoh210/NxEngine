#pragma once
#include "Common/Common.h"
#include "Common/dataStructs/Array.h"
#include "Common/dataStructs/Map.h"
#include "System.h"

class ECSListener
{
public:
	virtual void OnMakeEntity(EntityHandle handle) {}
	virtual void OnRemoveEntity(EntityHandle handle) {}
	virtual void OnAddComponent(EntityHandle handle, uint32 id) {}
	virtual void OnRemoveComponent(EntityHandle handle, uint32 id) {}

	const Array<uint32>& GetComponentIDs() {
		return componentIDs;
	}

	inline bool ShouldNotifyOnAllComponentOperations() {
		return notifyOnAllComponentOperations;
	}
	inline bool ShouldNotifyOnAllEntityOperations() {
		return notifyOnAllEntityOperations;
	}

protected:
	void SetNotificationSettings(
		bool shouldNotifyOnAllComponentOperations,
		bool shouldNotifyOnAllEntityOperations) {
		notifyOnAllComponentOperations = shouldNotifyOnAllComponentOperations;
		notifyOnAllEntityOperations = shouldNotifyOnAllEntityOperations;
	}
	void addComponentID(uint32 id) {
		componentIDs.push_back(id);
	}
private:
	Array<uint32> componentIDs;
	bool notifyOnAllComponentOperations = false;
	bool notifyOnAllEntityOperations = false;
};

class ECS
{
public:
	ECS() {};
	~ECS();

	//Entities
	EntityHandle MakeEntity(BaseComponent** entityComponents, const uint32* componentIDs, size_t numComponents);
	void RemoveEntity(EntityHandle handle);


	template<class A>
	EntityHandle MakeEntity(A& c1)
	{
		BaseComponent* components[] = { &c1 };
		uint32 componentIDs[] = { A::ID};
		return MakeEntity(components, componentIDs, 1);
	}

	template<class A, class B>
	EntityHandle MakeEntity(A& c1, B& c2)
	{
		BaseComponent* components[] = { &c1, &c2 };
		uint32 componentIDs[] = { A::ID, B::ID};
		return MakeEntity(components, componentIDs, 2);
	}

	template<class A, class B, class C>
	EntityHandle MakeEntity(A& c1, B& c2, C& c3)
	{
		BaseComponent* components[] = { &c1, &c2, &c3 };
		uint32 componentIDs[] = { A::ID, B::ID, C::ID };
		return MakeEntity(components, componentIDs, 3);
	}

	//Components
	inline Array<EntityHandle> GetEntities()
	{
		Array<EntityHandle> HadlesArray;
		for (int i = 0; i < entities.size(); i++)
		{
			HadlesArray.push_back((EntityHandle)entities[i]);
		}
		return HadlesArray;
	}

	//template<class A, class B, class C, class D>
	//EntityHandle MakeEntity(A&, B&, C&, D&)
	//{
	//	BaseComponent* components[] = { A, B, C, D };
	//	uint32 componentIDs[] = { A::ID, B::ID, C::ID, D::ID};
	//	return MakeEntity(components, componentIDs, 4);
	//}

	//template<class A, class B, class C, class D, class E>
	//EntityHandle MakeEntity(A&, B&, C&, D&, E&)
	//{
	//	BaseComponent* components[] = { A, B, C, D, E };
	//	uint32 componentIDs[] = { A::ID, B::ID, C::ID, D::ID, D::ID };
	//	return MakeEntity(components, componentIDs, 5);
	//}

	// Component methods
	template<class Component>
	inline void addComponent(EntityHandle entity, Component* component)
	{
		AddComponentInternal(entity, handleToEntity(entity), Component::ID, component);
		for (uint32 i = 0; i < listeners.size(); i++) {
			const Array<uint32>& componentIDs = listeners[i]->GetComponentIDs();
			if (listeners[i]->ShouldNotifyOnAllComponentOperations()) {
				listeners[i]->OnAddComponent(entity, Component::ID);
			}
			else {
				for (uint32 j = 0; j < componentIDs.size(); j++) {
					if (componentIDs[j] == Component::ID) {
						listeners[i]->OnAddComponent(entity, Component::ID);
						break;
					}
				}
			}
		}
	}

	template<class Component>
	bool RemoveComponent(EntityHandle entity)
	{
		for (uint32 i = 0; i < listeners.size(); i++) {
			const Array<uint32>& componentIDs = listeners[i]->GetComponentIDs();
			for (uint32 j = 0; j < componentIDs.size(); j++) {
				if (listeners[i]->ShouldNotifyOnAllComponentOperations()) {
					listeners[i]->OnRemoveComponent(entity, Component::ID);
				}
				else {
					if (componentIDs[j] == Component::ID) {
						listeners[i]->OnRemoveComponent(entity, Component::ID);
						break;
					}
				}
			}
		}
		return RemoveComponentInternal(entity, Component::ID);
	}

	template<class Component>
	inline Component* GetComponent(EntityHandle entity)
	{
		return (Component*)GetComponentInternal(handleToEntity(entity), components[Component::ID], Component::ID);
	}

	void UpdateSystems(SystemList& systems, float DeltaTime);



private:
	Map<uint32, Array<uint8>> components;
	Array<std::pair<uint32, Array<std::pair<uint32, uint32>>>*> entities;
	Array<ECSListener*> listeners;

	inline std::pair<uint32, Array<std::pair<uint32, uint32>>>* handleToRawType(EntityHandle handle)
	{
		return (std::pair<uint32, Array<std::pair<uint32, uint32>>>*)handle;
	}

	inline Array <std::pair<uint32, uint32>>& handleToEntity(EntityHandle handle)
	{
		return handleToRawType(handle)->second;
	}

	inline uint32 handleToEntityIndex(EntityHandle handle)
	{
		return handleToRawType(handle)->first;
	}

	void AddComponentInternal(EntityHandle handle, Array<std::pair<uint32, uint32>>& entity, uint32 componentID, BaseComponent* component);
	BaseComponent* GetComponentInternal(Array<std::pair<uint32, uint32>>& entityComponents, Array<uint8>& array, uint32 componentID);
	bool RemoveComponentInternal(EntityHandle handle, uint32 componentID);
	void DeleteComponent(uint32 componentID, uint32 index);

	void updateSystemWithMultComponents(uint32 index, SystemList systems, float DeltaTime, const Array<uint32>& componentTypes, Array<BaseComponent*>& componentParam, Array<Array<uint8>*>& componentArrays);

	uint32 findLeastCommonComponent(const Array<uint32>& componentTypes, const Array<uint32>& componentFlags);
	//NULL_COPY_AND_ASSIGN(ECS);
};
