#ifdef GL_ES
	precision highp float;
	precision highp int;
#endif

#define PI 3.14159265359
#define INV_PI 0.31830988618
#define INV_TWO_PI 0.15915494309
#define saturate(x) clamp(x, 0.0, 1.0)

out vec4 FragColor;

layout (location = 0) in vec3 WorldPos;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec2 UV0;

vec3 diffuseNormalizedLambertBrdf(vec3 reflectance) 
{
    return reflectance * INV_PI;
}



void main()
{
    
    FragColor;
}
