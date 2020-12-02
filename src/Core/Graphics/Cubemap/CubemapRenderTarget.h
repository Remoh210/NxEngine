#pragma once
#include "Cubemap.h"
#include "rendering/RenderDevice.h"
#include "rendering/VertexArray.h"
#include "rendering/IndexedModel.h"
#include "rendering/Shader.h"


#include "Core/Graphics/ShaderManager/ShaderManager.h"

class CubemapRenderTarget
{
public:
	CubemapRenderTarget(RenderDevice* deviceIn, Cubemap* cubemapIn,
		uint32 width, uint32 height);

	inline ~CubemapRenderTarget() { CubeVAO->~VertexArray(); }

	inline void Generate(Shader* shader, Texture* hdrTexture)
	{
		texTest = hdrTexture;
		DrawParams drawParams;
		drawParams.primitiveType = PRIMITIVE_TRIANGLES;
		drawParams.faceCulling = FACE_CULL_NONE;
		drawParams.shouldWriteDepth = true;
		drawParams.depthFunc = DRAW_FUNC_ALWAYS;


		glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
		glm::mat4 captureViews[] =
		{
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
		};


		shader->SetSampler("equirectangularMap", *hdrTexture, *sampler, 0);
		shader->SetUniformMat4("projection", captureProjection);
		for (unsigned int i = 0; i < 6; ++i)
		{
			shader->SetUniformMat4("view", captureViews[i]);
			mRenderDevice->GenerateCubemap(id, shader->GetId(), cubemapTex->GetId(),
				CubeVAO->GetId(), drawParams, CubeVAO->GetNumIndices(), i);
			
			mRenderDevice->Draw(id, shader->GetId(), CubeVAO->GetId(), drawParams, 1, CubeVAO->GetNumIndices());
		}
	}

	inline void TEST_DrawSkybox(Shader* shader)
	{
		DrawParams drawParams;
		drawParams.primitiveType = PRIMITIVE_TRIANGLES;
		drawParams.faceCulling = FACE_CULL_NONE;
		drawParams.shouldWriteDepth = true;
		drawParams.depthFunc = DRAW_FUNC_ALWAYS;
		shader->SetSampler3D("environmentMap", *cubemapTex, *sampler, 0);
		//drawParams.sourceBlend = BLEND_FUNC_SRC_ALPHA;
		//drawParams.destBlend = BLEND_FUNC_ONE;
		mRenderDevice->Draw(0, shader->GetId(), CubeVAO->GetId(), drawParams, 1, CubeVAO->GetNumIndices());
	}

	inline uint32 GetId() { return id; }

private:
	RenderDevice* mRenderDevice;
	Sampler* sampler;
	Cubemap* cubemapTex;
	VertexArray* CubeVAO;
	uint32 id;

	Texture* texTest;
};