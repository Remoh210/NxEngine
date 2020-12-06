#pragma once
#include "Common/dataStructs/String.h"
#include "Core/Graphics/Cubemap/Cubemap.h"
#include "CubemapRenderTarget.h"
#include "rendering/VertexArray.h"
#include "rendering/Sampler.h"
#include "rendering/RenderDevice.h"

class CubemapManager
{
public:
	CubemapManager(RenderDevice* deviceIn);
	static void Inititialize(RenderDevice* deviceIn, Shader* capShader, Shader* irShader);
	static Cubemap* GenerateCubemapFromHDR(Texture* texture, uint32 width, uint32 height, bool bGenerateMip = false);
	static Cubemap* GenerateIrradianceMapFromCubeMap(Cubemap* cubemap, uint32 width, uint32 height);
	static Cubemap* GenerateSpecularMapFromCubeMap(Cubemap* cubemap, Shader* prefilterShader, uint32 width, uint32 height);
	//static Cubemap* GenerateCubemapFromEnvironment();

private:
	static VertexArray*  cubeVAO;
	static Sampler*		 cubemapSampler;
	static RenderDevice* device;
	static Array<mat4>  captureViews;
	static mat4 captureProjection;
	static Shader* captureShader;
	static Shader* irradShader;
};

