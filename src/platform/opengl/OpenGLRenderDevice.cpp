
#include "OpenGLRenderDevice.h"
#include <stb_image.h>


bool OpenGLRenderDevice::bIsInitialized = false;

static bool AddShader(GLuint shaderProgram, const String& text, GLenum type,
		Array<GLuint>* shaders);
//static void addAllAttributes(GLuint program, const String& vertexShaderText, uint32 version);
static bool CheckShaderError(GLuint shader, int flag,
		bool isProgram, const String& errorMessage);
static void AddShaderUniforms(GLuint shaderProgram, const String& shaderText,
		Map<String, GLint>& uniformMap, Map<String, GLint>& samplerMap);

bool OpenGLRenderDevice::GlobalInit() 
{
    if(bIsInitialized) 
	{
		return true;
	}
	int32 major = 3;
	int32 minor = 2;
	
	//bIsInitialized = true;
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

OpenGLRenderDevice::OpenGLRenderDevice() :
	shaderVersion(""), version(0),
	boundFBO(0)
	//viewportFBO(0),
	//boundShader(0),
	//currentFaceCulling(FACE_CULL_NONE),
	//currentDepthFunc(DRAW_FUNC_ALWAYS),
	//currentSourceBlend(BLEND_FUNC_NONE),
	//currentDestBlend(BLEND_FUNC_NONE),
	//currentStencilFunc(DRAW_FUNC_ALWAYS),
	//currentStencilTestMask((uint32)0xFFFFFFFF),
	//currentStencilWriteMask((uint32)0xFFFFFFFF),
	//currentStencilComparisonVal(0),
	//currentStencilFail(STENCIL_KEEP),
	//currentStencilPassButDepthFail(STENCIL_KEEP),
	//currentStencilPass(STENCIL_KEEP),
	//blendingEnabled(false),
	//shouldWriteDepth(false),
	//stencilTestEnabled(false),
{
    
}

uint32 OpenGLRenderDevice::CreateRenderTarget(uint32 texture,
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

uint32 OpenGLRenderDevice::CreateShaderProgram(const String& shaderText)
{
	GLuint shaderProgram = glCreateProgram();

    if(shaderProgram == 0) 
	{
		DEBUG_LOG(LOG_TYPE_RENDERER, LOG_ERROR, "Error creating shader program\n");
        return (uint32)-1;
    }

	String version = GetShaderVersion();
	String vertexShaderText = "#version " + version +
		"\n#define VS_BUILD\n#define GLSL_VERSION " + version + "\n" + shaderText;
	String fragmentShaderText = "#version " + version +
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

    glValidateProgram(shaderProgram);
	if(CheckShaderError(shaderProgram, GL_VALIDATE_STATUS,
				true, "Invalid shader program")) {
		return (uint32)-1;
	}

	//addAllAttributes(shaderProgram, vertexShaderText, getVersion());
	AddShaderUniforms(shaderProgram, shaderText, programData.uniformMap,
			programData.samplerMap);

	shaderProgramMap[shaderProgram] = programData;
	return shaderProgram;

}

static bool AddShader(GLuint shaderProgram, const String& text, GLenum type,
		Array<GLuint>* shaders)
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

static void AddShaderUniforms(GLuint shaderProgram, const String& shaderText,
		Map<String, GLint>& uniformMap, Map<String, GLint>& samplerMap)
{
	GLint numBlocks;
	glGetProgramiv(shaderProgram, GL_ACTIVE_UNIFORM_BLOCKS, &numBlocks);
	for(int32 block = 0; block < numBlocks; ++block) {
		GLint nameLen;
		glGetActiveUniformBlockiv(shaderProgram, block,
				GL_UNIFORM_BLOCK_NAME_LENGTH, &nameLen);

		Array<GLchar> name(nameLen);
		glGetActiveUniformBlockName(shaderProgram, block, nameLen, NULL, &name[0]);
		String uniformBlockName((char*)&name[0], nameLen-1);
		uniformMap[uniformBlockName] = glGetUniformBlockIndex(shaderProgram, &name[0]);
	}

	GLint numUniforms = 0;
	glGetProgramiv(shaderProgram, GL_ACTIVE_UNIFORMS, &numBlocks);
	
	// Would get GL_ACTIVE_UNIFORM_MAX_LENGTH, but buggy on some drivers
	Array<GLchar> uniformName(256); 
	for(int32 uniform = 0; uniform < numUniforms; ++uniform) {
		GLint arraySize = 0;
		GLenum type = 0;
		GLsizei actualLength = 0;
		glGetActiveUniform(shaderProgram, uniform, uniformName.size(),
				&actualLength, &arraySize, &type, &uniformName[0]);
		if(type != GL_SAMPLER_2D) {
			DEBUG_LOG(LOG_TYPE_RENDERER, LOG_ERROR,
					"Non-sampler2d uniforms currently unsupported!");
			continue;
		}
		String name((char*)&uniformName[0], actualLength - 1);
		samplerMap[name] = glGetUniformLocation(shaderProgram, (char*)&uniformName[0]);
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
		bool isProgram, const String& errorMessage)
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


String OpenGLRenderDevice::GetShaderVersion()
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

