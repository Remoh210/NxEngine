#if defined(VS_BUILD)

out vec2 texCoord0;
out vec3 VertColor;

out vec3 nearPoint;
out vec3 farPoint;

out mat4 fragView;
out mat4 fragProj;

//In
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec3 color;
layout (location = 4) in mat4 transformMat;

layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
};


vec3 UnprojectPoint(float x, float y, float z, mat4 view, mat4 projection) {
    mat4 viewInv = inverse(view);
    mat4 projInv = inverse(projection);
    vec4 unprojectedPoint =  viewInv * projInv * vec4(x, y, z, 1.0);
    return unprojectedPoint.xyz / unprojectedPoint.w;
}


void main()
{
    nearPoint = UnprojectPoint(position.x, position.y, 0.0, view, projection).xyz; // unprojecting on the near plane
    farPoint = UnprojectPoint(position.x, position.y, 1.0, view, projection).xyz; // unprojecting on the far plane
    gl_Position = vec4(position, 1.0); // using directly the clipped coordinates

    //gl_Position = projection * view * transformMat * vec4(position, 1.0);
    texCoord0 = texCoord;
	VertColor = color;

    fragView = view;
    fragProj = projection;
}

#elif defined(FS_BUILD)
uniform sampler2D diffuse;
in vec2 texCoord0;
in vec3 VertColor;

in vec3 nearPoint;
in vec3 farPoint;

in mat4 fragView;
in mat4 fragProj;

float near = 0.01;
float far = 100;


vec4 grid(vec3 fragPos3D, float scale, bool drawAxis) {
    vec2 coord = fragPos3D.xz * scale;
    vec2 derivative = fwidth(coord);
    vec2 grid = abs(fract(coord - 0.5) - 0.5) / derivative;
    float line = min(grid.x, grid.y);
    float minimumz = min(derivative.y, 1);
    float minimumx = min(derivative.x, 1);
    vec4 color = vec4(0.2, 0.2, 0.2, 1.0 - min(line, 1.0));
    // z axis
    if(fragPos3D.x > -0.1 * minimumx && fragPos3D.x < 0.1 * minimumx)
        color.z = 1.0;
    // x axis
    if(fragPos3D.z > -0.1 * minimumz && fragPos3D.z < 0.1 * minimumz)
        color.x = 1.0;
    return color;
}

float computeDepth(vec3 pos) {
    vec4 clip_space_pos = fragProj * fragView * vec4(pos.xyz, 1.0);
    return (clip_space_pos.z / clip_space_pos.w);
}
float computeLinearDepth(vec3 pos) {
    vec4 clip_space_pos = fragProj * fragView * vec4(pos.xyz, 1.0);
    float clip_space_depth = (clip_space_pos.z / clip_space_pos.w) * 2.0 - 1.0; // put back between -1 and 1
    float linearDepth = (2.0 * near * far) / (far + near - clip_space_depth * (far - near)); // get linear value between 0.01 and 100
    return linearDepth / far; // normalize
}



layout (location = 0) out vec4 FinalColor;
void main()
{
    float t = -nearPoint.y / (farPoint.y - nearPoint.y);

    vec3 fragPos3D = nearPoint + t * (farPoint - nearPoint);

    gl_FragDepth = computeDepth(fragPos3D);

    float linearDepth = computeLinearDepth(fragPos3D);
    float fading = max(0, (0.5 - linearDepth));

    FinalColor = (grid(fragPos3D, 10, true) + grid(fragPos3D, 1, true))* float(t > 0); // adding multiple resolution for the grid
    FinalColor.a *= fading;

}
#endif