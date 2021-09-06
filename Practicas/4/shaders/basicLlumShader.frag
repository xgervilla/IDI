#version 330 core

out vec4 FragColor;

in vec3 vertexFrag;
in vec3 normalFrag;
in vec3 posFrag;

in vec3 matambFS;
in vec3 matdiffFS;
in vec3 matspecFS;
in float matshinFS;

uniform vec3 colFocus;
uniform vec3 posFocus;

vec3 llumAmbient = vec3(1, 1, 1);

vec3 Lambert (vec3 NormSCO, vec3 L) {
    vec3 colRes = llumAmbient * matambFS;
    
    if (dot (L, NormSCO) > 0)
      colRes = colRes + colFocus * matdiffFS * dot (L, NormSCO);
    return (colRes);
}

vec3 Phong (vec3 NormSCO, vec3 L, vec4 vertSCO){

    vec3 colRes = Lambert (NormSCO, L);

    // Calculamos R i V
    if (dot(NormSCO,L) < 0)
      return colRes;
      
    vec3 R = reflect(-L, NormSCO);
    //equivale a: normalize (2.0*dot(NormSCO,L)*NormSCO - L);
    vec3 V = normalize(-vertSCO.xyz);

    if ((dot(R, V) < 0) || (matshinFS == 0))
      return colRes;  // no hay Componente especular
    
    //Componente especular
    float shine = pow(max(0.0, dot(R, V)), matshinFS);
    return (colRes + matspecFS * colFocus * shine); 
}

void main(){

    vec3 luz = normalize(posFrag-vertexFrag);

    vec3 nMaux = normalize(normalFrag);
    
    vec3 fcolor = Phong(nMaux,luz,vec4(vertexFrag,1.));
    
    FragColor = vec4(fcolor,1);	
}

