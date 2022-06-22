#ifdef GL_ES
	precision highp float;
	precision highp int;
#endif

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aUV;

//uniform mat4 projection;
uniform mat4 model;
uniform mat4 proj_view;


out vec3 WorldPos;
out vec2 UV;


void main() 
{
    UV = aUV;

    WorldPos = vec3(model * vec4(aPos, 1.0));

    gl_Position =  proj_view * vec4(WorldPos, 1.0);
}
