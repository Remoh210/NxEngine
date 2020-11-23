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

	//Set lights 
	ShaderManager::GetMainShader()->SetUniform3fv("lightPositions[0]", lightPosBuffer);
	ShaderManager::GetMainShader()->SetUniform3fv("lightColors[0]", lightColorBuffer);

	//Draw meshes
    for(Map<std::pair<Array<MeshInfo*>, Shader*>, Array<mat4> >::iterator it
            = meshRenderBuffer.begin(); it != meshRenderBuffer.end(); ++it)
    {
		mat4* transforms = &it->second[0];
		size_t numTransforms = it->second.size();
		//Shader* modelShader = it->first.second;
		Shader * modelShader = it->first.second;
		modelShader->SetUniformBuffer("Matrices", *MatrixUniformBuffer);

		for (MeshInfo* mesh : it->first.first)
		{
            
			VertexArray* vertexArray = mesh->vertexArray;

			uint32 samplerUnit = 0;
			//Diffuse
			Texture* DiffuseTexture = mesh->material->diffuseTexture;
			if (DiffuseTexture != nullptr)
			{
				modelShader->SetSampler("albedoMap", *DiffuseTexture, *sampler, samplerUnit);
				modelShader->SetUniform1f("bUseAlbedoMap", true);
			}
			else
			{
				//TODO: Change That
				modelShader->SetUniform4f("uColorAlpha", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
				modelShader->SetUniform1f("bUseAlbedoMap", false);
			}
			//Normal
			Texture* normalTexture = mesh->material->normalMap;
			if (normalTexture != nullptr)
			{
				samplerUnit++;
				modelShader->SetSampler("normalMap", *normalTexture, *sampler, samplerUnit);
				modelShader->SetUniform1f("bUseNormalMap", true);
			}
			else
			{
				modelShader->SetUniform1f("bUseNormalMap", false);
			}
			Texture* metallicTexture = mesh->material->metallicMap;
			if (metallicTexture != nullptr)
			{
				samplerUnit++;
				modelShader->SetSampler("metallicMap", *metallicTexture, *sampler, samplerUnit);
				modelShader->SetUniform1f("bUseMetallicMap", true);
			}
			else
			{
				modelShader->SetUniform1f("uMetallic", 0.9f);
				modelShader->SetUniform1f("bUseMetallicMap", false);
			}
			//Roughness
			Texture* roughnessTexture = mesh->material->roughnessMap;
			if (roughnessTexture != nullptr)
			{
				samplerUnit++;
				modelShader->SetSampler("roughnessMap", *roughnessTexture, *sampler, samplerUnit);
				modelShader->SetUniform1f("bUseMetallicMap", true);
			}
			else
			{
				modelShader->SetUniform1f("bUseRoughnessMap", false);
				modelShader->SetUniform1f("uRoughness", 0.3f);
			}
			//Ao
			Texture* AoTexture = mesh->material->aoMap;
			if (AoTexture)
			{
				samplerUnit++;
				modelShader->SetSampler("aoMap", *AoTexture, *sampler, samplerUnit);
				modelShader->SetUniform1f("bUseAoMap", true);
			}
			else
			{
				modelShader->SetUniform1f("bUseAoMap", false);
				modelShader->SetUniform1f("uAo", 0.2f);
			}
			
			
			
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
	MatrixUniformBuffer->ResetOffset();
}
