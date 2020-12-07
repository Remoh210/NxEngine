#if defined(VS_BUILD)

//In
layout (location = 0) in vec3 position;
out vec3 WorldPos;


layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
};

void main()
{
    WorldPos = position;

	mat4 rotView = mat4(mat3(view));
	vec4 clipPos = projection * rotView * vec4(WorldPos, 1.0);

	gl_Position = clipPos.xyww;
}


#elif defined(FS_BUILD)

out vec4 FragColor;
in vec3 WorldPos;

uniform samplerCube environmentMap;

void main()
{		
    vec3 envColor = texture(environmentMap, WorldPos, 3.9).rgb;
    
    // HDR tonemap and gamma correct
    envColor = envColor / (envColor + vec3(1.0));
    envColor = pow(envColor, vec3(1.0/2.2)); 
    
    FragColor = vec4(envColor, 1.0);
}
#endif
