#ifdef GL_ES
    precision highp int;
    precision highp float;
#endif
out vec4 FragColor;

in vec3 WorldPos;
in vec2 UV;

uniform sampler2D uAlbedo;

void main()
{
    FragColor = texture(uAlbedo, UV);
    //FragColor = vec4(UV, 0.0, 0.5);
}
