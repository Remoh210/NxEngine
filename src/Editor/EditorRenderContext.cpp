#include "EditorRenderContext.h"
#include "rendering/VertexArray.h"
#include "Core/Application/Application.h"
#include "Core/FileSystem/FileSystem.h"


EditorRenderContext::EditorRenderContext(RenderDevice& deviceIn, RenderTarget& targetIn, DrawParams& drawParamsIn,
	        Sampler& samplerIn, const mat4& perspectiveIn, Camera* CameraIn)
            : RenderContext(deviceIn, targetIn),
		    drawParams(drawParamsIn),
			sampler(samplerIn),
			perspective(perspectiveIn),
			mainCamera(CameraIn)
{
	 editorGridSlices = 10;
	 editorGridScale = 20;

	// editorGridDrawParams.primitiveType = PRIMITIVE_LINES;
	// editorGridDrawParams.shouldWriteDepth = true;
	// editorGridDrawParams.depthFunc = DRAW_FUNC_LESS;
	// editorGridVA = new VertexArray(deviceIn,PrimitiveGenerator::CreateGridVA(editorGridSlices, vec3(0.3f)), BufferUsage::USAGE_DYNAMIC_DRAW);
    //             //Load and set shaders
    // String LINE_SHADER_TEXT_FILE = Nx::FileSystem::GetPath("res/shaders/LineShader.glsl");
    // String LineShaderText;
    // Application::loadTextFileWithIncludes(LineShaderText, LINE_SHADER_TEXT_FILE, "#include");
    // Shader* grid_shader = new Shader(deviceIn, LineShaderText);
	// editorGridVA->SetShader(grid_shader);

	// editorGridTransform.scale = vec3(editorGridScale);
	// editorGridTransform.position.x = -0.5 * editorGridScale;
	// editorGridTransform.position.z = -0.5 * editorGridScale;

	//Infinite grid
	editorGridDrawParams.primitiveType = PRIMITIVE_TRIANGLES;
	editorGridDrawParams.shouldWriteDepth = true;
	editorGridDrawParams.depthFunc = DRAW_FUNC_LESS;
	String GrigShaderPath = Nx::FileSystem::GetPath("res/shaders/EditorGridShader.glsl");
    String GrigShaderText;
    Application::loadTextFileWithIncludes(GrigShaderText, GrigShaderPath, "#include");
	Shader* GRIDshader = new Shader(deviceIn, GrigShaderText);
	MatrixUniformBuffer = new UniformBuffer(deviceIn, 2, sizeof(mat4), BufferUsage::USAGE_STATIC_DRAW);
	
	
	//MatrixUniformBuffer->Update(&perspectiveIn[0], sizeof(glm::mat4));

	//MatrixUniformBuffer


    editorGridVA = new VertexArray(deviceIn, PrimitiveGenerator::CreateQuad(), BufferUsage::USAGE_STATIC_DRAW);
	editorGridVA->SetShader(GRIDshader);

	//editorGridTransform.position.x = -0.5 * editorGridScale;
    //editorGridTransform.position.z = -0.5 * editorGridScale;
	editorGridTransform.scale = vec3(editorGridScale);

	MatrixUniformBuffer->Update(glm::value_ptr(perspective), sizeof(glm::mat4), 0);
	
}

void EditorRenderContext::Flush()
{
	//Draw Editor stuff first
	DrawEditorHelpers();

	DrawDebugShapes();

    Texture* currentTexture = nullptr;
    for(Map<std::pair<VertexArray*, Texture*>, Array<mat4> >::iterator it
            = meshRenderBuffer.begin(); it != meshRenderBuffer.end(); ++it)
    {
        
        VertexArray* vertexArray = it->first.first;
        Texture* texture = it->first.second;
        mat4* transforms = &it->second[0];
        size_t numTransforms = it->second.size();

        if(numTransforms == 0)
        {
            continue;
        }

        Shader& modelShader = *vertexArray->GetShader();

        if(texture != currentTexture)
        {
            modelShader.SetSampler("diffuse", *texture, sampler, 0);
        }
        vertexArray->UpdateBuffer(4, transforms, numTransforms*sizeof(mat4));
		if (vertexArray->GetNumIndices() == 0)
		{
			mRenderDevice->DrawArrays(mRenderTarget->GetId(), modelShader.GetId(), vertexArray->GetId(),
				drawParams, 300);
		}
		else
		{
			Draw(modelShader, *vertexArray, drawParams, numTransforms);
		}
        
        it->second.clear();
    }
}

void EditorRenderContext::DrawEditorHelpers()
{
    Array<mat4> transforms;

	mat4 viewMatrix = mainCamera->GetViewMatrix();

	editorGridVA->GetShader()->SetUniformBuffer("Matrices", *MatrixUniformBuffer);
	
	MatrixUniformBuffer->Update(glm::value_ptr(viewMatrix), sizeof(glm::mat4), 1);

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
			modelShader.SetSampler("diffuse", *texture, sampler, 0);
		}
		vertexArray->UpdateBuffer(4, transforms, numTransforms * sizeof(mat4));

		Draw(modelShader, *vertexArray, shapeDrawParams, numTransforms);
		
	}
	debugShapeBuffer.clear();

    //HACK...
	MatrixUniformBuffer->ResetOffset();

	int i = 0;
}
