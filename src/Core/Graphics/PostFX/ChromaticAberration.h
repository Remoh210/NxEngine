#pragma once

#include "PostFX.h"
#include "Core/Time/NxTime.h"

class CromaticAberration : public PostFX
{
public:
	CromaticAberration(RenderDevice* deviceIn, Shader* shaderIn, VertexArray* quadVAO, Sampler* samplerIn)
		:PostFX(deviceIn, shaderIn, quadVAO, samplerIn)
	{};

	void Apply(RenderTarget* From, RenderTarget* To)
	{
		float dt = NxTime::GetDeltaTime();
		DrawParams drawParams;
		drawParams.primitiveType = PRIMITIVE_TRIANGLES;
		drawParams.shouldWriteDepth = false;
		playbackTime += dt;
		shader->SetUniform1f("Time", 0.5f * playbackTime);
		shader->SetSampler("Texture", *From->GetTexture(), *sampler, 0);
		renderDevice->Draw(To->GetId(), shader->GetId(), VAO->GetId(), drawParams, 1, VAO->GetNumIndices());
	}

private:
	float playbackTime = 0.f;
};
