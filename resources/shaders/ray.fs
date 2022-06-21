#ifdef GL_ES
    precision highp int;
    precision highp float;
#endif
out vec4 FragColor;

in vec3 FragPos;

const vec3 objectColor = vec3(0.0, 1.0, 1.0);

void main() 
{
    FragColor = vec4(objectColor, 0.4);
}
