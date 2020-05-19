#pragma once
#include "common/Common.h"
#include "glad/gl.h"

	enum SamplerFilter
	{
		FILTER_NEAREST = GL_NEAREST,
		FILTER_LINEAR = GL_LINEAR,
		FILTER_NEAREST_MIPMAP_NEAREST = GL_NEAREST_MIPMAP_NEAREST,
		FILTER_LINEAR_MIPMAP_NEAREST = GL_LINEAR_MIPMAP_NEAREST,
		FILTER_NEAREST_MIPMAP_LINEAR = GL_NEAREST_MIPMAP_LINEAR,
		FILTER_LINEAR_MIPMAP_LINEAR = GL_LINEAR_MIPMAP_LINEAR,
	};

	enum SamplerWrapMode
	{
		WRAP_CLAMP = GL_CLAMP_TO_EDGE,
		WRAP_REPEAT = GL_REPEAT,
		WRAP_REPEAT_MIRROR = GL_MIRRORED_REPEAT,
	};

	enum PixelFormat
	{
		FORMAT_R,
		FORMAT_RG,
		FORMAT_RGB,
		FORMAT_RGBA,
		FORMAT_DEPTH,
		FORMAT_DEPTH_AND_STENCIL
	};

	enum PrimitiveType
	{
		PRIMITIVE_TRIANGLES = GL_TRIANGLES,
		PRIMITIVE_POINTS = GL_POINTS,
		PRIMITIVE_LINE_STRIP = GL_LINE_STRIP,
		PRIMITIVE_LINE_LOOP = GL_LINE_LOOP,
		PRIMITIVE_LINES = GL_LINES,
		PRIMITIVE_LINE_STRIP_ADJACENCY = GL_LINE_STRIP_ADJACENCY,
		PRIMITIVE_LINES_ADJACENCY = GL_LINES_ADJACENCY,
		PRIMITIVE_TRIANGLE_STRIP = GL_TRIANGLE_STRIP,
		PRIMITIVE_TRIANGLE_FAN = GL_TRIANGLE_FAN,
		PRIMITIVE_TRAINGLE_STRIP_ADJACENCY = GL_TRIANGLE_STRIP_ADJACENCY,
		PRIMITIVE_TRIANGLES_ADJACENCY = GL_TRIANGLES_ADJACENCY
	};

class OpenGLRenderDevice
{
public:
    static bool GlobalInit();
    OpenGLRenderDevice();
    ~OpenGLRenderDevice();

    uint32 CreateTexture2D(int32 width, int32 height, enum PixelFormat dataFormat,
               const void* data, enum PixelFormat internalFormat, bool bGenerateMipmaps, bool bCompress);
    void ReleaseTexture2D(uint32 texture2D);
	static GLint GetOpenGLFormat(enum PixelFormat format);
	static GLint GetOpenGLInternalFormat(enum PixelFormat format, bool bCompress);
private:
    
};

