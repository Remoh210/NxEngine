#include "ShaderManager.h"

 Map<NString, Shader*> ShaderManager::importedShaders;
 Map<NString, Shader*> ShaderManager::PBRShaders;
 Shader*               ShaderManager::mainPipelineShader = nullptr;
 RenderDevice*         ShaderManager::renderDevice = nullptr;