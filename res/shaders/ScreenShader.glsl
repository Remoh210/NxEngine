#if defined(VS_BUILD)

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;

out vec3 WorldPos;
out vec2 UV;

void main()
{
    WorldPos = position;
    UV = texCoord;
    gl_Position =  vec4(position.x, position.y, 0.0, 1.0);
}

#elif defined(FS_BUILD)

in vec2 UV;
in vec3 WorldPos;

uniform sampler2D Texture;

out vec4 FragColor;

void main()
{		

    FragColor = texture(Texture, UV);
}
#endif
