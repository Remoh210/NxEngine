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


private:
	static Map<NString, Shader*> importedShaders;
	static Shader* mainPipelineShader;
	static RenderDevice* renderDevice;
};