#pragma once
#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "common/Common.h"
#include "common/dataStructs/Map.h"
#include "common/dataStructs/Array.h"
#include "common/dataStructs/String.h"



	enum SamplerFilter
	{
		FILTER_NEAREST = GL_NEAREST,
		FILTER_LINEAR = GL_LINEAR,
		FILTER_NEAREST_MIPMAP_NEAREST = GL_NEAREST_MIPMAP_NEAREST,
		FILTER_LINEAR_MIPMAP_NEAREST = GL_LINEAR_MIPMAP_NEAREST,
		FILTER_NEAREST_MIPMAP_LINEAR = GL_NEAREST_MIPMAP_LINEAR,
		FILTER_LINEAR_MIPMAP_LINEAR = GL_LINEAR_MIPMAP_LINEAR,
	};

	enum BufferUsage
	{
		USAGE_STATIC_DRAW = GL_STATIC_DRAW,
		USAGE_STREAM_DRAW = GL_STREAM_DRAW,
		USAGE_DYNAMIC_DRAW = GL_DYNAMIC_DRAW,

		USAGE_STATIC_COPY = GL_STATIC_COPY,
		USAGE_STREAM_COPY = GL_STREAM_COPY,
		USAGE_DYNAMIC_COPY = GL_DYNAMIC_COPY,

		USAGE_STATIC_READ = GL_STATIC_READ,
		USAGE_STREAM_READ = GL_STREAM_READ,
		USAGE_DYNAMIC_READ = GL_DYNAMIC_READ,
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

	enum FramebufferAttachment
	{
		ATTACHMENT_COLOR = GL_COLOR_ATTACHMENT0,
		ATTACHMENT_DEPTH = GL_DEPTH_ATTACHMENT,
		ATTACHMENT_STENCIL = GL_STENCIL_ATTACHMENT
	};

class OpenGLRenderDevice
{
public:
    static bool GlobalInit();
    OpenGLRenderDevice();
    ~OpenGLRenderDevice(){};

	uint32 createRenderTarget(uint32 texture, int32 width, int32 height,
			enum FramebufferAttachment attachment, uint32
			attachmentNumber, uint32 mipLevel);
	uint32 releaseRenderTarget(uint32 fbo);

    uint32 CreateTexture2D(int32 width, int32 height, enum PixelFormat dataFormat,
               const void* data, enum PixelFormat internalFormat, bool bGenerateMipmaps, bool bCompress);
    void ReleaseTexture2D(uint32 texture2D);
	static GLint GetOpenGLFormat(enum PixelFormat format);
	static GLint GetOpenGLInternalFormat(enum PixelFormat format, bool bCompress);
private:
	//Types
	struct VertexArray
	{
		uint32* buffers;
		uintptr* bufferSizes;
		uint32  numBuffers;
		uint32  numElements;
		uint32  instanceComponentsStartIndex;
		enum BufferUsage usage;
	};

	struct ShaderProgram
	{
		Array<uint32> shaders;
		Map<String, int32> uniformMap;
		Map<String, int32> samplerMap;
	};

	struct FBOData
	{
		int32 width;
		int32 height;
	};
	
	//Functions
	void SetFBO(uint32 fbo);

	static bool isInitialized;
	//DeviceContext context;
	String shaderVersion;
	uint32 version;
	Map<uint32, VertexArray> vaoMap;
	Map<uint32, FBOData> fboMap;
	Map<uint32, ShaderProgram> shaderProgramMap;

	uint32 boundFBO;


};

