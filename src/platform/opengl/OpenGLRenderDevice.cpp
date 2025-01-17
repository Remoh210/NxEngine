
#include "OpenGLRenderDevice.h"
#include <iostream>
#include <stb_image.h>


bool OpenGLRenderDevice::bIsInitialized = false;

static bool AddShader(GLuint shaderProgram, const NString& text, GLenum type,
		NxArray<GLuint>* shaders);
static void AddAllAttributes(GLuint program, const NString& vertexShaderText, uint32 version);
static bool CheckShaderError(GLuint shader, int flag,
		bool isProgram, const NString& errorMessage);
static void AddShaderUniforms(GLuint shaderProgram, const NString& shaderText,
		NxMap<NString, GLint>& UBOMap, NxMap<NString, GLint>& uniformMap, NxMap<NString, GLint>& samplerMap);

bool OpenGLRenderDevice::GlobalInit() 
{
    if(bIsInitialized) 
	{
		return true;
	}
	int32 major = 3;
	int32 minor = 1;
	
	bIsInitialized = true;
	//if(SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
	//			SDL_GL_CONTEXT_PROFILE_CORE) != 0) {
	//	DEBUG_LOG(LOG_TYPE_RENDERER, LOG_WARNING,
	//			"Could not set core OpenGL profile");
	//	isInitialized = false;
	//}
	//if(SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, major) != 0) {
	//	DEBUG_LOG(LOG_TYPE_RENDERER, LOG_ERROR,
	//			"Could not set major OpenGL version to %d: %s",
	//			major, SDL_GetError());
	//	isInitialized = false;
	//}
	//if(SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, minor) != 0) {
	//	DEBUG_LOG(LOG_TYPE_RENDERER, LOG_ERROR,
	//			"Could not set minor OpenGL version to %d: %s",
	//			minor, SDL_GetError());
	//	isInitialized = false;
	//}
	
	return bIsInitialized;
}

OpenGLRenderDevice::OpenGLRenderDevice(Window* window) :
	shaderVersion(""), version(0),
	boundFBO(0),
	viewportFBO(0),
	boundVAO(0),
	boundShader(0),
	currentFaceCulling(FACE_CULL_NONE),
	currentDepthFunc(DRAW_FUNC_ALWAYS),
	currentSourceBlend(BLEND_FUNC_NONE),
	currentDestBlend(BLEND_FUNC_NONE),
	currentStencilFunc(DRAW_FUNC_ALWAYS),
	currentStencilTestMask((uint32)0xFFFFFFFF),
	currentStencilWriteMask((uint32)0xFFFFFFFF),
	currentStencilComparisonVal(0),
	currentStencilFail(STENCIL_KEEP),
	currentStencilPassButDepthFail(STENCIL_KEEP),
	currentStencilPass(STENCIL_KEEP),
	bBlendingEnabled(false),
	bShouldWriteDepth(false),
	bStencilTestEnabled(false),
	bScissorTestEnabled(false)
{
	//context = SDL_GL_CreateContext(window.getWindowHandle());
	//glewExperimental = GL_TRUE;
	//GLenum res = glewInit();
	//if(res != GLEW_OK) {
	//	DEBUG_LOG(LOG_TYPE_RENDERER, LOG_ERROR, "%s", glewGetErrorString(res));
	//	throw std::runtime_error("Render device could not be initialized");
	//}

	struct FBOData fboWindowData;
	fboWindowData.width = window->GetWidth();
	fboWindowData.height = window->GetHeight();
	fboMap[0] = fboWindowData;
	//glDisable(GL_DITHER);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(DRAW_FUNC_ALWAYS);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	glDepthMask(GL_FALSE);
	//glEnable(GL_FRAMEBUFFER_SRGB);
	//glFrontFace(GL_CW);
}

void OpenGLRenderDevice::Draw(uint32 fbo, uint32 shader, uint32 vao,
		const DrawParams& drawParams,
		uint32 numInstances, uint32 numElements)
{
	if(numInstances == 0) 
	{
		return;
	}

	SetFBO(fbo);
	SetViewport(fbo);
	SetBlending(drawParams.sourceBlend, drawParams.destBlend);
	SetScissorTest(drawParams.useScissorTest,
			drawParams.scissorStartX, drawParams.scissorStartY,
			drawParams.scissorWidth, drawParams.scissorHeight);
	SetFaceCulling(drawParams.faceCulling);
	SetDepthTest(drawParams.shouldWriteDepth, drawParams.depthFunc);
	SetShader(shader);
	SetVAO(vao);

	glLineWidth(drawParams.lineThickness);

	if(numInstances == 1) 
	{
		glDrawElements(drawParams.primitiveType, static_cast<GLsizei>(numElements), GL_UNSIGNED_INT, 0);
	} 
	else 
	{
		glDrawElementsInstanced(drawParams.primitiveType, static_cast<GLsizei>(numElements), GL_UNSIGNED_INT, 0,
				static_cast<GLsizei>(numInstances));
	}
}

void OpenGLRenderDevice::DrawArrays(uint32 fbo, uint32 shader, uint32 vao,
	const DrawParams& drawParams,
	uint32 numVertecies)
{

	SetFBO(fbo);
	SetViewport(fbo);
	SetBlending(drawParams.sourceBlend, drawParams.destBlend);
	SetScissorTest(drawParams.useScissorTest,
		drawParams.scissorStartX, drawParams.scissorStartY,
		drawParams.scissorWidth, drawParams.scissorHeight);
	SetFaceCulling(drawParams.faceCulling);
	SetDepthTest(drawParams.shouldWriteDepth, drawParams.depthFunc);
	SetShader(shader);
	SetVAO(vao);

	glLineWidth(drawParams.lineThickness);
	glDrawArrays(drawParams.primitiveType, 0, static_cast<GLsizei>(numVertecies));
}

void OpenGLRenderDevice::GenerateCubemap(uint32 fbo, uint32 shader, uint32 textureId, uint32 vao,
	const DrawParams& drawParams, uint32 numElements, uint32 count, bool bUseMipLevel, uint32 mip)
{
	SetFBO(fbo);
	SetViewport(fbo);
	if (bUseMipLevel)
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + count, textureId, mip);
	}
	else
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + count, textureId, 0);
	}
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

uint32 OpenGLRenderDevice::ReleaseRenderTarget(uint32 fbo)
{
	if(fbo == 0) {
		return 0;
	}

	NxMap<uint32, FBOData>::iterator it = fboMap.find(fbo);
	if(it == fboMap.end()) {
		return 0;
	}

	glDeleteFramebuffers(1, &fbo);
	fboMap.erase(it);
	return 0;
}

void OpenGLRenderDevice::Clear(uint32 fbo, bool bShouldClearColor, bool bShouldClearDepth,
		bool bShouldClearStencil, const vec4& color, uint32 stencil)
{
	SetFBO(fbo);
	uint32 flags = 0;
	if(bShouldClearColor) {
		flags |= GL_COLOR_BUFFER_BIT;
		glClearColor(color.r, color.g, color.b, color.a);
	}
	if(bShouldClearDepth) {
		flags |= GL_DEPTH_BUFFER_BIT;
	}
	if(bShouldClearStencil) {
		flags |= GL_STENCIL_BUFFER_BIT;
		SetStencilWriteMask(stencil);
	}

	glClear(flags);
}

void OpenGLRenderDevice::UpdateVertexArrayBuffer(uint32 vao, uint32 bufferIndex,
			const void* data, uintptr dataSize)
{
	if(vao == 0) {
		return;
	}

	NxMap<uint32, VertexArray>::iterator it = vaoMap.find(vao);
	if(it == vaoMap.end()) {
		return;
	}
	const struct VertexArray* vaoData = &it->second;
	BufferUsage usage;
	if(bufferIndex >= vaoData->instanceComponentsStartIndex) {
		usage = USAGE_DYNAMIC_DRAW;
	} else {
		usage = vaoData->usage;
	}

	SetVAO(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vaoData->buffers[bufferIndex]);
	if(vaoData->bufferSizes[bufferIndex] >= dataSize) {
		glBufferSubData(GL_ARRAY_BUFFER, 0, dataSize, data);
	} else {
		glBufferData(GL_ARRAY_BUFFER, dataSize, data, usage);
		vaoData->bufferSizes[bufferIndex] = dataSize;
	}	
}

void OpenGLRenderDevice::SetShader(uint32 shader)
{
	if(shader == boundShader) 
	{
		return;
	}
	glUseProgram(shader);
	boundShader = shader;
}


void OpenGLRenderDevice::SetStencilTest(bool bEnable, DrawFunc stencilFunc,
		uint32 stencilTestMask, uint32 stencilWriteMask, int32 stencilComparisonVal,
		StencilOp stencilFail, StencilOp stencilPassButDepthFail,
		StencilOp stencilPass)
{
	if(bEnable != bStencilTestEnabled) 
	{
		if(bEnable) 
		{
			glEnable(GL_STENCIL_TEST);
		} else 
		{
			glDisable(GL_STENCIL_TEST);
		}
		bStencilTestEnabled = bEnable;
	}

	if(stencilFunc != currentStencilFunc || stencilTestMask != currentStencilTestMask
			|| stencilComparisonVal != currentStencilComparisonVal) 
	{
		glStencilFunc(stencilFunc, stencilTestMask, stencilComparisonVal);
		currentStencilComparisonVal = stencilComparisonVal;
		currentStencilTestMask = stencilTestMask;
		currentStencilFunc = stencilFunc;
	}

	if(stencilFail != currentStencilFail || stencilPass != currentStencilPass
			|| stencilPassButDepthFail != currentStencilPassButDepthFail)
	{
		glStencilOp(stencilFail, stencilPassButDepthFail, stencilPass);
		currentStencilFail = stencilFail;
		currentStencilPass = stencilPass;
		currentStencilPassButDepthFail = stencilPassButDepthFail;
	}

	SetStencilWriteMask(stencilWriteMask);
}

void OpenGLRenderDevice::SetStencilWriteMask(uint32 mask)
{
	if(currentStencilWriteMask == mask) 
	{
		return;
	}
	glStencilMask(mask);
	currentStencilWriteMask = mask;
}

void OpenGLRenderDevice::SetDepthTest(bool bShouldWrite, DrawFunc depthFunc)
{
	if (bShouldWrite != bShouldWriteDepth)
	{
		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);

		if (depthFunc != currentDepthFunc)
		{
			glDepthFunc(depthFunc);
			currentDepthFunc = depthFunc;
		}
	}
	else
	{
		glDisable(GL_DEPTH_TEST);
	}
}

void OpenGLRenderDevice::SetFaceCulling(FaceCulling cullingMode)
{
	if(cullingMode == currentFaceCulling) {
		return;
	}
	
	if(cullingMode == FACE_CULL_NONE) { // Face culling is enabled, but needs to be disabled
		glDisable(GL_CULL_FACE);
	} else if(currentFaceCulling == FACE_CULL_NONE) { // Face culling is disabled but needs to be enabled
		glEnable(GL_CULL_FACE);
		glCullFace(cullingMode);
	} else { // Only need to change culling state
		glCullFace(cullingMode);
	}
	currentFaceCulling = cullingMode;
}

void OpenGLRenderDevice::SetScissorTest(bool bEnable, uint32 startX, uint32 startY,
			uint32 width, uint32 height)
{
	if(!bEnable) {
		if(!bScissorTestEnabled) {
			return;
		} else {
			glDisable(GL_SCISSOR_TEST);
			bScissorTestEnabled = false;
			return;
		}
	}
	if(!bScissorTestEnabled) {
		glEnable(GL_SCISSOR_TEST);
	}
	glScissor(startX, startY, width, height);
	bScissorTestEnabled = true;
}

void OpenGLRenderDevice::SetBlending(BlendFunc sourceBlend, BlendFunc destBlend)
{
	if(sourceBlend == currentSourceBlend && destBlend == currentDestBlend) {
		return;
	} else if(sourceBlend == BLEND_FUNC_NONE || destBlend == BLEND_FUNC_NONE) {
		glDisable(GL_BLEND);
	} else if(currentSourceBlend == BLEND_FUNC_NONE || currentDestBlend == BLEND_FUNC_NONE) {
		glEnable(GL_BLEND);
		glBlendFunc(sourceBlend, destBlend);
	} else {
		glBlendFunc(sourceBlend, destBlend);
	}

	currentSourceBlend = sourceBlend;
	currentDestBlend = destBlend;
}

void OpenGLRenderDevice::SetViewport(uint32 fbo)
{
	if(fbo == viewportFBO) {
		return;
	}
	glViewport(0, 0, fboMap[fbo].width, fboMap[fbo].height);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, fboMap[fbo].width, fboMap[fbo].height);
	viewportFBO = fbo;
}

void OpenGLRenderDevice::UpdateFBOSize(uint32 fbo, uint32 width, uint32 height)
{
	fboMap[fbo].width = width;
	fboMap[fbo].height = height;
	SetViewport(fbo);
	glViewport(0, 0, width, height);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);

}

uint32 OpenGLRenderDevice::CreateRenderTarget(uint32 texture,
		uint32 width, uint32 height,
		FramebufferAttachment attachment,
		uint32 attachmentNumber, uint32 mipLevel)
{
	uint32 fbo;
	glGenFramebuffers(1, &fbo);
	SetFBO(fbo);
	GLenum attachmentTypeGL = attachment + attachmentNumber;
	glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentTypeGL, GL_TEXTURE_2D, texture, mipLevel);

	// create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
	unsigned int rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		DEBUG_LOG(LOG_TYPE_RENDERER, LOG_ERROR, "Framebuffer is not complete!");
	}
		
	struct FBOData data;
	data.width = width;
	data.height = height;
	fboMap[fbo] = data;

	return fbo;
}

uint32 OpenGLRenderDevice::CreateRenderTarget(uint32 width, uint32 height)
{

	uint32 fbo;
	uint32 rbo;
	glGenFramebuffers(1, &fbo);
	glGenRenderbuffers(1, &rbo);

	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

	struct FBOData data;
	data.width = width;
	data.height = height;
	fboMap[fbo] = data;

	return fbo;
}

uint32 OpenGLRenderDevice::CreateTexture2D(uint32 width, uint32 height, enum PixelFormat dataFormat, 
           const void* data, PixelFormat internalFormat, bool bGenerateMipmaps, bool bCompress, bool bFloatType)//TODO: TYPE ENUM
{
    uint32 textureID;
    glGenTextures(1, &textureID);
	GLint GLDataFormat = GetOpenGLFormat(dataFormat);
	GLint GLInternalFormat = GetOpenGLInternalFormat(internalFormat, bCompress);

	glBindTexture(GL_TEXTURE_2D, textureID);
	if (bFloatType)
	{
		//TODO: Change that
		glTexImage2D(GL_TEXTURE_2D, 0, GLInternalFormat, width, height, 0, GLDataFormat, GL_FLOAT, data);
	}
	else
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GLInternalFormat, width, height, 0, GLDataFormat, GL_UNSIGNED_BYTE, data);
	}	

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if (bGenerateMipmaps)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
	}
 
    //else
    //{
    //   DEBUG_LOG(LOG_TYPE_RENDERER, LOG_ERROR, "Missing data");
    //}

    return textureID;
}

uint32 OpenGLRenderDevice::CreateTextureCube(uint32 width, uint32 height, PixelFormat dataFormat, const void* data, PixelFormat internalFormat, bool bGenerateMipmaps, bool bCompress)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	GLint GLDataFormat = GetOpenGLFormat(dataFormat);
	GLint GLInternalFormat = GetOpenGLInternalFormat(internalFormat, bCompress);

	for (unsigned int i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GLInternalFormat, width, height, 0, GLDataFormat, GL_FLOAT, data);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (bGenerateMipmaps)
	{
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	}
	else
	{
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		//glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}

	return textureID;
}

void OpenGLRenderDevice::ReleaseTexture2D(uint32 texture2D)
{
	glDeleteTextures(1, &texture2D);
}

uint32 OpenGLRenderDevice::CreateSampler(SamplerFilter minFilter, SamplerFilter magFilter,
			SamplerWrapMode wrapU, SamplerWrapMode wrapV, float anisotropy)
{
	uint32 result = 0;
	glGenSamplers(1, &result);
	glSamplerParameteri(result, GL_TEXTURE_WRAP_S, wrapU);
	glSamplerParameteri(result, GL_TEXTURE_WRAP_T, wrapV);
	glSamplerParameteri(result, GL_TEXTURE_MAG_FILTER, magFilter);
	glSamplerParameteri(result, GL_TEXTURE_MIN_FILTER, minFilter);
	// if(anisotropy != 0.0f && minFilter != FILTER_NEAREST && minFilter != FILTER_LINEAR) {
	// 	glSamplerParameterf(result, GL_TEXTURE_MAX_ANISOTROPY_EXT, anisotropy);
	// }
	return result;
}


uint32 OpenGLRenderDevice::ReleaseSampler(uint32 sampler)
{
	if(sampler == 0) {
		return 0;
	}
	glDeleteSamplers(1, &sampler);
	return 0;
}

GLint OpenGLRenderDevice::GetOpenGLFormat(PixelFormat format)
{
	switch(format) {
	case PixelFormat::FORMAT_R: return GL_RED;
	case PixelFormat::FORMAT_RG: return GL_RG;
	case PixelFormat::FORMAT_RGB: return GL_RGB;
	case PixelFormat::FORMAT_RGB16F: return GL_RGB16F;
	case PixelFormat::FORMAT_SRGB: return GL_SRGB;
	case PixelFormat::FORMAT_RGBA: return GL_RGBA;
	case PixelFormat::FORMAT_DEPTH: return GL_DEPTH_COMPONENT;
	case PixelFormat::FORMAT_DEPTH_AND_STENCIL: return GL_DEPTH_STENCIL;
	default:
		DEBUG_LOG(LOG_TYPE_RENDERER, LOG_ERROR, "PixelFormat %d is not a valid PixelFormat.",
				format);
		return 0;
	};
}

uint32 OpenGLRenderDevice::CreateShaderProgram(const NString& shaderText)
{
	GLuint shaderProgram = glCreateProgram();

    if(shaderProgram == 0) 
	{
		DEBUG_LOG(LOG_TYPE_RENDERER, LOG_ERROR, "Error creating shader program\n");
        return (uint32)-1;
    }

	NString version = GetShaderVersion();
	NString vertexShaderText = "#version " + version +
		"\n#define VS_BUILD\n#define GLSL_VERSION " + version + "\n" + shaderText;
	NString fragmentShaderText = "#version " + version +
		"\n#define FS_BUILD\n#define GLSL_VERSION " + version + "\n" + shaderText;

	ShaderProgram programData;
	if(!AddShader(shaderProgram, vertexShaderText, GL_VERTEX_SHADER,
				&programData.shaders)) {
		return (uint32)-1; 
	}
	if(!AddShader(shaderProgram, fragmentShaderText, GL_FRAGMENT_SHADER,
				&programData.shaders)) {
		return (uint32)-1;
	}
	
	glLinkProgram(shaderProgram);
	if(CheckShaderError(shaderProgram, GL_LINK_STATUS,
				true, "Error linking shader program")) {
		return (uint32)-1;
	}

	 //glValidateProgram(shaderProgram);
	 //if(CheckShaderError(shaderProgram, GL_VALIDATE_STATUS,
	 //			true, "Invalid shader program")) {
	 //	return (uint32)-1;
	 //}

	AddAllAttributes(shaderProgram, vertexShaderText, GetVersion());
	AddShaderUniforms(shaderProgram, shaderText, programData.UBOMap, programData.uniformMap,
			programData.samplerMap);

	shaderProgramMap[shaderProgram] = programData;
	return shaderProgram;

}

uint32 OpenGLRenderDevice::CreateUniformBuffer(const void* data, uintptr dataSize,
	enum BufferUsage usage)
{
	uint32 ubo;
	glGenBuffers(1, &ubo);
	glBindBuffer(GL_UNIFORM_BUFFER, ubo);
	glBufferData(GL_UNIFORM_BUFFER, dataSize, data, usage);
	return ubo;
}

void OpenGLRenderDevice::UpdateUniformBuffer(uint32 buffer, const void* data, uintptr dataSize, uintptr PrevDataSize)
{
	//glBindBuffer(GL_UNIFORM_BUFFER, buffer);
	//void* dest = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
	//memcpy(dest, data, dataSize);
	//glUnmapBuffer(GL_UNIFORM_BUFFER);

	//PrevDataSize = offset
	glBindBuffer(GL_UNIFORM_BUFFER, buffer);
	glBufferSubData(GL_UNIFORM_BUFFER, PrevDataSize, dataSize, data);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

uint32 OpenGLRenderDevice::ReleaseUniformBuffer(uint32 buffer)
{
	if (buffer == 0) {
		return 0;
	}
	glDeleteBuffers(1, &buffer);
	return 0;
}

uint32 OpenGLRenderDevice::CreateVertexArray(const float** vertexData,
	const uint32* vertexElementSizes, uint32 numVertexComponents,
	uint32 numInstanceComponents, uint32 numVertices, const uint32* indices,
	uint32 numIndices, BufferUsage usage)
{
	unsigned int numBuffers = numVertexComponents + numInstanceComponents + 1;

	GLuint VAO;
	GLuint* buffers = new GLuint[numBuffers];
	uintptr* bufferSizes = new uintptr[numBuffers];

	glGenVertexArrays(1, &VAO);
	SetVAO(VAO);

	glGenBuffers(numBuffers, buffers);
	for(uint32 i = 0, attribute = 0; i < numBuffers-1; i++) {
		BufferUsage attribUsage = usage;
		bool inInstancedMode = false;
		if(i >= numVertexComponents) {
			attribUsage = USAGE_DYNAMIC_DRAW;
			inInstancedMode = true;
		}

		uint32 elementSize = vertexElementSizes[i];
		const void* bufferData = inInstancedMode ? nullptr : vertexData[i];
		uintptr dataSize = inInstancedMode
			? elementSize * sizeof(float)
			: elementSize * sizeof(float) * numVertices;
		
		glBindBuffer(GL_ARRAY_BUFFER, buffers[i]);
		glBufferData(GL_ARRAY_BUFFER, dataSize, bufferData, attribUsage);
		bufferSizes[i] = dataSize;

		// Because OpenGL doesn't support attributes with more than 4
		// elements, each set of 4 elements gets its own attribute.
		uint32 elementSizeDiv = elementSize/4;
		uint32 elementSizeRem = elementSize%4;
		for(uint32 j = 0; j < elementSizeDiv; j++) {
			glEnableVertexAttribArray(attribute);
			glVertexAttribPointer(attribute, 4, GL_FLOAT, GL_FALSE,
					elementSize * sizeof(GLfloat),
					(const GLvoid*)(sizeof(GLfloat) * j * 4));
			if(inInstancedMode) {
				glVertexAttribDivisor(attribute, 1);
			}
			attribute++;
		}
		if(elementSizeRem != 0) {
			glEnableVertexAttribArray(attribute);
			glVertexAttribPointer(attribute, elementSize, GL_FLOAT, GL_FALSE,
					elementSize * sizeof(GLfloat),
					(const GLvoid*)(sizeof(GLfloat) * elementSizeDiv * 4));
			if(inInstancedMode) {
				glVertexAttribDivisor(attribute, 1);
			}
			attribute++;
		}

	}

	if (numIndices > 0)
	{
		uintptr indicesSize = numIndices * sizeof(uint32);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[numBuffers - 1]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesSize,
			indices, usage);
		bufferSizes[numBuffers - 1] = indicesSize;
	}

	struct VertexArray vaoData;
	vaoData.buffers = buffers;
	vaoData.bufferSizes = bufferSizes;
	vaoData.numBuffers = numBuffers;
	vaoData.numElements = numIndices;
	vaoData.usage = usage;
	vaoData.instanceComponentsStartIndex = numVertexComponents;
	vaoMap[VAO] = vaoData;
	return VAO;

}

uint32 OpenGLRenderDevice::ReleaseVertexArray(uint32 vao)
{
	if (vao == 0) {
		return 0;
	}
	NxMap<uint32, VertexArray>::iterator it = vaoMap.find(vao);
	if (it == vaoMap.end()) {
		return 0;
	}
	const struct VertexArray* vaoData = &it->second;

	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(vaoData->numBuffers, vaoData->buffers);
	delete[] vaoData->buffers;
	delete[] vaoData->bufferSizes;
	vaoMap.erase(it);
	return 0;

}


void OpenGLRenderDevice::SetVAO(uint32 vao)
{
	if(vao == boundVAO) {
		return;
	}
	glBindVertexArray(vao);
	boundVAO = vao;
}

static bool AddShader(GLuint shaderProgram, const NString& text, GLenum type,
		NxArray<GLuint>* shaders)
{
	GLuint shader = glCreateShader(type);

	if(shader == 0)
	{
		DEBUG_LOG(LOG_TYPE_RENDERER, LOG_ERROR, "Error creating shader type %d", type);
		return false;
	}

	const GLchar* p[1];
	p[0] = text.c_str();
	GLint lengths[1];
	lengths[0] = (GLint)text.length();

	glShaderSource(shader, 1, p, lengths);
	glCompileShader(shader);

	GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) 
	{
        GLchar InfoLog[1024];

        glGetShaderInfoLog(shader, 1024, NULL, InfoLog);
        DEBUG_LOG(LOG_TYPE_RENDERER, LOG_ERROR, "Error compiling shader type %d: '%s'\n",
				shader, InfoLog);	
		return false;
    }

	glAttachShader(shaderProgram, shader);
	shaders->push_back(shader);
	return true;
}

static void AddShaderUniforms(GLuint shaderProgram, const NString& shaderText,
		NxMap<NString, GLint>& UBOMap, NxMap<NString, GLint>& uniformMap, NxMap<NString, GLint>& samplerMap)
{
	GLint numBlocks;
	glGetProgramiv(shaderProgram, GL_ACTIVE_UNIFORM_BLOCKS, &numBlocks);
	for(int32 block = 0; block < numBlocks; ++block) {
		GLint nameLen;
		glGetActiveUniformBlockiv(shaderProgram, block,
				GL_UNIFORM_BLOCK_NAME_LENGTH, &nameLen);

		NxArray<GLchar> name(nameLen);
		glGetActiveUniformBlockName(shaderProgram, block, nameLen, NULL, &name[0]);
		NString uniformBlockName((char*)&name[0], nameLen-1);
		UBOMap[uniformBlockName] = glGetUniformBlockIndex(shaderProgram, &name[0]);
	}

	GLint numUniforms = 0;
	glGetProgramiv(shaderProgram, GL_ACTIVE_UNIFORMS, &numUniforms);
	
	// Would get GL_ACTIVE_UNIFORM_MAX_LENGTH, but buggy on some drivers
	NxArray<GLchar> uniformName(256); 
	for(int32 uniform = 0; uniform < numUniforms; ++uniform) {
		GLint NxArraySize = 0;
		GLenum type = 0;
		GLsizei actualLength = 0;
		glGetActiveUniform(shaderProgram, uniform, uniformName.size(),
				&actualLength, &NxArraySize, &type, &uniformName[0]);
		NString name((char*)&uniformName[0]);
		if(type == GL_SAMPLER_2D)
		{
			//DEBUG_LOG_TEMP("Sampler uniform: %s", (char*)&uniformName[0]);
			samplerMap[name] = glGetUniformLocation(shaderProgram, (char*)&uniformName[0]);
		}
		else if (type == GL_SAMPLER_CUBE)
		{
			//DEBUG_LOG_TEMP("Sampler uniform: %s", (char*)&uniformName[0]);
			samplerMap[name] = glGetUniformLocation(shaderProgram, (char*)&uniformName[0]);
		}
		else
		{
			//DEBUG_LOG_TEMP("Uniform: %s", (char*)&uniformName[0]);
			uniformMap[name] = glGetUniformLocation(shaderProgram, (char*)&uniformName[0]);
		}

	}

	//DEBUG_LOG_TEMP("UniformSize: %d", uniformMap.size());
}

static void AddAllAttributes(GLuint program, const NString& vertexShaderText, uint32 version)
{
	if(version >= 320) {
		// Layout is enabled. Return.
		return;
	}

	// FIXME: This code assumes attributes are listed in order, which isn't
	// true for all compilers. It's safe to ignore for now because OpenGL versions
	// requiring this aren't being used.
	GLint numActiveAttribs = 0;
	GLint maxAttribNameLength = 0;

	glGetProgramiv(program, GL_ACTIVE_ATTRIBUTES, &numActiveAttribs);
	glGetProgramiv(program, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxAttribNameLength);

//	DEBUG_LOG_TEMP2("Adding attributes!");
//	DEBUG_LOG_TEMP("%i %i", numActiveAttribs, maxAttribNameLength);
	NxArray<GLchar> nameData(maxAttribNameLength);
	for(GLint attrib = 0; attrib < numActiveAttribs; ++attrib) {
		GLint NxArraySize = 0;
		GLenum type = 0;
		GLsizei actualLength = 0;

		glGetActiveAttrib(program, attrib, nameData.size(),
				&actualLength, &NxArraySize, &type, &nameData[0]);
		glBindAttribLocation(program, attrib, (char*)&nameData[0]);
//		DEBUG_LOG_TEMP2("Adding attribute!");
//		DEBUG_LOG_TEMP("%s: %d", (char*)&nameData[0], attrib);
	}
}

uint32 OpenGLRenderDevice::GetVersion()
{
	if(version != 0) {
		return version;
	}

	int32 majorVersion;
	int32 minorVersion;
		
	glGetIntegerv(GL_MAJOR_VERSION, &majorVersion); 
	glGetIntegerv(GL_MINOR_VERSION, &minorVersion);
		
	version = (uint32)(majorVersion * 100 + minorVersion * 10);
	return version;
}

static bool CheckShaderError(GLuint shader, int flag,
		bool isProgram, const NString& errorMessage)
{
	GLint success = 0;
    GLchar error[1024] = { 0 };

	if(isProgram) {
		glGetProgramiv(shader, flag, &success);
	} else {
		glGetShaderiv(shader, flag, &success);
	}

	if(!success) {
		if(isProgram) {
			glGetProgramInfoLog(shader, sizeof(error), NULL, error);
		} else {
			glGetShaderInfoLog(shader, sizeof(error), NULL, error);
		}

		DEBUG_LOG(LOG_TYPE_RENDERER, LOG_ERROR, "%s: '%s'\n", errorMessage.c_str(), error);
		return true;
	}
	return false;
}


NString OpenGLRenderDevice::GetShaderVersion()
{
    if(!shaderVersion.empty()) {
		return shaderVersion;
	}
	
	uint32 version = GetVersion();

	if(version >= 330) {
		shaderVersion = StringFuncs::toString(version);
	}
	else if(version >= 320) {
		shaderVersion = "150";
	}
	else if(version >= 310) {
		shaderVersion = "140";
	}
	else if(version >= 300) {
		shaderVersion = "130";
	}
	else if(version >= 210) {
		shaderVersion = "120";
	}
	else if(version >= 200) {
		shaderVersion = "110";
	}
	else {
		int32 majorVersion = version / 100;
		int32 minorVersion = (version / 10) % 10;
		DEBUG_LOG(LOG_TYPE_RENDERER, LOG_ERROR,
				"Error: OpenGL Version %d.%d does not support shaders.\n",
				majorVersion, minorVersion);
		return "";
	}

	return shaderVersion;
}

void OpenGLRenderDevice::SwitchFBO(uint32 fbo, const DrawParams& drawParams)
{
	SetFBO(fbo);
	SetViewport(fbo);
	// SetBlending(drawParams.sourceBlend, drawParams.destBlend);
	// SetScissorTest(drawParams.useScissorTest,
	// 		drawParams.scissorStartX, drawParams.scissorStartY,
	// 		drawParams.scissorWidth, drawParams.scissorHeight);
	// SetFaceCulling(drawParams.faceCulling);
	// SetDepthTest(drawParams.shouldWriteDepth, drawParams.depthFunc);
	//SetShader(shader);
	//SetVAO(vao);
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
	case PixelFormat::FORMAT_RGB16F: return GL_RGB16F;
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

void OpenGLRenderDevice::SetShaderSampler(uint32 shader, const std::string &samplerName, uint32 texture, uint32 sampler, uint32 unit)
{
    SetShader(shader);
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, texture);
	glBindSampler(unit, sampler);
    glUniform1i(shaderProgramMap.at(shader).samplerMap.at(samplerName), unit);
}

void OpenGLRenderDevice::SetShaderSampler3D(uint32 shader, const std::string &samplerName, uint32 texture, uint32 sampler, uint32 unit)
{
	SetShader(shader);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
	glBindSampler(unit, sampler);
	glUniform1i(shaderProgramMap.at(shader).samplerMap.at(samplerName), unit);
}

void OpenGLRenderDevice::SetShaderUniform1i(uint32 shader, const NString& uniformName, int value)
{
	SetShader(shader);
    glUniform1i(shaderProgramMap.at(shader).uniformMap.at(uniformName), value);
}

void OpenGLRenderDevice::SetNxArrayUniform1iv(uint32 shader, const NString& uniformName, uint32 size, int* value)
{
	SetShader(shader);
	glUniform1iv(shaderProgramMap.at(shader).uniformMap.at(uniformName), size, value);
}

void OpenGLRenderDevice::SetShaderUniform1f(uint32 shader, const NString& uniformName, float value)
{
	SetShader(shader);
	glUniform1f(shaderProgramMap.at(shader).uniformMap.at(uniformName), value);
}

void OpenGLRenderDevice::SetShaderUniform3f(uint32 shader, const NString& uniformName, float* value)
{
	SetShader(shader);
	glUniform3f(shaderProgramMap.at(shader).uniformMap.at(uniformName), value[0], value[1], value[2]);
}

void OpenGLRenderDevice::SetShaderUniform2f(uint32 shader, const NString& uniformName, float* value)
{
	SetShader(shader);
	glUniform2f(shaderProgramMap.at(shader).uniformMap.at(uniformName), value[0], value[1]);
}

void OpenGLRenderDevice::SetNxArrayUniform3fv(uint32 shader, const NString& uniformName, uint32 size, float* value)
{
	SetShader(shader);
	glUniform3fv(shaderProgramMap.at(shader).uniformMap.at(uniformName), size, value);
}

void OpenGLRenderDevice::SetShaderUniform4f(uint32 shader, const NString& uniformName, float* value)
{
	SetShader(shader);
	glUniform4f(shaderProgramMap.at(shader).uniformMap.at(uniformName), value[0], value[1], value[2], value[3]);
}

void OpenGLRenderDevice::SetShaderUniform4fv(uint32 shader, const NString& uniformName, uint32 size, float* value)
{
	SetShader(shader);
	glUniform4fv(shaderProgramMap.at(shader).uniformMap.at(uniformName), size, value);
}

void OpenGLRenderDevice::SetShaderUniformMat4(uint32 shader, const NString& uniformName, float* value)
{
	SetShader(shader);
	glUniformMatrix4fv(shaderProgramMap.at(shader).uniformMap.at(uniformName), 1, GL_FALSE, value);
}

void OpenGLRenderDevice::SetShaderUniformMat4v(uint32 shader, const NString& uniformName, uint32 size, float* value)
{
	SetShader(shader);
	glUniformMatrix4fv(shaderProgramMap.at(shader).uniformMap.at(uniformName), size, GL_FALSE, value);
}

void OpenGLRenderDevice::SetShaderUniformBuffer(uint32 shader, const NString& uniformBufferName,
	uint32 buffer)
{
	SetShader(shader);
	//glUniformBlockBinding(shader, shaderProgramMap[shader].UBOMap[uniformBufferName], 0);
	glBindBufferBase(GL_UNIFORM_BUFFER,
		shaderProgramMap[shader].UBOMap[uniformBufferName],
		buffer);
}

uint32 OpenGLRenderDevice::ReleaseShaderProgram(uint32 shader)
{
	if (shader == 0) {
		return 0;
	}
	NxMap<uint32, ShaderProgram>::iterator programIt = shaderProgramMap.find(shader);
	if (programIt == shaderProgramMap.end()) {
		return 0;
	}
	const struct ShaderProgram* shaderProgram = &programIt->second;

	for (NxArray<uint32>::const_iterator it = shaderProgram->shaders.begin();
		it != shaderProgram->shaders.end(); ++it)
	{
		glDetachShader(shader, *it);
		glDeleteShader(*it);
	}
	glDeleteProgram(shader);
	shaderProgramMap.erase(programIt);
	return 0;
}




