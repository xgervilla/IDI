#version 330 core

in vec3 vertex;
in vec3 normal;

in vec3 matamb;
in vec3 matdiff;
in vec3 matspec;
in float matshin;

uniform mat4 proj;
uniform mat4 view;
uniform mat4 TG;

uniform vec3 posFocus;
uniform vec3 colFocus;

out vec3 vertexFrag;
out vec3 normalFrag;
out vec3 posFrag;

out vec3 matambFS;
out vec3 matdiffFS;
out vec3 matspecFS;
out float matshinFS;


void main(){
    
    vertexFrag = (view*TG*vec4(vertex,1.)).xyz;
    posFrag = (view*vec4(posFocus,1.)).xyz;
    
    mat3 normalM = (inverse(transpose(mat3(view*TG))));
    normalFrag = normalM * normal;
    
    matambFS = matamb;
    matdiffFS = matdiff;
    matspecFS = matspec;
    matshinFS = matshin;
    
    gl_Position = proj * view * TG * vec4 (vertex, 1.0);
}
