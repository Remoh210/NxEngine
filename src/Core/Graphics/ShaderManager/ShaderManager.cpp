#include "ShaderManager.h"

 Map<NString, Shader*> ShaderManager::importedShaders;
 Map<NString, Shader*> ShaderManager::PBRShaders;
 Map<NString, Shader*> ShaderManager::PostFXShaders;
 Shader*               ShaderManager::mainPipelineShader = nullptr;
 RenderDevice*         ShaderManager::renderDevice = nullptr;