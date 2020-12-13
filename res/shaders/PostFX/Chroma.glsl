#if defined(VS_BUILD)

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;

out vec2 UV;

void main()
{
    UV = texCoord;
    gl_Position =  vec4(position.x, position.y, 0.0, 1.0);
}

#elif defined(FS_BUILD)

in vec2 UV;
in vec3 WorldPos;

uniform sampler2D Texture;
//uniform float Time;

// Above fragment shader.
uniform float     uStrength;
uniform float     uSize;
uniform float     uFalloff;


out vec4 FragColor;

void main()
{		
	// float amount = 0.0;
	
	// amount = (1.0 + sin(Time*6.0)) * 0.5;
	// amount *= 1.0 + sin(Time*2.0) * 0.5;
	// amount *= 1.0 + sin(Time*9.0) * 0.5;
	// amount *= 1.0 + sin(Time*8.0) * 0.5;

	// //amount *= 1.0 + sin(Time*16.0) * 0.5;
	// //amount *= 1.0 + sin(Time*19.0) * 0.5;
	// //amount *= 1.0 + sin(Time*27.0) * 0.5;
	// amount = pow(amount, 3.0);

	// amount *= 0.05;
	
    // vec3 col;
    // col.r = texture( Texture, vec2(UV.x+amount,UV.y) ).r;
    // col.g = texture( Texture, UV ).g;
    // col.b = texture( Texture, vec2(UV.x-amount,UV.y) ).b;

	// col *= (1.0 - amount * 0.5);

	vec2 fromCenter = UV - vec2(0.5f, 0.5f);
	float dist = length(fromCenter);
	float vignette = 1.0f - smoothstep(uSize, uSize - uFalloff, dist);
	vec2 rOffset = fromCenter * vignette * uStrength;
	float r = texture(Texture, UV + rOffset).r;
	float g = texture(Texture, UV).g;
	vec2 bOffset = -rOffset;
	float b = texture(Texture, UV + bOffset).b;

    FragColor = vec4(r, g, b, 1.0f);
}
#endif
