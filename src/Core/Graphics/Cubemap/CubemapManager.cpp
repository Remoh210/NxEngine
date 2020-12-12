#include "CubemapManager.h"
#include "Core/Graphics/PrimitiveGenerator/PrimitiveGenerator.h"

VertexArray*  CubemapManager::cubeVAO = nullptr;
Sampler*	  CubemapManager::cubemapSampler = nullptr;
RenderDevice* CubemapManager::device = nullptr;
Shader*		  CubemapManager::captureShader;
Shader*		  CubemapManager::irradShader;
Array<mat4>   CubemapManager::captureViews;
mat4		  CubemapManager::captureProjection;

void CubemapManager::Inititialize(RenderDevice* deviceIn, Shader* capShader, Shader* irShader)
{
	device = deviceIn;
	captureShader = capShader;
	irradShader = irShader;
	const IndexedModel& model = PrimitiveGenerator::CreateCube(vec3(1.0f, 1.0f, 1.0f));
	cubeVAO = new VertexArray(device, model, USAGE_STATIC_DRAW);
	cubemapSampler = new Sampler(device, SamplerFilter::FILTER_LINEAR);

	//Only for now maybe

	captureViews.push_back(glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f),  glm::vec3(0.0f, -1.0f, 0.0f)));
	captureViews.push_back(glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	captureViews.push_back(glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f),  glm::vec3(0.0f, 0.0f, 1.0f)));
	captureViews.push_back(glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
	captureViews.push_back(glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f),  glm::vec3(0.0f, -1.0f, 0.0f)));
	captureViews.push_back(glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));

	captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);


}

Cubemap* CubemapManager::GenerateCubemapFromHDR(Texture* texture, uint32 width, uint32 height, bool bGenerateMip)
{
	DrawParams drawParams;
	drawParams.primitiveType = PRIMITIVE_TRIANGLES;
	drawParams.faceCulling = FACE_CULL_NONE;
	drawParams.shouldWriteDepth = false;
	drawParams.depthFunc = DRAW_FUNC_LEQUAL;

	Cubemap* cubemap = new Cubemap(device, width, height, bGenerateMip);
	CubemapRenderTarget renderTarget(device, cubemap, width, height);

	captureShader->SetSampler("equirectangularMap", *texture, *cubemapSampler, 0);
	captureShader->SetUniformMat4("projection", captureProjection);
	for (unsigned int i = 0; i < 6; ++i)
	{
		
		captureShader->SetUniformMat4("view", captureViews[i]);
		//TODO: Change func
		device->GenerateCubemap(renderTarget.GetId(), captureShader->GetId(), cubemap->GetId(),
			cubemap->GetId(), drawParams, cubeVAO->GetNumIndices(), i);

		device->Draw(renderTarget.GetId(), captureShader->GetId(), cubeVAO->GetId(), drawParams, 1, cubeVAO->GetNumIndices());
	}
	return cubemap;
}

Cubemap* CubemapManager::GenerateIrradianceMapFromCubeMap(Cubemap* cubemap, uint32 width, uint32 height)
{
	DrawParams drawParams;
	drawParams.primitiveType = PRIMITIVE_TRIANGLES;
	drawParams.faceCulling = FACE_CULL_NONE;
	drawParams.shouldWriteDepth = false;
	drawParams.depthFunc = DRAW_FUNC_LEQUAL;

	Cubemap* IrradCubemap = new Cubemap(device, width, height);

	CubemapRenderTarget renderTarget(device, IrradCubemap, width, height);
	device->UpdateFBOSize(renderTarget.GetId(), width, height);
	//Shader* conv = ShaderManager::GetPBRShader("IR_CONV_SHADER");
	//TODO: Change the name:
	irradShader->SetSampler3D("environmentMap", *cubemap, *cubemapSampler, 0);
	irradShader->SetUniformMat4("projection", captureProjection);
	for (unsigned int i = 0; i < 6; ++i)
	{
		irradShader->SetUniformMat4("view", captureViews[i]);
		device->GenerateCubemap(renderTarget.GetId(), irradShader->GetId(), IrradCubemap->GetId(),
			cubeVAO->GetId(), drawParams, cubeVAO->GetNumIndices(), i);

		device->Draw(renderTarget.GetId(), irradShader->GetId(), cubeVAO->GetId(), drawParams, 1, cubeVAO->GetNumIndices());
	}

	return IrradCubemap;
}

Cubemap* CubemapManager::GenerateSpecularMapFromCubeMap(Cubemap* cubemap, Shader* prefilterShader, uint32 width, uint32 height)
{
	Cubemap* prefilterMap = new Cubemap(device, width, height, true);
	Sampler sampler(device, SamplerFilter::FILTER_LINEAR_MIPMAP_LINEAR);

	prefilterShader->SetSampler3D("environmentMap", *cubemap, sampler, 0);
	prefilterShader->SetUniformMat4("projection", captureProjection);

	DrawParams drawParams;
	drawParams.primitiveType = PRIMITIVE_TRIANGLES;
	drawParams.faceCulling = FACE_CULL_NONE;
	drawParams.shouldWriteDepth = false;
	drawParams.depthFunc = DRAW_FUNC_LEQUAL;

	CubemapRenderTarget renderTarget(device, prefilterMap, width, height);
	unsigned int maxMipLevels = 5;
	for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
	{
		// reisze framebuffer according to mip-level size.
		unsigned int mipWidth = width * std::pow(0.5, mip);
		unsigned int mipHeight = height * std::pow(0.5, mip);
		device->UpdateFBOSize(renderTarget.GetId(), mipWidth, mipHeight);

		float roughness = (float)mip / (float)(maxMipLevels - 1);
		prefilterShader->SetUniform1f("roughness", roughness);
		for (unsigned int i = 0; i < 6; ++i)
		{
			prefilterShader->SetUniformMat4("view", captureViews[i]);

			device->GenerateCubemap(renderTarget.GetId(), prefilterShader->GetId(), prefilterMap->GetId(),
				cubeVAO->GetId(), drawParams, cubeVAO->GetNumIndices(), i, true, mip);
			device->Draw(renderTarget.GetId(), prefilterShader->GetId(), cubeVAO->GetId(), drawParams, 1, cubeVAO->GetNumIndices());
		}
	}

	return prefilterMap;
}
