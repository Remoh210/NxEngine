#include "EditorRenderContext.h"
#include "rendering/VertexArray.h"
#include "Common/Transform/Transform.h"
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
	GridDrawParams.primitiveType = PRIMITIVE_LINES;
	EditorGrid = new VertexArray(deviceIn,LineRenderer::CreateGridVA(40), BufferUsage::USAGE_DYNAMIC_DRAW);
                //Load and set shaders
    String LINE_SHADER_TEXT_FILE = Nx::FileSystem::GetPath("res/shaders/LineShader.glsl");
    String LineShaderText;
    Application::loadTextFileWithIncludes(LineShaderText, LINE_SHADER_TEXT_FILE, "#include");
    Shader* grid_shader = new Shader(deviceIn, LineShaderText);
    EditorGrid->SetShader(grid_shader);
			//Create grid#inVertexA#include#includerrayclude#include
}

void EditorRenderContext::Flush()
{
    Texture* currentTexture = nullptr;
    for(Map<std::pair<VertexArray*, Texture*>, Array<mat4> >::iterator it
            = meshRenderBuffer.begin(); it != meshRenderBuffer.end(); ++it)
    {
        //Draw Editor stuff 0first
        DrawEditorHelpers();
        


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
    Transform gridTransform;
    gridTransform.scale = vec3(200);
    Array<mat4> transforms;
    transforms.push_back(perspective * mainCamera->GetViewMatrix() * gridTransform.ToMatrix());
    EditorGrid->UpdateBuffer(4, &transforms[0], sizeof(mat4));
    Draw(*EditorGrid->GetShader(), *EditorGrid, GridDrawParams, 1);
}
