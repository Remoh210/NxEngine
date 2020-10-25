
#if defined(VS_BUILD)

out vec2 texCoord0;
out vec3 VertColor;

layout(location = 3) in mat4 fragView;
layout(location = 7) in mat4 fragProj;


layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;
layout (location = 2) in vec3 color;
layout (location = 4) in mat4 transformMat;



void main()
{
    gl_Position = transformMat * vec4(position, 1.0) ;
    texCoord0 = texCoord;
	VertColor = color;
    fragView = 
}

#elif defined(FS_BUILD)
uniform sampler2D diffuse;
in vec2 texCoord0;
in vec3 VertColor;

layout (location = 0) out vec4 FinalColor;
void main()
{
    FinalColor = vec4(VertColor.rgb, 1);

}
#endif
