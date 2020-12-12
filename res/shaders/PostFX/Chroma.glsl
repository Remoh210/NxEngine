#if defined(VS_BUILD)

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;

out vec3 WorldPos;
out vec2 UV;

uniform  mat4 projection;
uniform  mat4 view;


void main()
{
    WorldPos = position;
    UV = texCoord;
    gl_Position =  projection * view * vec4(WorldPos, 1.0);
}

#elif defined(FS_BUILD)

in vec2 UV;
in vec3 WorldPos;

uniform sampler2D Texture;
uniform float Time;


out vec4 FragColor;

void main()
{		
	float amount = 0.0;
	
	amount = (1.0 + sin(Time*6.0)) * 0.5;
	amount *= 1.0 + sin(Time*16.0) * 0.5;
	amount *= 1.0 + sin(Time*19.0) * 0.5;
	amount *= 1.0 + sin(Time*27.0) * 0.5;
	amount = pow(amount, 3.0);

	amount *= 0.05;
	
    vec3 col;
    col.r = texture( Texture, vec2(UV.x+amount,UV.y) ).r;
    col.g = texture( Texture, UV ).g;
    col.b = texture( Texture, vec2(UV.x-amount,UV.y) ).b;

	col *= (1.0 - amount * 0.5);
	
    FragColor = vec4(col,1.0);
}
#endif
