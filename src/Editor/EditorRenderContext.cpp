#include "EditorRenderContext.h"
#include "rendering/VertexArray.h"

void EditorRenderContext::Flush()
{
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
			this->Draw(modelShader, *vertexArray, drawParams, numTransforms);
		}
        

        it->second.clear();
    }
}
