#ifdef GL_ES
    precision highp int;
    precision highp float;
#endif
layout(location = 0) in vec3 aPos;

out vec3 UV0;
out vec3 WorldPos;

uniform mat4 projection;
uniform mat4 view;

void main() {
    WorldPos = aPos * 1000.0;
    UV0 = aPos;
    vec4 pos = projection * view * vec4(WorldPos, 1.0);
    gl_Position = pos.xyww;
}
