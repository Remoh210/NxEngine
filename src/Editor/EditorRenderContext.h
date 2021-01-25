#pragma once

#include "Core/ECS/ECS.h"
#include "rendering/RenderDevice.h"
#include "rendering/RenderContext.h"
#include "rendering/Shader.h"
#include "rendering/Sampler.h"
#include "rendering/VertexArray.h"
#include "Common/CommonTypes.h"
#include "Common/Transform/Transform.h"
#include "Common/dataStructs/Map.h"
#include "Core/Graphics/PrimitiveGenerator/PrimitiveGenerator.h"
#include <Core/Camera/Camera.h>
#include <Core/Graphics/PrimitiveGenerator/PrimitiveGenerator.h>
#include "Core/Graphics/DebugRenderer/DebugShape.h"
#include "Core/Graphics/Cubemap/Cubemap.h"
#include "Core/Graphics/PostFX/ChromaticAberration.h"
#include "Core/Components/StaticMeshComponent/StaticMeshComponent.h"
#include "Core/Graphics/SkinnedMesh/SkinnedMeshInfo.h"
#include "Core/Components/LightComponent/LightComponent.h"
#include "rendering/UniformBuffer.h"

#include "gtc/matrix_inverse.hpp"

class EditorRenderContext : public RenderContext
{
public:
	EditorRenderContext(RenderDevice* deviceIn, RenderTarget* targetIn, DrawParams drawParamsIn,
	        Sampler* samplerIn, const mat4 perspectiveIn, Camera* CameraIn);
	inline void RenderMesh(NxArray<MeshInfo*> meshes, Shader* shader,  const mat4& transformIn)
	{
		meshRenderBuffer[std::make_pair(meshes, shader)].push_back(transformIn);
	}

	inline void RenderSkinnedMesh(SkinnedMeshInfo* SkinnedMesh, mat4 transformIn)
	{
		SkinnedMeshBuffer[SkinnedMesh].push_back(transformIn);// glm::inverseTranspose(transformIn))
	}

	inline void RenderImpostor(MeshInfo* meshIn, Transform& transformIn)
	{
		ImpostorBuffer.push_back(std::make_pair(meshIn, transformIn));
	}

	inline void RenderPrimitives(VertexArray* VertexArray, Shader* InShader, mat4 transform, DrawParams drawParamsIn)
	{
		NxArray<mat4> transforms;
		transforms.push_back(transform);
		VertexArray->UpdateBuffer(4, &transforms[0], transforms.size() * sizeof(mat4));
		Draw(*InShader, *VertexArray, drawParamsIn, 1);

	}

	void RenderSkybox(RenderTarget* renderTarget);

	void SetLights(Shader* shader);

	void GenerateBRDF();

	void GeneratePBRMapsFromTexture(NString HDRtexture);


	inline void RenderLight(ECS::ComponentHandle<LightComponent> lightComp, vec3 positionIn)
	{
		lightBuffer.push_back(std::make_pair(lightComp, positionIn));
	}

	inline void RenderDebugShapes(DebugShape* shapeIn, const mat4& transformIn)
	{
		debugShapeBuffer[shapeIn].push_back(perspective * mainCamera->GetViewMatrix() * transformIn);
	}

	void InitEditorHelpers();
	void DrawScene(RenderTarget* renderTarget);
	void DrawSkeletal(RenderTarget* renderTarget);
	void DrawImpostors(RenderTarget* renderTarget);
	void DrawEditorHelpers(RenderTarget* renderTarget);
	void DrawDebugShapes(RenderTarget* renderTarget);
	inline void ResizeRenderTargets(float width, float height)
	{
		if (width == 0 || height == 0)
			return;

		sceneRenderTarget->Resize(width, height);
		chromaRenderTarget->Resize(width, height);
		perspective = glm::perspective(glm::radians(45.0f), (float)width / height, 0.1f, 10000.0f);
		
	}

	inline void ResizeViewPort(float width, float height)
	{
		if (width == 0 || height == 0)
			return;

		sceneRenderTarget->Resize(width, height);
		chromaRenderTarget->Resize(width, height);
		mRenderDevice->UpdateFBOSize(this->mRenderTarget->GetId(), width, height);
		perspective = glm::perspective(glm::radians(45.0f), (float)width / height, 0.1f, 10000.0f);
	}
	
    void Flush();
	void SetTextures(Material* material, Shader* shader);
    //~EditorRenderContext();

	inline void ToggleGrid() { bDrawGrid = !bDrawGrid; }
	inline void TogglePostFX() { bPostFX = !bPostFX; }
	inline Texture* GetScreenTexture() { return finalTexture; };
private:
	DrawParams drawParams;
	//Shader& shader;
	Sampler* sampler;
	UniformBuffer* MatrixUniformBuffer;
	UniformBuffer* LightsUniformBuffer;
	mat4 perspective;
	NxMap<std::pair<NxArray<MeshInfo*>, Shader*>, NxArray<mat4>> meshRenderBuffer;
	NxMap<SkinnedMeshInfo*, NxArray<mat4>> SkinnedMeshBuffer;
	NxMap<DebugShape*, NxArray<mat4>> debugShapeBuffer;
	NxArray<std::pair<MeshInfo*, Transform>> ImpostorBuffer;

	NxArray<std::pair<ECS::ComponentHandle<LightComponent>, vec3>> lightBuffer; // pos color

	Camera* mainCamera;

	//Screen
	DrawParams screenQuadDrawParams;
	VertexArray* screenQuadVAO;
	Shader* screenShader;
	RenderTarget* sceneRenderTarget;
	Texture* offscreenTexture;
	Sampler* screenTextureSampler;

	CromaticAberration* chromaFX;
	Texture* chromaTexture;
	RenderTarget* chromaRenderTarget;
	Texture* finalTexture;


	//Grid VO
	Transform editorGridTransform;
	VertexArray* editorGridVA;
	DrawParams editorGridDrawParams;
	float editorGridSlices;
	float editorGridScale;


	//Grid VO
	DrawParams impostorDrawParams;


	//
	VertexArray* cubeVA;
	Cubemap* SkyboxTex;
	Cubemap* IrradMap;
	Cubemap* PrefilterCubemap;
	Sampler* cubemapSampler;
	Sampler* prefilterSampler;
	Sampler* brdfSampler;
	Texture* brdfLUTTexture;

	bool bDrawGrid;
	bool bPostFX;
};

