#version 330 core

out vec4 FragColor;
in vec3 fcolor;
uniform float val;
/*
val: indica qué color pintar (0-rojo / 1-blanco) -> pasar cualquier otro valor se considera negro (por defecto)
*/

void main() {
/*
rojo: (1.,0.,0.)
blanco: (1.,1.,1.)
negro: (0.,0.,0.)
*/
    if(val==1.) FragColor=vec4(0.,0.,0.,0.);
    else{
        FragColor = vec4(fcolor,1.);
        if(fcolor == vec3(1.,0.,0.)){
            if(int(gl_FragCoord.x)%10 < 4) discard;
        }
    }
}
