#ifdef GL_ES
	precision highp float;
	precision highp int;
#endif

layout (location = 0) in vec3 aPos;
layout (location = 2) in vec2 aUV0;
layout (location = 3) in vec2 aUV1;

uniform mat4 model;
uniform mat4 proj_view;

out vec3 WorldPos;
out vec2 UV0;
out vec2 UV1;

void main() 
{
    UV0 = aUV0;
    UV1 = aUV1;
    WorldPos = vec3(model * vec4(aPos, 1.0)); 

    gl_Position =  proj_view * vec4(WorldPos, 1.0);
}
