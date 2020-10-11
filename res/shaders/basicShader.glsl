
#if defined(VS_BUILD)

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
in vec2 texCoord0;

layout (location = 0) out vec4 FinalColor;
void main()
{
    FinalColor = texture(diffuse, texCoord0);

}
#endif
