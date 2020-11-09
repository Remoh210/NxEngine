#include "ShaderManager.h"

 Map<NString, Shader*> ShaderManager::importedShaders;
 Shader*               ShaderManager::mainPipelineShader = nullptr;
 RenderDevice*         ShaderManager::renderDevice = nullptr;