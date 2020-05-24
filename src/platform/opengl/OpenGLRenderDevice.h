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

enum FaceCulling
{
	FACE_CULL_NONE,
	FACE_CULL_BACK = GL_BACK,
	FACE_CULL_FRONT = GL_FRONT,
	FACE_CULL_FRONT_AND_BACK = GL_FRONT_AND_BACK,
};
enum DrawFunc
{
	DRAW_FUNC_NEVER = GL_NEVER,
	DRAW_FUNC_ALWAYS = GL_ALWAYS,
	DRAW_FUNC_LESS = GL_LESS,
	DRAW_FUNC_GREATER = GL_GREATER,
	DRAW_FUNC_LEQUAL = GL_LEQUAL,
	DRAW_FUNC_GEQUAL = GL_GEQUAL,
	DRAW_FUNC_EQUAL = GL_EQUAL,
	DRAW_FUNC_NOT_EQUAL = GL_NOTEQUAL,
};
	enum BlendFunc
{
	BLEND_FUNC_NONE,
	BLEND_FUNC_ONE = GL_ONE,
	BLEND_FUNC_SRC_ALPHA = GL_SRC_ALPHA,
	BLEND_FUNC_ONE_MINUS_SRC_ALPHA = GL_ONE_MINUS_SRC_ALPHA,
	BLEND_FUNC_ONE_MINUS_DST_ALPHA = GL_ONE_MINUS_DST_ALPHA,
	BLEND_FUNC_DST_ALPHA = GL_DST_ALPHA,
};
enum StencilOp
{
	STENCIL_KEEP = GL_KEEP,
	STENCIL_ZERO = GL_ZERO,
	STENCIL_REPLACE = GL_REPLACE,
	STENICL_INCR = GL_INCR,
	STENCIL_INCR_WRAP = GL_INCR_WRAP,
	STENCIL_DECR_WRAP = GL_DECR_WRAP,
	STENCIL_DECR = GL_DECR,
	STENCIL_INVERT = GL_INVERT,
};


struct DrawParams
{
	PrimitiveType primitiveType = PRIMITIVE_TRIANGLES;
	FaceCulling faceCulling = FACE_CULL_NONE;
	DrawFunc depthFunc = DRAW_FUNC_ALWAYS;
	bool shouldWriteDepth = true;
	bool useStencilTest = false;
	DrawFunc stencilFunc = DRAW_FUNC_ALWAYS;
	uint32 stencilTestMask = 0;
	uint32 stencilWriteMask = 0;
	int32 stencilComparisonVal = 0;
	StencilOp stencilFail = STENCIL_KEEP;
	StencilOp stencilPassButDepthFail = STENCIL_KEEP;
	StencilOp stencilPass = STENCIL_KEEP;
	bool useScissorTest = false;
	uint32 scissorStartX = 0;
	uint32 scissorStartY = 0;
	uint32 scissorWidth = 0;
	uint32 scissorHeight = 0;
	BlendFunc sourceBlend = BLEND_FUNC_NONE;
	BlendFunc destBlend = BLEND_FUNC_NONE;
};

class OpenGLRenderDevice
{
public:
    static bool GlobalInit();
    OpenGLRenderDevice();
    ~OpenGLRenderDevice(){};

	void Draw(uint32 fbo, uint32 shader, uint32 vao,
		const DrawParams& drawParams,
		uint32 numInstances, uint32 numElements);

	void UpdateVertexArrayBuffer(uint32 vao, uint32 bufferIndex,
			const void* data, uintptr dataSize);

	uint32 CreateRenderTarget(uint32 texture, uint32 width, uint32 height,
			FramebufferAttachment attachment, uint32
			attachmentNumber, uint32 mipLevel);
	uint32 ReleaseRenderTarget(uint32 fbo);

    uint32 CreateTexture2D(uint32 width, uint32 height, PixelFormat dataFormat,
			const void* data, PixelFormat internalFormat, bool bGenerateMipmaps, bool bCompress);
    void ReleaseTexture2D(uint32 texture2D);

	uint32 CreateSampler(SamplerFilter minFilter, SamplerFilter magFilter,
			SamplerWrapMode wrapU, SamplerWrapMode wrapV, float anisotropy);
	
	uint32 ReleaseSampler(uint32 sampler);
    
    void SetShaderSampler(uint32 shader, const String& samplerName,
            uint32 texture, uint32 sampler, uint32 unit);

	uint32 CreateShaderProgram(const String& shaderText);

	uint32 CreateVertexArray(const float** vertexData,
		const uint32* vertexElementSizes, uint32 numVertexComponents,
		uint32 numInstanceComponents, uint32 numVertices, const uint32* indices,
		uint32 numIndices, BufferUsage usage);

	void SetVAO(uint32 vao);

	uint32 GetVersion();
	String GetShaderVersion();

	static GLint GetOpenGLFormat(PixelFormat format);
	static GLint GetOpenGLInternalFormat(PixelFormat format, bool bCompress);
private:
	
	
	//Types
	struct VertexArray
	{
		uint32* buffers;
		uintptr* bufferSizes;
		uint32  numBuffers;
		uint32  numElements;
		uint32  instanceComponentsStartIndex;
		BufferUsage usage;
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
	void SetShader(uint32 shader);
	void SetStencilTest(bool bEnable, DrawFunc stencilFunc,
		uint32 stencilTestMask, uint32 stencilWriteMask, int32 stencilComparisonVal,
		StencilOp stencilFail, StencilOp stencilPassButDepthFail,
		StencilOp stencilPass);
	void SetStencilWriteMask(uint32 mask);
	void SetDepthTest(bool bShouldWrite, DrawFunc depthFunc);
	void SetFaceCulling(FaceCulling cullingMode);
	void SetFBO(uint32 fbo);
	void SetBlending(BlendFunc sourceBlend, BlendFunc destBlend);
	void SetScissorTest(bool bEnable, uint32 startX, uint32 startY,
			uint32 width, uint32 height);
	void SetViewport(uint32 fbo);
	

	static bool bIsInitialized;
	//DeviceContext context;
	String shaderVersion;
	uint32 version;
	Map<uint32, VertexArray> vaoMap;
	Map<uint32, FBOData> fboMap;
	Map<uint32, ShaderProgram> shaderProgramMap;

	uint32 boundFBO;
	uint32 viewportFBO;
	uint32 boundVAO;
	uint32 boundShader;

	FaceCulling currentFaceCulling;
	DrawFunc currentDepthFunc;
	BlendFunc currentSourceBlend;
	BlendFunc currentDestBlend;
	DrawFunc currentStencilFunc;
	uint32 currentStencilTestMask;
	uint32 currentStencilWriteMask;
	int32 currentStencilComparisonVal;
	StencilOp currentStencilFail;
	StencilOp currentStencilPassButDepthFail;
	StencilOp currentStencilPass;
	bool bBlendingEnabled;
	bool bShouldWriteDepth;
	bool bStencilTestEnabled;
	bool bScissorTestEnabled;


};

