#pragma once
#include "RenderDevice.h"
#include "Common/Common.h"
#include "Core/Graphics/Texture/Texture.h"

class RenderTarget
{

public:
    RenderTarget(RenderDevice* deviceIn) 
            : mRenderDevice(deviceIn),
              id(0)
            {}

    inline ~RenderTarget() 
    { 
        if(mRenderDevice)
        {
            mRenderDevice->ReleaseRenderTarget(id);
        } 
    }

    inline RenderTarget(RenderDevice& deviceIn, Texture& texture, 
            uint32 width, uint32 height, 
            FramebufferAttachment attachmentType = FramebufferAttachment::ATTACHMENT_COLOR,
            uint32 attachmentNumber = 0, uint32 mipLevel = 0):
            mRenderDevice(&deviceIn), texture(&texture),
			id(deviceIn.CreateRenderTarget(width, height, 
            texture.GetHeight(), attachmentType, attachmentNumber, mipLevel))
            {}

    inline RenderTarget(RenderDevice& deviceIn, Texture& texture,
            FramebufferAttachment attachmentType = FramebufferAttachment::ATTACHMENT_COLOR,
            uint32 attachmentNumber = 0, uint32 mipLevel = 0):
            mRenderDevice(&deviceIn), texture(&texture),
			id(deviceIn.CreateRenderTarget(texture.GetId(), texture.GetWidth(), 
            texture.GetHeight(), attachmentType, attachmentNumber, mipLevel))
            {}
    
    inline uint32 GetId() { return id; };
	inline Texture* GetTexture() { return texture; };
	inline uint32 GetTexreId() { return texture->GetId(); };

	inline void Resize(uint32 width, uint32 height)
	{
		mRenderDevice->UpdateFBOSize(id, width, height);

		delete texture;
		texture = new Texture(mRenderDevice, width, height, PixelFormat::FORMAT_RGB, PixelFormat::FORMAT_RGB16F, false, false, true);

		mRenderDevice->ReleaseRenderTarget(id);

		id = mRenderDevice->CreateRenderTarget(texture->GetId(), width, height, FramebufferAttachment::ATTACHMENT_COLOR, 0, 0);
	}

private:
    RenderDevice* mRenderDevice;
	Texture* texture;
    uint32 id;
    
};

