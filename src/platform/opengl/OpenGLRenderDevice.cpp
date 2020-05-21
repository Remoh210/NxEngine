
#include "OpenGLRenderDevice.h"
#include <stb_image.h>

bool OpenGLRenderDevice::GlobalInit() 
{
    return true;
}

OpenGLRenderDevice::OpenGLRenderDevice() 
{
    
}

uint32 OpenGLRenderDevice::createRenderTarget(uint32 texture,
		uint32 width, uint32 height,
		enum FramebufferAttachment attachment,
		uint32 attachmentNumber, uint32 mipLevel)
{
	uint32 fbo;
	glGenFramebuffers(1, &fbo);
	SetFBO(fbo);

	GLenum attachmentTypeGL = attachment + attachmentNumber;
	glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentTypeGL,
			GL_TEXTURE_2D, texture, mipLevel);

	struct FBOData data;
	data.width = width;
	data.height = height;
	fboMap[fbo] = data;

	return fbo;
}

uint32 OpenGLRenderDevice::CreateTexture2D(uint32 width, uint32 height, enum PixelFormat dataFormat, 
           const void* data, enum PixelFormat internalFormat, bool bGenerateMipmaps, bool bCompress)
{
 
    uint32 textureID;
    glGenTextures(1, &textureID);
	GLint GLDataFormat = GetOpenGLFormat(dataFormat);
	GLint GLInternalFormat = GetOpenGLInternalFormat(internalFormat, bCompress);

    if (data)
    {
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GLInternalFormat, width, height, 0, GLDataFormat, GL_UNSIGNED_BYTE, data);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		if (bGenerateMipmaps)
		{
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
		}

    }
    else
    {
       DEBUG_LOG(LOG_TYPE_RENDERER, LOG_ERROR, "Missing data");
    }

    return textureID;
    
//    GLint format = GetOpenGLFormat(dataFormat);
//	GLint TexinternalFormat = GetOpenGLInternalFormat(internalFormat, bCompress);
//	GLenum textureTarget = GL_TEXTURE_2D;
//	GLuint textureHandle;
//
//    glGenTextures(1, &textureHandle);
//	glBindTexture(textureTarget, textureHandle);
//	glTexParameterf(textureTarget, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//	glTexParameterf(textureTarget, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//	glTexParameteri(textureTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//	glTexParameteri(textureTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//	glTexImage2D(textureTarget, 0, TexinternalFormat, width, height, 0, format,
//			GL_UNSIGNED_BYTE, data);
//
//    if(bGenerateMipmaps)
//    {
//		glGenerateMipmap(textureTarget);
//	}
//	else
//    {
//		glTexParameteri(textureTarget, GL_TEXTURE_BASE_LEVEL, 0);
//		glTexParameteri(textureTarget, GL_TEXTURE_MAX_LEVEL, 0);
//	}
//
//	return textureHandle;

}

void OpenGLRenderDevice::ReleaseTexture2D(uint32 texture2D)
{
	glDeleteTextures(1, &texture2D);
}

GLint OpenGLRenderDevice::GetOpenGLFormat(enum PixelFormat format)
{
	switch(format) {
	case PixelFormat::FORMAT_R: return GL_RED;
	case PixelFormat::FORMAT_RG: return GL_RG;
	case PixelFormat::FORMAT_RGB: return GL_RGB;
	case PixelFormat::FORMAT_RGBA: return GL_RGBA;
	case PixelFormat::FORMAT_DEPTH: return GL_DEPTH_COMPONENT;
	case PixelFormat::FORMAT_DEPTH_AND_STENCIL: return GL_DEPTH_STENCIL;
	default:
		DEBUG_LOG(LOG_TYPE_RENDERER, LOG_ERROR, "PixelFormat %d is not a valid PixelFormat.",
				format);
		return 0;
	};
}

GLint OpenGLRenderDevice::GetOpenGLInternalFormat(enum PixelFormat format, bool bCompress)
{
	switch(format) {
	case PixelFormat::FORMAT_R: return GL_RED;
	case PixelFormat::FORMAT_RG: return GL_RG;
	case PixelFormat::FORMAT_RGB: 
		if(bCompress) 
        {
			return GL_COMPRESSED_RGB;
		} 
        else 
        {
			return GL_RGB;

		}
	case PixelFormat::FORMAT_RGBA:
		if(bCompress) 
        {
			return GL_COMPRESSED_RGBA;
		}
        else
        {
			return GL_RGBA;
		}
	case PixelFormat::FORMAT_DEPTH: return GL_DEPTH_COMPONENT;
	case PixelFormat::FORMAT_DEPTH_AND_STENCIL: return GL_DEPTH_STENCIL;
	default:
		DEBUG_LOG(LOG_TYPE_RENDERER, LOG_ERROR, "PixelFormat %d is not a valid PixelFormat.",
				format);
		return 0;
	};
}

void OpenGLRenderDevice::SetFBO(uint32 fbo)
{
	if(fbo == boundFBO) 
	{
		return;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	boundFBO = fbo;
}
