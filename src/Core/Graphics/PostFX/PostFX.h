#pragma once

#include "rendering/RenderDevice.h"
#include "rendering/RenderTarget.h"
#include "rendering/Shader.h"
#include "rendering/VertexArray.h"

class PostFX
{
public:
	PostFX(RenderDevice* deviceIn, Shader* shaderIn, VertexArray* quadVAO, Sampler* samplerIn)
		:renderDevice(deviceIn)
		,shader(shaderIn)
		,VAO(quadVAO)
		,sampler(samplerIn)
	{};

protected:
	Shader* shader;
	VertexArray* VAO;
	RenderDevice* renderDevice;
	Sampler* sampler;
};
