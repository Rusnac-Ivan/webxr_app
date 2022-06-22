#ifdef GL_ES
    precision highp int;
    precision highp float;
#endif 
layout(location = 0) in vec3 aPos;

out vec3 FragPos;

uniform mat4 model;
uniform mat4 proj_view;

void main() {
    FragPos = vec3(model * vec4(aPos, 1.0));

    gl_Position = proj_view * vec4(FragPos, 1.0);
}
