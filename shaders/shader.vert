#version 450 core
layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;

layout(set = 0 , binding = 0) uniform ObjectBuffer {
    mat4 model;
}ubo;

layout(set = 0 , binding = 1) uniform CameraBuffer {
    mat4 view;
    mat4 proj;
	mat4 porjView;
}cmo;

void main()
{

    gl_Position = cmo.proj * cmo.view * ubo.model * vec4(inPos, 1.0);
	fragColor = inColor;
	fragTexCoord = inTexCoord;
}