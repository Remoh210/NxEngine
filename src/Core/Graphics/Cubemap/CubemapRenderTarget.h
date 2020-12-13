#pragma once
#include "Cubemap.h"
#include "rendering/RenderDevice.h"
#include "rendering/VertexArray.h"
#include "rendering/IndexedModel.h"
#include "rendering/Texture.h"
#include "rendering/Shader.h"


#include "Core/Graphics/ShaderManager/ShaderManager.h"

class CubemapRenderTarget
{
public:
	CubemapRenderTarget(RenderDevice* deviceIn, Cubemap* cubemapIn,
		uint32 width, uint32 height);

	inline ~CubemapRenderTarget() {}



	inline uint32 GetId() { return id; }

private:
	RenderDevice* mRenderDevice;
	
	Cubemap* cubemapTex;
	uint32 id;
};