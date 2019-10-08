#version 330

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;

out vec3 FragPos;
out vec3 FragNormal;
out vec2 FragUV;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normalMatrix;

void main()
{
	vec4 worldPos = projection * view * model * vec4(position, 1.0);
	FragPos = vec3(vec4(position,1.0) * model);
	//FragNormal = normalize(normalMatrix * normal);
	
	FragNormal = mat3(transpose(inverse(model))) * normal;
	FragUV = uv;

	gl_Position = worldPos;
}