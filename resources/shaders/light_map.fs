#ifdef GL_ES
	precision highp float;
    precision highp int;
    precision highp sampler2D;
#endif

#define saturate(x) clamp(x, 0.0, 1.0)

out vec4 FragColor;

uniform vec4      uBaseColorFactor;
uniform sampler2D uBaseColorMap;
uniform int       uBaseColorMapSet;

in vec3 WorldPos;
in vec2 UV0;
in vec2 UV1;

void main()
{
	vec4 baseColor;
	
    if (uBaseColorMapSet > -1) 
        baseColor = texture(uBaseColorMap, uBaseColorMapSet == 0 ? UV0 : UV1);
    else 
        baseColor = uBaseColorFactor;
        
    FragColor = baseColor;
}
