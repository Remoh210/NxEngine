
#if defined(VS_BUILD)

uniform vec3 test;

out vec2 texCoord0;

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;
layout (location = 4) in mat4 transformMat;


void main()
{
    gl_Position = transformMat * vec4(position, 1.0) ;
    texCoord0 = texCoord;
}

#elif defined(FS_BUILD)
uniform sampler2D diffuse;
uniform bool bUseTexture;
uniform vec4 colorAlpha;
in vec2 texCoord0;

layout (location = 0) out vec4 FinalColor;
void main()
{

	if(bUseTexture)
	{
		FinalColor = texture(diffuse, texCoord0);
	}
	else
	{
		FinalColor = colorAlpha;
	}    

}
#endif
