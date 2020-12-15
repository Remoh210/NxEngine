#include "EditorRenderContext.h"
#include "rendering/VertexArray.h"
#include "Core/Application/Application.h"
#include <Core/Application/Settings/GlobalSettings.h>
#include "Core/FileSystem/FileSystem.h"
#include "Core/Graphics/ShaderManager/ShaderManager.h"
#include "Core/Graphics/Cubemap/CubemapManager.h"

#define MAX_NUM_LIGHTS 20

EditorRenderContext::EditorRenderContext(RenderDevice* deviceIn, RenderTarget* targetIn, DrawParams drawParamsIn,
	        Sampler* samplerIn, const mat4 perspectiveIn, Camera* CameraIn)
            : RenderContext(deviceIn, targetIn),
		    drawParams(drawParamsIn),
			sampler(samplerIn),
			perspective(perspectiveIn),
			mainCamera(CameraIn)
{
	//Settings
	bPostFX = false;
	bDrawGrid = true;

	screenQuadVAO = new VertexArray(mRenderDevice, PrimitiveGenerator::CreateQuad(), BufferUsage::USAGE_STATIC_DRAW);

	MatrixUniformBuffer = new UniformBuffer(deviceIn, 2, sizeof(mat4), BufferUsage::USAGE_STATIC_DRAW);

	InitEditorHelpers();
	GenerateBRDF();

	chromaTexture = new Texture(deviceIn, GlobalSettings::GetWindowWidth(), GlobalSettings::GetWindowHeight(), PixelFormat::FORMAT_RGBA, PixelFormat::FORMAT_RGBA, false, false, false);
	chromaRenderTarget = new RenderTarget(*mRenderDevice, *chromaTexture);

	Shader* PBRShader = ShaderManager::GetPBRShader("PBR_SHADER");
	if (!PBRShader)
	{
		PBRShader->SetUniformBuffer("Matrices", *MatrixUniformBuffer);
		DEBUG_LOG_TEMP("NO PBR SHADER"); return;
	}

	MatrixUniformBuffer->Update(glm::value_ptr(perspective), sizeof(glm::mat4), 0);
	cubemapSampler = new Sampler(mRenderDevice, FILTER_LINEAR);
	prefilterSampler = new Sampler(mRenderDevice, FILTER_LINEAR_MIPMAP_LINEAR);
	brdfSampler = new Sampler(mRenderDevice, FILTER_LINEAR, FILTER_LINEAR, WRAP_CLAMP);

	offscreenTexture = new Texture(deviceIn, GlobalSettings::GetWindowWidth(), GlobalSettings::GetWindowHeight(), PixelFormat::FORMAT_RGB, PixelFormat::FORMAT_RGB16F, false, false, true);
	sceneRenderTarget = new RenderTarget(*mRenderDevice, *offscreenTexture);
	screenShader = ShaderManager::GetPBRShader("SCREEN_SHADER");
	screenTextureSampler = new Sampler(mRenderDevice, FILTER_LINEAR, FILTER_LINEAR);

	GeneratePBRMapsFromTexture("res/textures/HDR/road.hdr");

	screenQuadDrawParams.primitiveType = PRIMITIVE_TRIANGLES;
	screenQuadDrawParams.shouldWriteDepth = false;
	chromaFX = new CromaticAberration(deviceIn, ShaderManager::GetPostFXshader("CHROMA_SHADER"), screenQuadVAO, screenTextureSampler);
}

void EditorRenderContext::Flush()
{
	vec4 clearColor(0.0f, 0.0f, 0.0f, 1.0f);
	mRenderDevice->Clear(sceneRenderTarget->GetId(), true, true, true, clearColor, 0);
	DrawScene(sceneRenderTarget);

	if (bPostFX)
	{
		chromaFX->Apply(sceneRenderTarget, chromaRenderTarget);
		screenShader->SetSampler("Texture", *chromaRenderTarget->GetTexture(), *screenTextureSampler, 0);
		finalTexture = chromaRenderTarget->GetTexture();
	}
	else
	{
		screenShader->SetSampler("Texture", *sceneRenderTarget->GetTexture(), *screenTextureSampler, 0);
		finalTexture = offscreenTexture;
	}

	

	mRenderDevice->Draw(mRenderTarget->GetId(), screenShader->GetId(), 
		screenQuadVAO->GetId(), screenQuadDrawParams, 1, screenQuadVAO->GetNumIndices());
}

void EditorRenderContext::RenderSkybox(RenderTarget* renderTarget)
{
	DrawParams drawParams2;
	Shader* skyboxShader = ShaderManager::GetPBRShader("SKYBOX_SHADER");
	drawParams2.primitiveType = PRIMITIVE_TRIANGLES;
	drawParams2.faceCulling = FACE_CULL_NONE;
	drawParams2.shouldWriteDepth = true;
	drawParams2.depthFunc = DRAW_FUNC_LEQUAL;
	skyboxShader->SetSampler3D("environmentMap", *SkyboxTex, *cubemapSampler, 0);
	mRenderDevice->Draw(renderTarget->GetId(), skyboxShader->GetId(), cubeVA->GetId(), drawParams2, 1, cubeVA->GetNumIndices());
}

void EditorRenderContext::SetLights()
{
	//TODO: Set as struct in glsl
	//Set lights
	Array<int> lightTypes;
	Array<vec3> lightColors;
	Array<vec3> lightPositions;
	Array<vec3> lightDirections;

	int numLights = lightBuffer.size();

	Shader* PBRShader = ShaderManager::GetPBRShader("PBR_SHADER");
	if (!PBRShader)
	{
		DEBUG_LOG_TEMP("NO PBR SHADER"); return;
	}

	PBRShader->SetUniform1i("uNumLights", numLights < MAX_NUM_LIGHTS ? numLights : MAX_NUM_LIGHTS);

	for (std::pair<ECS::ComponentHandle<LightComponent>, vec3> light : lightBuffer)
	{
		lightTypes.push_back(light.first->lightType);
		lightColors.push_back(light.first->color * light.first->intensity);
		lightDirections.push_back(light.first->direction);
		lightPositions.push_back(light.second);
	}

	PBRShader->SetUniform1iv("lightTypes[0]", lightTypes);
	PBRShader->SetUniform3fv("lightColors[0]", lightColors);
	PBRShader->SetUniform3fv("lightPositions[0]", lightPositions);
	PBRShader->SetUniform3fv("lightDirections[0]", lightDirections);



	//PBRShader->SetUniform1f("uAmbient", ambient);
	PBRShader->SetSampler3D("irradianceMap", *IrradMap, *cubemapSampler, 11);

	//
	PBRShader->SetSampler3D("prefilterMap", *PrefilterCubemap, *prefilterSampler, 12);

	PBRShader->SetSampler("brdfLUT", *brdfLUTTexture, *brdfSampler, 13);

	PBRShader->SetUniform3f("uCamPos", mainCamera->Position);

}

void EditorRenderContext::GeneratePBRMapsFromTexture(NString HDRtexture)
{
	//CUBE_MAP*******************
	NString SKYBOX_TEXTURE_FILE = HDRtexture;
	//NString SKYBOX_TEXTURE_FILE = "res/textures/HDR/road.hdr";
	ArrayBitmap hdrBitMap;
	hdrBitMap.Load(SKYBOX_TEXTURE_FILE, true);
	Texture* hdr_texture = new Texture(mRenderDevice, hdrBitMap, PixelFormat::FORMAT_RGB16F, true, false, true);

	Cubemap* hdrCubemap = CubemapManager::GenerateCubemapFromHDR(hdr_texture, 32, 32);
	IrradMap = CubemapManager::GenerateIrradianceMapFromCubeMap(hdrCubemap, 32, 32);

	Cubemap* Cubemap128 = CubemapManager::GenerateCubemapFromHDR(hdr_texture, 512, 512, true);
	PrefilterCubemap = CubemapManager::GenerateSpecularMapFromCubeMap(Cubemap128, ShaderManager::GetPBRShader("PREFILTER_SHADER"), 512, 512);



	if (!ShaderManager::GetPBRShader("SKYBOX_SHADER"))
	{
		ShaderManager::GetPBRShader("SKYBOX_SHADER")->SetUniformBuffer("Matrices", *MatrixUniformBuffer);
		DEBUG_LOG_TEMP("NO PBR SHADER"); return;
	}

	SkyboxTex = CubemapManager::GenerateCubemapFromHDR(hdr_texture, 2048, 2048);

	cubeVA = new VertexArray(mRenderDevice, PrimitiveGenerator::CreateCube(vec3(1.f)), USAGE_STATIC_DRAW);
}

void EditorRenderContext::GenerateBRDF()
{
	brdfLUTTexture = new Texture(mRenderDevice, 512, 512, FORMAT_RG, FORMAT_RGB16F, false, false, true);
	RenderTarget* brdfTarget = new RenderTarget(*mRenderDevice, *brdfLUTTexture);
	DrawParams drawParamsTEST;
	drawParamsTEST.primitiveType = PRIMITIVE_TRIANGLES;
	drawParamsTEST.faceCulling = FACE_CULL_NONE;
	drawParamsTEST.shouldWriteDepth = false;
	drawParamsTEST.depthFunc = DRAW_FUNC_ALWAYS;
	mRenderDevice->Draw(brdfTarget->GetId(), ShaderManager::GetPBRShader("BRDF_SHADER")->GetId(), screenQuadVAO->GetId(), drawParamsTEST
		, 1, screenQuadVAO->GetNumIndices());
}

void EditorRenderContext::InitEditorHelpers()
{
	editorGridSlices = 200;
	editorGridScale = 1000;

	editorGridDrawParams.primitiveType = PRIMITIVE_LINES;
	editorGridDrawParams.shouldWriteDepth = true;
	editorGridDrawParams.depthFunc = DRAW_FUNC_LESS;
	editorGridVA = new VertexArray(mRenderDevice, PrimitiveGenerator::CreateGridVA(editorGridSlices, vec3(0.3f)), BufferUsage::USAGE_DYNAMIC_DRAW);
	//Load and set shaders
	NString LINE_SHADER_TEXT_FILE = "res/shaders/EditorGridSimpleShader.glsl";
	NString LineShaderText;
	Application::loadTextFileWithIncludes(LineShaderText, LINE_SHADER_TEXT_FILE, "#include");
	Shader* grid_shader = new Shader(mRenderDevice, LineShaderText);
	editorGridVA->SetShader(grid_shader);
	editorGridVA->GetShader()->SetUniformBuffer("Matrices", *MatrixUniformBuffer);

	editorGridTransform.scale = vec3(editorGridScale);
	editorGridTransform.position.x = -0.5 * editorGridScale;
	editorGridTransform.position.z = -0.5 * editorGridScale;
}

void EditorRenderContext::DrawScene(RenderTarget* renderTarget)
{
	//Update mat UBO
	mat4 viewMatrix = mainCamera->GetViewMatrix();
	MatrixUniformBuffer->Update(glm::value_ptr(viewMatrix), sizeof(glm::mat4), 1);

	RenderSkybox(renderTarget);
	//Draw Editor stuff first
    DrawEditorHelpers(renderTarget);
    DrawDebugShapes(renderTarget);

	SetLights();

	//Draw meshes
	for (Map<std::pair<Array<MeshInfo*>, Shader*>, Array<mat4> >::iterator it
		= meshRenderBuffer.begin(); it != meshRenderBuffer.end(); ++it)
	{
		mat4* transforms = &it->second[0];
		size_t numTransforms = it->second.size();
		//Shader* modelShader = it->first.second;
		Shader * modelShader = it->first.second;
		//modelShader->SetUniformBuffer("Matrices", *MatrixUniformBuffer);

		//might be unnecessary
		modelShader->SetUniform1f("bUseAlbedoMap", false);
		modelShader->SetUniform1f("bUseNormalMap", false);
		modelShader->SetUniform1f("bUseMetallicMap", false);


		for (MeshInfo* mesh : it->first.first)
		{
			if (mesh->material != nullptr)
			{
				SetTextures(mesh->material, modelShader);
			}


			VertexArray* vertexArray = mesh->vertexArray;

			vertexArray->UpdateBuffer(4, transforms, numTransforms * sizeof(mat4));
			if (vertexArray->GetNumIndices() == 0)
			{
				mRenderDevice->DrawArrays(renderTarget->GetId(), modelShader->GetId(), vertexArray->GetId(),
					drawParams, 300);
			}
			else
			{
				mRenderDevice->Draw(renderTarget->GetId(), modelShader->GetId(), vertexArray->GetId(),
					drawParams, 1, vertexArray->GetNumIndices());

				//Draw to default framebuffer
				//Draw(*modelShader, *vertexArray, drawParams, numTransforms);
			}

			it->second.clear();
		}
	}

	//HACK...
	MatrixUniformBuffer->ResetOffset();

	lightBuffer.clear();
	meshRenderBuffer.clear();
}

void EditorRenderContext::DrawEditorHelpers(RenderTarget* renderTarget)
{
	if (!bDrawGrid) { return; }
    Array<mat4> transforms;
    transforms.push_back(editorGridTransform.ToMatrix());

	editorGridVA->UpdateBuffer(4, &transforms[0], sizeof(mat4));

	mRenderDevice->Draw(renderTarget->GetId(), editorGridVA->GetShader()->GetId(),
		editorGridVA->GetId(), editorGridDrawParams, 1, editorGridVA->GetNumIndices());
}

void EditorRenderContext::DrawDebugShapes(RenderTarget* renderTarget)
{
	Texture* currentTexture = nullptr;
	for (Map<DebugShape*, Array<mat4>>::iterator it
		= debugShapeBuffer.begin(); it != debugShapeBuffer.end(); ++it)
	{
		DebugShape* shapeToDraw = it->first;

		VertexArray* vertexArray = shapeToDraw->vertexArray;
		Texture* texture = shapeToDraw->texture;
		DrawParams shapeDrawParams = shapeToDraw->drawParams;
		mat4* transforms = &it->second[0];
		size_t numTransforms = it->second.size();

		if (numTransforms == 0)
		{
			continue;
		}

		Shader& modelShader = *vertexArray->GetShader();

		if (texture != currentTexture)
		{
			modelShader.SetSampler("diffuse", *texture, *sampler, 0);
		}
		vertexArray->UpdateBuffer(4, transforms, numTransforms * sizeof(mat4));

		mRenderDevice->Draw(renderTarget->GetId(), modelShader.GetId(), vertexArray->GetId(), 
			shapeDrawParams, numTransforms, vertexArray->GetNumIndices());
	}
	debugShapeBuffer.clear();
}


void EditorRenderContext::SetTextures(Material* material, Shader* shader)
{
	uint32 samplerUnit = 1;

	//Albedo
	Texture* albedoTexture = material->textures.Find(TEXTURE_ALBEDO);
	if (albedoTexture != nullptr)
	{
		shader->SetSampler("albedoMap", *albedoTexture, *sampler, samplerUnit);
		shader->SetUniform1f("bUseAlbedoMap", true);
	}
	else
	{
		//TODO: Change That
		shader->SetUniform4f("uColorAlpha", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
		shader->SetUniform1f("bUseAlbedoMap", false);
	}
	//Normal
	Texture* normalTexture = material->textures.Find(TEXTURE_NORMAL);
	if (normalTexture != nullptr)
	{
		samplerUnit++;
		shader->SetSampler("normalMap", *normalTexture, *sampler, samplerUnit);
		shader->SetUniform1f("bUseNormalMap", true);
	}
	else
	{
		shader->SetUniform1f("bUseNormalMap", false);
	}

	//Look for glTF PBR metallic roughness texure
	Texture* MRTexture = material->textures.Find(TEXTURE_MR);
	if (MRTexture != nullptr)
	{
		samplerUnit++;
		shader->SetUniform1i("uPBRTexType", PBR_TEXTURE_MR);
		shader->SetUniform1f("bUseMetallicMap", true);
		shader->SetSampler("metallicMap", *MRTexture, *sampler, samplerUnit);

		//TODO: Use texture channel
		//Ao
		shader->SetUniform1f("uAo", material->ambientOcclusion);
	}
	else
	{
		shader->SetUniform1i("uPBRTexType", PBR_TEXTURE_SPLIT);
		Texture* metallicTexture = material->textures.Find(TEXTURE_METALLIC);
		if (metallicTexture != nullptr)
		{
			samplerUnit++;
			shader->SetSampler("metallicMap", *metallicTexture, *sampler, samplerUnit);
			shader->SetUniform1f("bUseMetallicMap", true);
		}
		else
		{
			shader->SetUniform1f("bUseMetallicMap", false);
			shader->SetUniform1f("uMetallic", material->metallic);
		}
		Texture* roughnessTexture = material->textures.Find(TEXTURE_ROUGHNESS);
		if (roughnessTexture != nullptr)
		{
			samplerUnit++;
			shader->SetSampler("roughnessMap", *roughnessTexture, *sampler, samplerUnit);
			shader->SetUniform1f("bUseRoughnessMap", true);
		}
		else
		{
			shader->SetUniform1f("bUseRoughnessMap", false);
			shader->SetUniform1f("uRoughness", material->roughness);
		}

		Texture* AoTexture = material->textures.Find(TEXTURE_AO);
		if (AoTexture)
		{
			samplerUnit++;
			shader->SetSampler("aoMap", *AoTexture, *sampler, samplerUnit);
			shader->SetUniform1f("bUseAoMap", true);
		}
		else
		{
			//Ao
			shader->SetUniform1f("bUseAoMap", false);
			shader->SetUniform1f("uAo", material->ambientOcclusion);
		}
	}


}