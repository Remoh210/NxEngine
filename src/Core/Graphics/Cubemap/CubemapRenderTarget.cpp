#include "CubemapRenderTarget.h"
#include "Core/Graphics/PrimitiveGenerator/PrimitiveGenerator.h"

CubemapRenderTarget::CubemapRenderTarget(RenderDevice* deviceIn, Cubemap* cubemapIn,
	uint32 width, uint32 height)
	:mRenderDevice(deviceIn)
	, cubemapTex(cubemapIn)
	, id(mRenderDevice->CreateRenderTarget(cubemapTex->GetId(), cubemapTex->GetWidth()
	, cubemapTex->GetHeight(), FramebufferAttachment::ATTACHMENT_COLOR, 0, 0))

{

}