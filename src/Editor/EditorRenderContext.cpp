#include "EditorRenderContext.h"
#include "rendering/VertexArray.h"
#include "Core/Application/Application.h"
#include <Core/Application/Settings/GlobalSettings.h>
#include "Core/FileSystem/FileSystem.h"
#include "Core/Graphics/ShaderManager/ShaderManager.h"
#include "Core/Graphics/Cubemap/CubemapManager.h"
#include "Core/Graphics/DebugRenderer/DebugRenderer.h"

#define MAX_NUM_LIGHTS 30

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

	impostorDrawParams.faceCulling = FACE_CULL_FRONT;
	impostorDrawParams.shouldWriteDepth = true;
	impostorDrawParams.depthFunc = DRAW_FUNC_LESS;
	impostorDrawParams.sourceBlend = BLEND_FUNC_SRC_ALPHA;
	impostorDrawParams.destBlend = BLEND_FUNC_ONE_MINUS_SRC_ALPHA;
	screenQuadVAO = new VertexArray(mRenderDevice, PrimitiveGenerator::CreateQuad(), BufferUsage::USAGE_STATIC_DRAW);

	MatrixUniformBuffer = new UniformBuffer(deviceIn, BufferUsage::USAGE_STATIC_DRAW);
	MatrixUniformBuffer->AllocateElement(sizeof(mat4));
	MatrixUniformBuffer->AllocateElement(sizeof(mat4));
	
	MatrixUniformBuffer->Generate();

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

	if (DebugRenderer::GetShader())
	{
		DebugRenderer::GetShader()->SetUniformBuffer("Matrices", *MatrixUniformBuffer);
	}

	ShaderManager::GetPBRShader("IMPOSTOR_SHADER")->SetUniformBuffer("Matrices", *MatrixUniformBuffer);

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
		finalTexture = sceneRenderTarget->GetTexture();
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

void EditorRenderContext::SetLights(Shader* shader)
{
	//TODO: Set as struct in glsl
	NxArray<int> lightTypes;
	NxArray<vec3> lightColors;
	NxArray<vec3> lightPositions;
	NxArray<vec3> lightDirections;

	int numLights = lightBuffer.size();

	if (!shader)
	{
		DEBUG_LOG_TEMP("NO SHADER"); return;
	}

	shader->SetUniform1i("uNumLights", numLights < MAX_NUM_LIGHTS ? numLights : MAX_NUM_LIGHTS);

	for (std::pair<ECS::ComponentHandle<LightComponent>, vec3> light : lightBuffer)
	{
		lightTypes.push_back(static_cast<int>(light.first->lightType));
		lightColors.push_back(light.first->color.ToVec() * light.first->intensity);
		lightDirections.push_back(light.first->direction.ToVec());
		lightPositions.push_back(light.second);
	}

	shader->SetUniform1iv("lightTypes[0]", lightTypes);
	shader->SetUniform3fv("lightColors[0]", lightColors);
	shader->SetUniform3fv("lightPositions[0]", lightPositions);
	shader->SetUniform3fv("lightDirections[0]", lightDirections);

	//PBRShader->SetUniform1f("uAmbient", ambient);
	shader->SetSampler3D("irradianceMap", *IrradMap, *cubemapSampler, 11);
	shader->SetSampler3D("prefilterMap", *PrefilterCubemap, *prefilterSampler, 12);

	shader->SetSampler("brdfLUT", *brdfLUTTexture, *brdfSampler, 13);

	shader->SetUniform3f("uCamPos", mainCamera->Position);
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
	editorGridTransform.position.ToVec().x = -0.5 * editorGridScale;
	editorGridTransform.position.ToVec().z = -0.5 * editorGridScale;
}

void EditorRenderContext::DrawScene(RenderTarget* renderTarget)
{
	//Update mat UBO
	mat4 viewMatrix = mainCamera->GetViewMatrix();
	MatrixUniformBuffer->Update(glm::value_ptr(perspective), sizeof(glm::mat4), 16);
	MatrixUniformBuffer->Update(glm::value_ptr(viewMatrix), sizeof(glm::mat4), 16);


	RenderSkybox(renderTarget);
	//Draw Editor stuff first
    DrawEditorHelpers(renderTarget);
    DrawDebugShapes(renderTarget);



	SetLights(ShaderManager::GetMainShader());

	//Draw meshes
	for (NxMap<std::pair<NxArray<MeshInfo*>, Shader*>, NxArray<mat4> >::iterator it
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


			VertexArray* VertexArray = mesh->vertexArray;

			VertexArray->UpdateBuffer(4, transforms, numTransforms * sizeof(mat4));
			if (VertexArray->GetNumIndices() == 0)
			{
				mRenderDevice->DrawArrays(renderTarget->GetId(), modelShader->GetId(), VertexArray->GetId(),
					drawParams, 300);
			}
			else
			{
				mRenderDevice->Draw(renderTarget->GetId(), modelShader->GetId(), VertexArray->GetId(),
					drawParams, 1, VertexArray->GetNumIndices());
			}

			it->second.clear();
		}
	}

	DrawSkeletal(renderTarget);

	DrawImpostors(renderTarget);
	//HACK...
	MatrixUniformBuffer->ResetOffset();

	lightBuffer.clear();
	meshRenderBuffer.clear();
}

void EditorRenderContext::DrawSkeletal(RenderTarget* renderTarget)
{
	typedef NxMap<SkinnedMeshInfo*, NxArray<mat4>>::iterator it;
	Shader * skinnedShader = ShaderManager::GetPBRShader("MAIN_SKELETAL");

	SetLights(skinnedShader);

	for (auto it = SkinnedMeshBuffer.begin(); it != SkinnedMeshBuffer.end(); ++it)
	{
		NxArray<mat4> InvTransposed;
		for(mat4 curMat : it->second)
		{
			InvTransposed.push_back(glm::inverse(glm::transpose(curMat))); //glm::inverseTranspose(curMat))
		}

		mat4* transforms = &it->second[0];
		mat4* transformsInvT = &InvTransposed[0];

		size_t numTransforms = it->second.size();
		

		//modelShader->SetUniformBuffer("Matrices", *MatrixUniformBuffer);

		//might be unnecessary
		skinnedShader->SetUniform1f("bUseAlbedoMap", false);
		skinnedShader->SetUniform1f("bUseNormalMap", false);
		skinnedShader->SetUniform1f("bUseMetallicMap", false);

		
		if (it->first->vecFinalTransformation.size() > 0)
		{
			skinnedShader->SetUniform1i("numBonesUsed", it->first->vecFinalTransformation.size());
			skinnedShader->SetUniformMat4v("bones[0]", it->first->vecFinalTransformation);
		}
		else
		{
			skinnedShader->SetUniform1i("numBonesUsed", 0);
		}
		

		MeshInfo* mesh = it->first->mesh;
		if (mesh->material != nullptr)
		{
			SetTextures(mesh->material, skinnedShader);
		}

		VertexArray* VertexArray = mesh->vertexArray;

		VertexArray->UpdateBuffer(6, transforms, numTransforms * sizeof(mat4));
		skinnedShader->SetUniformMat4("matModelInvTrans", transformsInvT[0]);
		//VertexArray->UpdateBuffer(7, transformsInvT, numTransforms * sizeof(mat4));

		mRenderDevice->Draw(renderTarget->GetId(), skinnedShader->GetId(), VertexArray->GetId(),
			mesh->drawParams, 1, VertexArray->GetNumIndices());

		it->second.clear();
	}
}

void EditorRenderContext::DrawImpostors(RenderTarget* renderTarget)
{
	for (std::pair<MeshInfo*, Transform>& curItem : ImpostorBuffer)
	{
		Shader* shader = ShaderManager::GetPBRShader("IMPOSTOR_SHADER");

		shader->SetSampler("diffuse", *curItem.first->material->textures[TEXTURE_ALBEDO], *sampler, 0);
		shader->SetUniform3f("uImpostorPos", curItem.second.position.ToVec());
		shader->SetUniform3f("uImpostorInfo", vec3(curItem.second.scale.ToVec().x, curItem.second.scale.ToVec().y, 1));

		mRenderDevice->Draw(renderTarget->GetId(), shader->GetId(),
			curItem.first->vertexArray->GetId(), impostorDrawParams, 1, curItem.first->vertexArray->GetNumIndices());
	}

	ImpostorBuffer.clear();
}

void EditorRenderContext::DrawEditorHelpers(RenderTarget* renderTarget)
{
	if (!bDrawGrid) { return; }
    NxArray<mat4> transforms;
    transforms.push_back(editorGridTransform.ToMatrix());

	editorGridVA->UpdateBuffer(4, &transforms[0], sizeof(mat4));

	mRenderDevice->Draw(renderTarget->GetId(), editorGridVA->GetShader()->GetId(),
		editorGridVA->GetId(), editorGridDrawParams, 1, editorGridVA->GetNumIndices());
}

void EditorRenderContext::DrawDebugShapes(RenderTarget* renderTarget)
{
	Texture* currentTexture = nullptr;
	for (NxMap<DebugShape*, NxArray<mat4>>::iterator it
		= debugShapeBuffer.begin(); it != debugShapeBuffer.end(); ++it)
	{
		DebugShape* shapeToDraw = it->first;

		VertexArray* VertexArray = shapeToDraw->vertexArray;
		Texture* texture = shapeToDraw->texture;
		DrawParams shapeDrawParams = shapeToDraw->drawParams;
		mat4* transforms = &it->second[0];
		size_t numTransforms = it->second.size();

		if (numTransforms == 0)
		{
			continue;
		}

		Shader& modelShader = *VertexArray->GetShader();

		if (texture != currentTexture)
		{
			modelShader.SetSampler("diffuse", *texture, *sampler, 0);
		}
		
		
		if (shapeToDraw->bDrawAsArrays)
		{
			VertexArray->UpdateBuffer(2, transforms, numTransforms * sizeof(mat4));
			mRenderDevice->DrawArrays(renderTarget->GetId(), modelShader.GetId(), VertexArray->GetId(),
				shapeDrawParams, VertexArray->GetNumVertices());
		}
		else
		{
			VertexArray->UpdateBuffer(4, transforms, numTransforms * sizeof(mat4));
			mRenderDevice->Draw(renderTarget->GetId(), modelShader.GetId(), VertexArray->GetId(),
				shapeDrawParams, numTransforms, VertexArray->GetNumIndices());
		}

	}
	debugShapeBuffer.clear();
}


void EditorRenderContext::SetTextures(Material* material, Shader* shader)
{
	uint32 samplerUnit = 1;

	//Albedo
	
	Texture* albedoTexture = MapFuncs::Find(material->textures, NString(TEXTURE_ALBEDO));
	if (albedoTexture != nullptr)
	{
		shader->SetSampler("albedoMap", *albedoTexture, *sampler, samplerUnit);
		shader->SetUniform1f("bUseAlbedoMap", true);
	}
	else
	{
		//TODO: Change That
		shader->SetUniform4f("uColorAlpha", material->color.ToVec());
		shader->SetUniform1f("bUseAlbedoMap", false);
	}
	//Normal
	Texture* normalTexture = MapFuncs::Find(material->textures, NString(TEXTURE_NORMAL));
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
	Texture* MRTexture = MapFuncs::Find(material->textures, NString(TEXTURE_MR));
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
		Texture* metallicTexture = MapFuncs::Find(material->textures, NString(TEXTURE_METALLIC));
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
		Texture* roughnessTexture = MapFuncs::Find(material->textures, NString(TEXTURE_ROUGHNESS));
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
		Texture* AoTexture = MapFuncs::Find(material->textures, NString(TEXTURE_AO));
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
