
#if defined(VS_BUILD)

uniform vec3 test;

out vec2 texCoord0;

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;
layout (location = 4) in mat4 transformMat;

layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
};


void main()
{
	gl_Position = projection * view * transformMat * vec4(position, 1.0) ;
    texCoord0 = texCoord;
}

#elif defined(FS_BUILD)
uniform sampler2D diffuse;
uniform vec4 colorAlpha;
uniform bool bTexUse;
in vec2 texCoord0;

layout (location = 0) out vec4 FinalColor;
void main()
{	
	if(bTexUse)
	{
		FinalColor = texture(diffuse, texCoord0);
	}
	else
	{
		FinalColor = colorAlpha;
	}
}
#endif
