#pragma once
#include "RenderDevice.h"
#include "common/Common.h"
#include "Texture.h"

class RenderTarget
{

public:
    RenderTarget(RenderDevice& deviceIn) 
            : mRenderDevice(&deviceIn),
              id(0)
            {}

    inline ~RenderTarget() 
    { 
        if(mRenderDevice)
        {
            mRenderDevice->ReleaseRenderTarget(id);
        } 
    };

    inline RenderTarget(RenderDevice& deviceIn, Texture& texture, 
            uint32 width, uint32 height, 
            FramebufferAttachment attachmentType = FramebufferAttachment::ATTACHMENT_COLOR,
            uint32 attachmentNumber = 0, uint32 mipLevel = 0):
            mRenderDevice(&deviceIn), id(deviceIn.CreateRenderTarget(width, height, 
            texture.GetHeight(), attachmentType, attachmentNumber, mipLevel))
            {}

    inline RenderTarget(RenderDevice& deviceIn, Texture& texture,
            FramebufferAttachment attachmentType = FramebufferAttachment::ATTACHMENT_COLOR,
            uint32 attachmentNumber = 0, uint32 mipLevel = 0):
            mRenderDevice(&deviceIn), id(deviceIn.CreateRenderTarget(texture.GetId(), texture.GetWidth(), 
            texture.GetHeight(), attachmentType, attachmentNumber, mipLevel))
            {}
    
    inline uint32 GetId() { return id; };

private:
    RenderDevice* mRenderDevice;
    uint32 id;
    
};

