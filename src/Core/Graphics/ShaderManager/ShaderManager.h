#pragma once 
#include "Common/dataStructs/String.h"
#include "Common/dataStructs/Map.h"
#include "rendering/Shader.h"

class ShaderManager
{
public:

	inline static void SetRenderDevice(RenderDevice* renderDeviceIn)
	{
		renderDevice = renderDeviceIn;
	}

	inline static void SetMainShader(Shader* shaderIn)
	{
		mainPipelineShader = shaderIn;
	}

	inline static Shader* GetMainShader()
	{
		return mainPipelineShader;
	}

	inline static void AddPBRShader(NString shaderName, Shader* shader)
	{
		PBRShaders[shaderName] = shader;
	}

	inline static void AddPostFXshader(NString shaderName, Shader* shader)
	{
		PostFXShaders[shaderName] = shader;
	}

	inline static Shader* GetPostFXshader(NString shaderName)
	{
		return MapFuncs::Find(PostFXShaders, shaderName);
	}

	inline static Shader* GetPBRShader(NString shaderName)
	{
		return MapFuncs::Find(PBRShaders, shaderName);
	}


private:
	static NxMap<NString, Shader*> importedShaders;
	static NxMap<NString, Shader*> PBRShaders;
	static NxMap<NString, Shader*> PostFXShaders;
	static Shader* mainPipelineShader;
	static RenderDevice* renderDevice;
};