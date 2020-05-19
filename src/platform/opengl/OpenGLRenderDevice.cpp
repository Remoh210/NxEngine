
#include "OpenGLRenderDevice.h"

bool OpenGLRenderDevice::GlobalInit() 
{
    return true;
}

OpenGLRenderDevice::OpenGLRenderDevice() 
{
    
}


uint32 OpenGLRenderDevice::CreateTexture2D(int32 width, int32 height, enum PixelFormat dataFormat, 
           const void* data, enum PixelFormat internalFormat, bool bGenerateMipmaps, bool bCompress) 
{
    GLint format = getOpenGLFormat(dataFormat);
	GLint TexinternalFormat = getOpenGLInternalFormat(internalFormat, bCompress);
	GLenum textureTarget = GL_TEXTURE_2D;
	GLuint textureHandle;

    glGenTextures(1, &textureHandle);
	glBindTexture(textureTarget, textureHandle);
	glTexParameterf(textureTarget, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(textureTarget, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(textureTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(textureTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(textureTarget, 0, TexinternalFormat, width, height, 0, format,
			GL_UNSIGNED_BYTE, data);

    if(bGenerateMipmaps)
    {
		glGenerateMipmap(textureTarget);
	} else 
    {
		glTexParameteri(textureTarget, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(textureTarget, GL_TEXTURE_MAX_LEVEL, 0);
	}

	return textureHandle;

}

void OpenGLRenderDevice::ReleaseTexture2D(uint32 texture2D)
{
	glDeleteTextures(1, &texture2D);
}

static GLint getOpenGLFormat(enum PixelFormat format)
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

static GLint getOpenGLInternalFormat(enum PixelFormat format, bool bCompress)
{
	switch(format) {
	case PixelFormat::FORMAT_R: return GL_RED;
	case PixelFormat::FORMAT_RG: return GL_RG;
	case PixelFormat::FORMAT_RGB: 
		if(bCompress) 
        {
			return GL_COMPRESSED_SRGB;
		} 
        else 
        {
			return GL_RGB;

		}
	case PixelFormat::FORMAT_RGBA:
		if(bCompress) 
        {
			return GL_COMPRESSED_SRGB_ALPHA;
		} else 
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
