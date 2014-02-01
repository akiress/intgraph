#version 330

in vec4 vPosition;
in vec3 vNormal;
in vec2 vTexCoord;

out vec3 N;
out vec3 L;
out vec3 E;
out vec2 T;

uniform mat4 ModelView;
uniform mat4 projection;
uniform vec4 LightPosition;

void main()
{
    vec3 pos = (ModelView * vPosition).xyz;

    L = ( (ModelView*LightPosition).xyz - pos );
    E = ( -pos );
    N = ( ModelView*vec4(vNormal, 0.0) ).xyz;
    T = vTexCoord;

    gl_Position = projection * ModelView * vPosition;
}
