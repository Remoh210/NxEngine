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

	// lights
	// ------
	Array<glm::vec3> lightPositions;
	lightPositions.push_back(glm::vec3(-10.0f, 10.0f, 10.0f));
	lightPositions.push_back(glm::vec3(10.0f, 10.0f, 10.0f));
	lightPositions.push_back(glm::vec3(-10.0f, -10.0f, 10.0f));
	lightPositions.push_back(glm::vec3(10.0f, -10.0f, 10.0f));

	
	Array<glm::vec3> lightColors;
	lightColors.push_back(glm::vec3(3000.0f, 3000.0f, 3000.0f));
	lightColors.push_back(glm::vec3(3000.0f, 3000.0f, 3000.0f));
	lightColors.push_back(glm::vec3(3000.0f, 3000.0f, 3000.0f));
	lightColors.push_back(glm::vec3(3000.0f, 3000.0f, 3000.0f));



	// render light source (simply re-render sphere at light positions)
	// this looks a bit off as we use the same shader, but it'll make their positions obvious and 
	// keeps the codeprint small.

	//glm::vec3 newPos = lightPositions[1] + glm::vec3(sin(glfwGetTime() * 5.0) * 5.0, 0.0, 0.0);
	//newPos = lightPositions[1];
	//ShaderManager::GetMainShader()->SetUniform3f("lightPositions[0]", newPos);
	//ShaderManager::GetMainShader()->SetUniform3f("lightColors[0]", lightColors[1]);


	//glm::vec3 newPos = lightPositions[i] + glm::vec3(sin(glfwGetTime() * 5.0) * 5.0, 0.0, 0.0);
	//newPos = lightPositions[i];
	ShaderManager::GetMainShader()->SetUniform3fv("lightPositions[0]", lightPositions);
	ShaderManager::GetMainShader()->SetUniform3fv("lightColors[0]", lightColors);


	//for (unsigned int i = 0; i < sizeof(lightPositions) / sizeof(lightPositions[0]); ++i)
	//{
	//	glm::vec3 newPos = lightPositions[i] + glm::vec3(sin(glfwGetTime() * 5.0) * 5.0, 0.0, 0.0);
	//	newPos = lightPositions[i];
	//	ShaderManager::GetMainShader()->SetUniform3f("lightPositions[" + std::to_string(i) + "]", newPos);
	//	ShaderManager::GetMainShader()->SetUniform3f("lightColors[" + std::to_string(i) + "]", lightColors[i]);

	//	//model = glm::mat4(1.0f);
	//	//model = glm::translate(model, newPos);
	//	//model = glm::scale(model, glm::vec3(0.5f));
	//	//shader.setMat4("model", model);
	//	//renderSphere();
	//}

	//Draw Editor stuff first
	DrawEditorHelpers();
	DrawDebugShapes();

    for(Map<std::pair<Array<MeshInfo*>, Shader*>, Array<mat4> >::iterator it
            = meshRenderBuffer.begin(); it != meshRenderBuffer.end(); ++it)
    {
		//if (numTransforms == 0)
		//{
		//	continue;
		//}

		mat4* transforms = &it->second[0];
		size_t numTransforms = it->second.size();
		//Shader* modelShader = it->first.second;
		Shader * modelShader = it->first.second;
		modelShader->SetUniformBuffer("Matrices", *MatrixUniformBuffer);

		for (MeshInfo* mesh : it->first.first)
		{
            
			VertexArray* vertexArray = mesh->vertexArray;

			if (mesh->material->diffuseTextures.size() > 0)
			{
				Texture* texture = mesh->material->diffuseTextures[0];
				//modelShader->SetSampler("diffuse", *texture, *sampler, 0);
				//modelShader->SetUniform1f("bTexUse", true);
			}
			else
			{
				//modelShader->SetUniform1i("bTexUse", false);
				modelShader->SetUniform4f("colorAlpha", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
			}

			//PBR Stuff
			//modelShader->SetUniform1f("metallic", mesh->material->metallic);
			//modelShader->SetUniform1f("roughness", mesh->material->roughness);
			//modelShader->SetUniform1f("ao", mesh->material->ambientOcclusion);

			modelShader->SetUniform1f("metallic", 0.7f);
			modelShader->SetUniform1f("roughness", 0.3f);
			modelShader->SetUniform1f("ao", 0.2f);
			
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

	int i = 0;
}
