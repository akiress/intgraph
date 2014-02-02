#version 330

uniform sampler2D diffuse;
uniform vec4 AmbientProduct;

in vec3 N;
in vec3 L;
in vec3 E;
in vec2 T;

out vec4 color;

void main() 
{ 
    vec4 a = AmbientProduct;

    vec3 n = normalize ( N );
    vec3 l = normalize ( L );
    vec3 e = normalize ( E );

    float Kd = max ( dot ( l, n ), 0.0f );
    vec4 d = texture2D ( diffuse, T );

    if (d.a < 0.5)
    {
        discard;
    }
    else
    {
        color = (a + Kd) * d;
    }
}