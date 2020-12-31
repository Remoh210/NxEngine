#include "ShaderManager.h"

 NxMap<NString, Shader*> ShaderManager::importedShaders;
 NxMap<NString, Shader*> ShaderManager::PBRShaders;
 NxMap<NString, Shader*> ShaderManager::PostFXShaders;
 Shader*               ShaderManager::mainPipelineShader = nullptr;
 RenderDevice*         ShaderManager::renderDevice = nullptr;