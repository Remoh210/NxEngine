#pragma once
#include <cstdint>
#include <string>
#include <glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <gtx/quaternion.hpp>
#include <gtc/type_ptr.hpp>

#define TEXTURE_ALBEDO    "texture_albedo"
#define TEXTURE_NORMAL    "texture_normal"
#define TEXTURE_METALLIC  "texture_metallic"
#define TEXTURE_ROUGHNESS "texture_roughness"
#define TEXTURE_AO		  "texture_ao"
#define TEXTURE_SPECULAR   "texture_specular"

#define PBR_TEXTURE_SPLIT 0
#define PBR_TEXTURE_MR 1
#define PBR_TEXTURE_OMR 2

//glTF
#define TEXTURE_MR  "texture_metallic_roughness"

typedef glm::vec2 vec2;
typedef glm::vec3 vec3;
typedef glm::vec4 vec4;
typedef glm::quat quat;
typedef glm::mat4 mat4;
typedef glm::mat3 mat3;





