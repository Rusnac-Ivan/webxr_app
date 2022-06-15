#ifdef GL_ES
	precision highp float;
	precision highp int;
#endif

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aUV0;


uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;
uniform vec3 camPos;


layout (location = 0) out vec3 WorldPos;
layout (location = 1) out vec3 Normal;
layout (location = 2) out vec2 UV0;

void main() 
{
    UV0 = aUV0;
    WorldPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(model) * aNormal;   

    gl_Position =  projection * view * vec4(WorldPos, 1.0);
}





