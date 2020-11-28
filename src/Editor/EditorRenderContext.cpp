#include "EditorRenderContext.h"
#include "rendering/VertexArray.h"
#include "Core/Application/Application.h"
#include "Core/FileSystem/FileSystem.h"
#include "Core/Graphics/ShaderManager/ShaderManager.h"


EditorRenderContext::EditorRenderContext(RenderDevice* deviceIn, RenderTarget* targetIn, DrawParams drawParamsIn,
	        Sampler* samplerIn, const mat4 perspectiveIn, Camera* CameraIn)
            : RenderContext(deviceIn, targetIn),
		    drawParams(drawParamsIn),
			sampler(samplerIn),
			perspective(perspectiveIn),
			mainCamera(CameraIn)
{
	//Settings
	ambient = 0.02f;

	 editorGridSlices = 200;
	 editorGridScale = 1000;

	 editorGridDrawParams.primitiveType = PRIMITIVE_LINES;
	 editorGridDrawParams.shouldWriteDepth = true;
	 editorGridDrawParams.depthFunc = DRAW_FUNC_LESS;
	 editorGridVA = new VertexArray(deviceIn,PrimitiveGenerator::CreateGridVA(editorGridSlices, vec3(0.3f)), BufferUsage::USAGE_DYNAMIC_DRAW);
                 //Load and set shaders
     NString LINE_SHADER_TEXT_FILE = "res/shaders/EditorGridSimpleShader.glsl";
     NString LineShaderText;
     Application::loadTextFileWithIncludes(LineShaderText, LINE_SHADER_TEXT_FILE, "#include");
     Shader* grid_shader = new Shader(deviceIn, LineShaderText);
	 MatrixUniformBuffer = new UniformBuffer(deviceIn, 2, sizeof(mat4), BufferUsage::USAGE_STATIC_DRAW);
	 editorGridVA->SetShader(grid_shader);
	 editorGridVA->GetShader()->SetUniformBuffer("Matrices", *MatrixUniformBuffer);

	 editorGridTransform.scale = vec3(editorGridScale);
	 editorGridTransform.position.x = -0.5 * editorGridScale;
	 editorGridTransform.position.z = -0.5 * editorGridScale;

	 MatrixUniformBuffer->Update(glm::value_ptr(perspective), sizeof(glm::mat4), 0);


	 Shader* mainShader = ShaderManager::GetMainShader();
	 //mainShader->SetUniformBuffer("Matrices", *MatrixUniformBuffer);
	
}

void EditorRenderContext::Flush()
{
	mat4 viewMatrix = mainCamera->GetViewMatrix();
	MatrixUniformBuffer->Update(glm::value_ptr(viewMatrix), sizeof(glm::mat4), 1);

	//Draw Editor stuff first
	DrawEditorHelpers();
	DrawDebugShapes();

	//TODO: Set as struct in glsl
	//Set lights
	Array<int> lightTypes;
	Array<vec3> lightColors;
	Array<vec3> lightPositions;
	Array<vec3> lightDirections;

	int numLights = lightBuffer.size();
	ShaderManager::GetMainShader()->SetUniform1i("uNumLights", numLights < 50 ? numLights : 50);
	
	for (std::pair<ECS::ComponentHandle<LightComponent>, vec3> light : lightBuffer)
	{
		lightTypes.push_back(light.first->lightType);
		lightColors.push_back(light.first->color * light.first->intensity);
		lightDirections.push_back(light.first->direction);
		lightPositions.push_back(light.second);	
	}

	ShaderManager::GetMainShader()->SetUniform1iv("lightTypes[0]", lightTypes);
	ShaderManager::GetMainShader()->SetUniform3fv("lightColors[0]", lightColors);
	ShaderManager::GetMainShader()->SetUniform3fv("lightPositions[0]", lightPositions);
	ShaderManager::GetMainShader()->SetUniform3fv("lightDirections[0]", lightDirections);

	ShaderManager::GetMainShader()->SetUniform1f("uAmbient", ambient);
	ShaderManager::GetMainShader()->SetUniform3f("uCamPos", mainCamera->Position);

	//Draw meshes
    for(Map<std::pair<Array<MeshInfo*>, Shader*>, Array<mat4> >::iterator it
            = meshRenderBuffer.begin(); it != meshRenderBuffer.end(); ++it)
    {
		mat4* transforms = &it->second[0];
		size_t numTransforms = it->second.size();
		//Shader* modelShader = it->first.second;
		Shader * modelShader = it->first.second;
		modelShader->SetUniformBuffer("Matrices", *MatrixUniformBuffer);

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
				mRenderDevice->DrawArrays(mRenderTarget->GetId(), modelShader->GetId(), vertexArray->GetId(),
					drawParams, 300);
			}
			else
			{
				Draw(*modelShader, *vertexArray, drawParams, numTransforms);
			}

			it->second.clear();
		}
    }

	//HACK...
	MatrixUniformBuffer->ResetOffset();

	lightBuffer.clear();
	meshRenderBuffer.clear();
}

void EditorRenderContext::DrawEditorHelpers()
{
    Array<mat4> transforms;

    transforms.push_back(editorGridTransform.ToMatrix());

	editorGridVA->UpdateBuffer(4, &transforms[0], sizeof(mat4));

    Draw(*editorGridVA->GetShader(), *editorGridVA, editorGridDrawParams, 1);

	
}

void EditorRenderContext::DrawDebugShapes()
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

		Draw(modelShader, *vertexArray, shapeDrawParams, numTransforms);
		
	}
	debugShapeBuffer.clear();

    //HACK...
	//MatrixUniformBuffer->ResetOffset();
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