#ifdef GL_ES
	precision highp float;
	precision highp int;
#endif

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aUV0;
layout (location = 3) in vec2 aUV1;
layout (location = 4) in vec4 aJoint0;
layout (location = 5) in vec4 aWeight0;

layout (set = 0, binding = 0) uniform UBO 
{
	mat4 projection;
	mat4 model;
	mat4 view;
	vec3 camPos;
} ubo;

#define MAX_NUM_JOINTS 128

layout (set = 2, binding = 0) uniform UBONode {
	mat4 matrix;
	mat4 jointMatrix[MAX_NUM_JOINTS];
	float jointCount;
} node;


layout (location = 0) out vec3 WorldPos;
layout (location = 1) out vec3 Normal;
layout (location = 2) out vec2 UV0;
layout (location = 3) out vec2 UV1;

void main() 
{
    
    outUV0 = inUV0;
	outUV1 = inUV1;
    gl_Position =  ubo.projection * ubo.view * ubo.model * vec4(aPos, 1.0);
}





