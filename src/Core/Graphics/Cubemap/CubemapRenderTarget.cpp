#include "CubemapRenderTarget.h"
#include "Core/Graphics/PrimitiveGenerator/PrimitiveGenerator.h"

CubemapRenderTarget::CubemapRenderTarget(RenderDevice* deviceIn, Cubemap* cubemapIn,
	uint32 width, uint32 height)
	:mRenderDevice(deviceIn)
	, cubemapTex(cubemapIn)
	, id(mRenderDevice->CreateRenderTarget(cubemapTex->GetWidth()
	, cubemapTex->GetHeight()))

{

}

//CubemapRenderTarget::CubemapRenderTarget(RenderDevice* deviceIn, Texture* texture,
//	uint32 width, uint32 height)
//	:mRenderDevice(deviceIn)
//	, id(mRenderDevice->CreateRenderTarget(texture->GetId(), width
//		, height, FramebufferAttachment::ATTACHMENT_COLOR, 0, 0))
//
//{
//
//}