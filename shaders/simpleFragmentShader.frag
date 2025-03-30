#version 450

layout (location = 0) in vec3 fragColor;
layout (location = 1) in vec3 fragPositionWorldSpace;
layout (location = 2) in vec3 fragNormalWorldSpace;

layout (location = 0) out vec4 outColor;

layout (set = 0, binding = 0) uniform GlobalUbo
{
	mat4 projection;
	mat4 view;
	vec4 ambientColorLight; //w is the intensity
	vec3 lightPosition;
	vec4 lightColor; //w is the intensity
	//vec3 directionToLight;
} ubo;

layout (push_constant) uniform Push {
	mat4 modelMatrix;
	mat4 normalMatrix;
} push;

void main()
{
	vec3 directionToLight = ubo.lightPosition - fragPositionWorldSpace;

	float attenuation = 1 / (dot(directionToLight, directionToLight));

	vec3 lightColor = ubo.lightColor.xyz * ubo.lightColor.w * attenuation;
	vec3 ambientLight = ubo.ambientColorLight.xyz * ubo.ambientColorLight.w;
	vec3 diffuseLight = lightColor * max(dot(normalize(fragNormalWorldSpace), normalize(directionToLight)), 0);

	outColor = vec4((diffuseLight + ambientLight) * fragColor, 1.0);
}