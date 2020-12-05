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
	static Cubemap* GenerateCubemapFromHDR(Texture* texture, uint32 width, uint32 height);
	static Cubemap* GenerateIrradianceMapFromCubeMap(Cubemap* Cubemap, uint32 width, uint32 height);
	//static Cubemap* GenerateCubemapFromEnvironment();

private:
	static VertexArray*  cubeVAO;
	static Sampler*		 cubemapSampler;
	static RenderDevice* device;

	static Shader* captureShader;
	static Shader* irradShader;
};

