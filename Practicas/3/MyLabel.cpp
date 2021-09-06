#include "MyLabel.h"

MyLabel::MyLabel(QWidget *parent=0):QLabel(parent){
    cont = 0;
    apagat = true;
}

/*
 * Els 4 moviments tenen la mateixa esructura:
    si no està apagat aumenta el contador:
    ->si el contador és igual a 15 (límit de moviments permesos) el robot s'apaga i el contador torna a 0
    ->sino comprovem si el contador és >= 10 per indicar que queden pocs moviments (color de fons en color groc) i escrivim al label el moviment realitzat
    ->com els dos casos modifiquen el contador al final emetem el signal que provoca que s'actualitzi segons el valor actual

 * El gir només actualitza l'estat si el robot no està apagat

 * On_off actualitza l'estat "suprem" del robot (apagat o
    encès) i segons aquest nou estat modifica el label; per evitar que quedi un contador erroni també s'emet el signal que actualitza el label del contador
*/

void MyLabel::move_left(){
    if (not apagat) {
        ++cont;
        if(cont == 15){
            cont = 0;
            setStyleSheet("background-color: red;");
            setText("Apagat");
            apagat = true;
        }
        else{
            if(cont>=10) setStyleSheet("background-color: yellow;");
            
            setText("Esquerra");
        }
        emit enviaCont(cont);
    }
}
      
void MyLabel::move_back(){
    if (not apagat) {
        ++cont;
        if(cont == 15){
            cont = 0;
            setStyleSheet("background-color: red;");
            setText("Apagat");
            apagat = true;
        }
        else{
            if(cont>=10) setStyleSheet("background-color: yellow;");
            
            setText("Enrera");
        }
        emit enviaCont(cont);
    }
}

void MyLabel::move_front(){
    if (not apagat) {
        ++cont;
        if(cont == 15){
            cont = 0;
            setStyleSheet("background-color: red;");
            setText("Apagat");
            apagat = true;
        }
        else{
            if(cont>=10) setStyleSheet("background-color: yellow;");
            
            setText("Endavant");
        }
        emit enviaCont(cont);
    }
}

void MyLabel::move_right(){
    if (not apagat) {
        ++cont;
        if(cont == 15){
            cont = 0;
            setStyleSheet("background-color: red;");
            setText("Apagat");
            apagat = true;
        }
        else{
            if(cont>=10) setStyleSheet("background-color: yellow;");
            
            setText("Dreta");
        }
        emit enviaCont(cont);
    }
}

void MyLabel::gir(){
    if(not apagat){
        setText("Gir");
    }
}

void MyLabel::on_off(){
    apagat = not apagat;
    cont = 0; //tant si l'apaguem com l'encenem el contador es reinicia i per tant torna a 0
    if(apagat){
        setStyleSheet("background-color: red;");
        setText("Apagat");
    }
    else{
        setStyleSheet("background-color: green;");
        setText("Funcionant");
    }
    emit enviaCont(cont);
}
