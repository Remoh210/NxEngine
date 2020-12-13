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
#include "Core/Components/StaticMeshComponent.h"
#include "Core/Components/LightComponent.h"
#include "rendering/UniformBuffer.h"

class EditorRenderContext : public RenderContext
{
public:
	EditorRenderContext(RenderDevice* deviceIn, RenderTarget* targetIn, DrawParams drawParamsIn,
	        Sampler* samplerIn, const mat4 perspectiveIn, Camera* CameraIn);
	inline void RenderMesh(Array<MeshInfo*> meshes, Shader* shader,  const mat4& transformIn)
	{
		meshRenderBuffer[std::make_pair(meshes, shader)].push_back(transformIn);
	}

	inline void RenderPrimitives(VertexArray* vertexArray, Shader* InShader, mat4 transform, DrawParams drawParamsIn)
	{
		Array<mat4> transforms;
		transforms.push_back(transform);
		vertexArray->UpdateBuffer(4, &transforms[0], transforms.size() * sizeof(mat4));
		Draw(*InShader, *vertexArray, drawParamsIn, 1);

	}

	void RenderSkybox(RenderTarget* renderTarget);

	void SetLights();

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
	void DrawEditorHelpers(RenderTarget* renderTarget);
	void DrawDebugShapes(RenderTarget* renderTarget);
	
    void Flush();
	void SetTextures(Material* material, Shader* shader);
    //~EditorRenderContext();

	inline void ToggleGrid() { bDrawGrid = !bDrawGrid; }
	inline void TogglePostFX() { bPostFX = !bPostFX; }
private:
	DrawParams drawParams;
	//Shader& shader;
	Sampler* sampler;
	UniformBuffer* MatrixUniformBuffer;
	mat4 perspective;
	Map<std::pair<Array<MeshInfo*>, Shader*>, Array<mat4>> meshRenderBuffer;
	Map<DebugShape*, Array<mat4>> debugShapeBuffer;

	Array<std::pair<ECS::ComponentHandle<LightComponent>, vec3>> lightBuffer; // pos color

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


	//Grid VO
	Transform editorGridTransform;
	VertexArray* editorGridVA;
	DrawParams editorGridDrawParams;
	float editorGridSlices;
	float editorGridScale;

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

