#if defined(VS_BUILD)


//In
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec3 color;
layout (location = 4) in mat4 transformMat;

out vec2 TexCoords;
out vec3 WorldPos;
out vec3 Normal;


layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
};

void main()
{
    TexCoords = texCoord;
    WorldPos = vec3(transformMat * vec4(position, 1.0));
    Normal = mat3(transformMat) * normal;   
	
    gl_Position =  projection * view * vec4(WorldPos, 1.0);
}


#elif defined(FS_BUILD)
out vec4 FragColor;
in vec2 TexCoords;
in vec3 WorldPos;
in vec3 Normal;

//Camera
uniform vec3 uCamPos;

//Global lighting uniforms
uniform float uAmbient;

// material parameters
uniform vec4  uColorAlpha;
uniform float uMetallic;
uniform float uRoughness;
uniform float uAo;
uniform int   uPBRTexType;

//Material parameters bu textures
uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D metallicMap;
uniform sampler2D roughnessMap;
uniform sampler2D aoMap;

// IBL
uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;
uniform sampler2D brdfLUT;

//Material parameters bu textures
uniform bool bUseAlbedoMap;
uniform bool bUseNormalMap;
uniform bool bUseMetallicMap;
uniform bool bUseRoughnessMap;
uniform bool bUseAoMap;

// lights
const int MAX_LIGHTS = 30;
uniform int  uNumLights;
uniform int  lightTypes[MAX_LIGHTS];
uniform vec3 lightPositions[MAX_LIGHTS];
uniform vec3 lightDirections[MAX_LIGHTS];
uniform vec3 lightColors[MAX_LIGHTS];

const float PI = 3.14159265359;

// ----------------------------------------------------------------------------
// Easy trick to get tangent-normals to world-space to keep PBR code simplified.
// Don't worry if you don't get what's going on; you generally want to do normal 
// mapping the usual way for performance anways; I do plan make a note of this 
// technique somewhere later in the normal mapping tutorial.
vec3 getNormalFromMap()
{
    vec3 tangentNormal = texture(normalMap, TexCoords).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(WorldPos);
    vec3 Q2  = dFdy(WorldPos);
    vec2 st1 = dFdx(TexCoords);
    vec2 st2 = dFdy(TexCoords);

    vec3 N   = normalize(Normal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}
// ----------------------------------------------------------------------------
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / max(denom,  0.00000001); // prevent divide by zero for roughness=0.0 and NdotH=1.0
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
} 
// ----------------------------------------------------------------------------
void main()
{		
    
    vec3 V = normalize(uCamPos - WorldPos);
	vec3 N;
	vec3 albedo = uColorAlpha.rgb;
    float metallic = uMetallic;
    float roughness = uRoughness;
    float ao = uAo;
	
	
	if(bUseAlbedoMap){ albedo = pow(texture(albedoMap, TexCoords).rgb, vec3(2.2));}
	
	if(bUseNormalMap){N = getNormalFromMap();}
	else{N = normalize(Normal);}
	vec3 R = reflect(-V, N);
	
	if(uPBRTexType == 0)
	{
		if(bUseMetallicMap) { metallic  = texture(metallicMap, TexCoords).r;}
		if(bUseRoughnessMap){roughness = texture(roughnessMap, TexCoords).r;}
		if(bUseAoMap)		{ao = texture(aoMap, TexCoords).r;}
	}
	//glTF PBR texture
	else if(uPBRTexType == 1)
	{
		if(bUseMetallicMap)
		{ 
			metallic  = texture(metallicMap, TexCoords).b;
			roughness = texture(metallicMap, TexCoords).g;
		}
	}
	//UE4 PBR texture
	else if(uPBRTexType == 2)
	{
		if(bUseMetallicMap || bUseRoughnessMap || bUseAoMap)
		{ 
			metallic  = texture(metallicMap, TexCoords).b;
			roughness = texture(metallicMap, TexCoords).g;
			ao = texture(aoMap, TexCoords).b;
		}
	}


    // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0 
    // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)    
    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);

    // reflectance equation
    vec3 Lo = vec3(0.0);
    for(int i = 0; i < uNumLights; ++i) 
    {
		int lightType = lightTypes[i];
		if(lightType == 0)
		{
			vec3 L = normalize(-lightDirections[i]);
			vec3 H = normalize(V + L);
			
			// calculate per-light radiance
			float radScale = max(dot(N, L), 0.0);
			vec3 radiance = lightColors[i] * radScale;
	
			// Cook-Torrance BRDF
			float NDF = DistributionGGX(N, H, roughness);   
			float G   = GeometrySmith(N, V, L, roughness);      
			vec3 F    = fresnelSchlick(clamp(dot(H, V), 0.0, 1.0), F0);
			
			vec3 nominator    = NDF * G * F; 
			float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
			vec3 specular = nominator / max(denominator, 0.001); // prevent divide by zero for NdotV=0.0 or NdotL=0.0
			
			// kS is equal to Fresnel
			vec3 kS = F;
			// for energy conservation, the diffuse and specular light can't
			// be above 1.0 (unless the surface emits light); to preserve this
			// relationship the diffuse component (kD) should equal 1.0 - kS.
			vec3 kD = vec3(1.0) - kS;
			// multiply kD by the inverse metalness such that only non-metals 
			// have diffuse lighting, or a linear blend if partly metal (pure metals
			// have no diffuse light).
			kD *= 1.0 - metallic;	  
	
			// scale light by NdotL
			float NdotL = max(dot(N, L), 0.0);        
	
			// add to outgoing radiance Lo
			// we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
			Lo += (kD * albedo / PI + specular) * radiance * NdotL;  
		}
		else if(lightType == 1)
		{
		     // calculate per-light radiance
			vec3 L = normalize(lightPositions[i] - WorldPos);
			vec3 H = normalize(V + L);
			float distance = length(lightPositions[i] - WorldPos);
			float attenuation = 1.0 / (distance * distance);
			vec3 radiance = lightColors[i] * attenuation;
	
			// Cook-Torrance BRDF
			float NDF = DistributionGGX(N, H, roughness);   
			float G   = GeometrySmith(N, V, L, roughness);      
			vec3 F    = fresnelSchlick(clamp(dot(H, V), 0.0, 1.0), F0);
			
			vec3 nominator    = NDF * G * F; 
			float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
			vec3 specular = nominator / max(denominator, 0.001); // prevent divide by zero for NdotV=0.0 or NdotL=0.0
			
			// kS is equal to Fresnel
			vec3 kS = F;
			// for energy conservation, the diffuse and specular light can't
			// be above 1.0 (unless the surface emits light); to preserve this
			// relationship the diffuse component (kD) should equal 1.0 - kS.
			vec3 kD = vec3(1.0) - kS;
			// multiply kD by the inverse metalness such that only non-metals 
			// have diffuse lighting, or a linear blend if partly metal (pure metals
			// have no diffuse light).
			kD *= 1.0 - metallic;	  
	
			// scale light by NdotL
			float NdotL = max(dot(N, L), 0.0);        
	
			// add to outgoing radiance Lo
			// we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
			Lo += (kD * albedo / PI + specular) * radiance * NdotL;  
		}

    }   
    
    // this ambient lighting with environment lighting).
    // ambient lighting (we now use IBL as the ambient term)
    vec3 F = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
    
    vec3 kS = F;
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - metallic;	
	
    vec3 irradiance = texture(irradianceMap, N).rgb;
    vec3 diffuse      = irradiance * albedo;
	
    // sample both the pre-filter map and the BRDF lut and combine them together as per the Split-Sum approximation to get the IBL specular part.
    const float MAX_REFLECTION_LOD = 4.0;
    vec3 prefilteredColor = textureLod(prefilterMap, R,  roughness * MAX_REFLECTION_LOD).rgb;    
    vec2 brdf  = texture(brdfLUT, vec2(max(dot(N, V), 0.0), roughness)).rg;
    vec3 specular = prefilteredColor * (F * brdf.x + brdf.y);

    vec3 ambient = (kD * diffuse + specular) * ao;
    
    vec3 color = ambient + Lo;

    // HDR tonemapping
    color = color / (color + vec3(1.0));
    // gamma correct
    color = pow(color, vec3(1.0/2.2)); 

    FragColor = vec4(color, 1);
}
#endif