#include "CubemapRenderTarget.h"
#include "Core/Graphics/PrimitiveGenerator/PrimitiveGenerator.h"

CubemapRenderTarget::CubemapRenderTarget(RenderDevice* deviceIn, Cubemap* cubemapIn,
	uint32 width, uint32 height)
	:mRenderDevice(deviceIn)
	, cubemapTex(cubemapIn)
	, id(mRenderDevice->CreateRenderTarget(cubemapTex->GetId(), cubemapTex->GetWidth()
	, cubemapTex->GetHeight(), FramebufferAttachment::ATTACHMENT_COLOR, 0, 0))

{
	IndexedModel& model = PrimitiveGenerator::CreateCube(vec3(1.0f, 1.0f, 1.0f));
	CubeVAO = new VertexArray(mRenderDevice, model, USAGE_STATIC_DRAW);

	sampler = new Sampler(mRenderDevice, SamplerFilter::FILTER_LINEAR);
	DrawParams drawParams;
	drawParams.primitiveType = PRIMITIVE_TRIANGLES;
	drawParams.faceCulling = FACE_CULL_NONE;
	drawParams.shouldWriteDepth = true;
	drawParams.depthFunc = DRAW_FUNC_LESS;
	//Create Vao

}