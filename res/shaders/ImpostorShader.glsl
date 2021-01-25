
#if defined(VS_BUILD)

uniform vec3 test;

out vec2 texCoord0;

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;

uniform vec3 uImpostorPos; // Position of the center of the billboard
uniform vec3 uImpostorInfo; //x,y - size, z - axis (0 - all, 1 - up)

layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
};


void main()
{
	
	
	float axis = uImpostorInfo.z;
	vec2 impostorSize = uImpostorInfo.xy;
	
	vec3 CameraRight_worldspace;
	vec3 CameraUp_worldspace;
	
	CameraRight_worldspace = vec3(view[0][0], view[1][0], view[2][0]);
	
	if(axis == 1)
	{
		CameraUp_worldspace = vec3(0, 1, 0);
	}
	else
	{
		CameraUp_worldspace = vec3(view[0][1], view[1][1], view[2][1]);
	}

	vec3 center = uImpostorPos;
	
	vec3 vertexPosition_worldspace =
    center
    + CameraRight_worldspace * position.x * impostorSize.x
    + CameraUp_worldspace * position.y * impostorSize.y;
	
	gl_Position = projection * view* vec4(vertexPosition_worldspace, 1.0f);
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
