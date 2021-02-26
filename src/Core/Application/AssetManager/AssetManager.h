#include "Common/CommonTypes.h"
#include "Common/dataStructs/NxArray.h"
#include "Common/dataStructs/Map.h"
#include "Common/dataStructs/String.h"
#include "Core/Graphics/Mesh/MeshInfo.h"
#include "Core/Graphics/SkinnedMesh/SkinnedMeshInfo.h"
#include "Core/Graphics/Animation/AnimationInfo.h"
#include "Core/Systems/Physics/PhysicsSystem.h"
#include "Core/Physcis/iRigidBody.h"

#include "rendering/IndexedModel.h"
#include "rendering/Material.h"

class AssetManager 
{
public:
    AssetManager() {};

	inline static void SetRenderDevice(RenderDevice* renderDeviceIn)
	{
		renderDevice = renderDeviceIn;
	}
	inline static void SetPhysicsSystem(ECS::PhysicsSystem* physSystemIn)
	{
		physicsSystem = physSystemIn;
	}

	static NxArray<MeshInfo*> ImportModel(NString file);
	static bool ImportModelGenerateCollider(NString file, NxArray<MeshInfo*>& meshArrayOut, nPhysics::eShapeType shapeType, nPhysics::iShape*& shapeOut);
	static SkinnedMeshInfo* ImportModelSkeletal(NString file);
	static AnimationInfo* ImportAnimation(NString file, NString name);
	static Texture* ImportTexture(RenderDevice* renderDevice, NString textureFileName, PixelFormat format);

	static nPhysics::iShape* GenerateCollisionShapeFromModels(NxArray<IndexedModel>& indexedModels);
	static nPhysics::iShape* GenerateCollisionShapeFromModel(IndexedModel& indexedModel);

	static nPhysics::iShape* GenerateCollisionConvexHullFromModels(NxArray<IndexedModel>& indexedModels);
	static nPhysics::iShape* GenerateCollisionConvexHullFromModel(IndexedModel& indexedModel);


private:
	static RenderDevice* renderDevice;
	static ECS::PhysicsSystem* physicsSystem;
	static Material* LoadMaterial(MaterialSpec& matSpec);

	static NxMap<NString, NxArray<MeshInfo*>> importedModels;
	static NxMap<NString, SkinnedMeshInfo*> importedSkinnedModels;
	static NxMap<NString, Texture*> importedTextures;
	static NxMap<NString, AnimationInfo*> importedAnimations;

	inline static Texture* FindTexture(NString textureFile)
	{
		auto imported_texture_it = importedTextures.find(textureFile);
		if (imported_texture_it != importedTextures.end())
		{
			return imported_texture_it->second;
		}
		else
		{
			return nullptr;
		}
	}
};