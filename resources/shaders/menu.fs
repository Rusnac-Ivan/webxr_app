#ifdef GL_ES
    precision highp int;
    precision highp float;
#endif
out vec4 FragColor;

in vec3 WorldPos;
in vec2 UV;

uniform bool uIsVideo;

uniform sampler2D uAlbedo;

void main()
{
    vec4 col = texture(uAlbedo, UV);
    
    if(uIsVideo)
    {        
        float rbAverage = (col.r + col.b) / 2.0;
        float gDelta = max(0.0, col.g - rbAverage);
        
        col.a = (1.0 - smoothstep(0.0, 0.2, gDelta)) * col.a;
        
        col.a = col.a * col.a;
    }
    
    FragColor = col;
}
