#include "NouGLWidget.h"

#include <iostream>

NouGLWidget::NouGLWidget(QWidget *parent) : MyGLWidget(parent) {
  grabKeyboard();
}

NouGLWidget::~NouGLWidget(){
}

void NouGLWidget::initializeGL() {
  MyGLWidget::initializeGL();
  focusLoc = glGetUniformLocation (program->programId(), "posFocus");
  colorLoc = glGetUniformLocation (program->programId(), "colFocus");
  
  
  luz_general = true;
  
  focusX = 3.;
  posFocus = glm::vec3(focusX,3.8, 0.5);
  glUniform3fv(focusLoc,1,&posFocus[0]);
  colFocus = glm::vec3(1,1,1);
  glUniform3fv(colorLoc,1,&colFocus[0]);

}

void NouGLWidget::modelTransformLego(){
  MyGLWidget::modelTransformLego();
}

void NouGLWidget::viewTransform(){
  MyGLWidget::viewTransform();
}

void NouGLWidget::keyPressEvent(QKeyEvent* event){
    makeCurrent();
    switch (event->key()) {
        case Qt::Key_A: {
            if(focusX>0){
                focusX-=0.2;
                posFocus = glm::vec3(focusX,3.8,0.5);
                if(luz_general)
                    glUniform3fv(focusLoc,1,&posFocus[0]);
            }
            emit value_focus(focusX*10);
            break;
        }
        case Qt::Key_D: {
            if(focusX<5.8){
                focusX+=0.2;
                posFocus = glm::vec3(focusX,3.8,0.5);
                if(luz_general)
                    glUniform3fv(focusLoc,1,&posFocus[0]);
            }
            emit value_focus((focusX+0.2)*10);
            break;
        }
        case Qt::Key_F: {
            luz_general = not luz_general;
            if(luz_general){
                colFocus = glm::vec3(1,1,1);
                glUniform3fv(colorLoc,1,&colFocus[0]);
                posFocus = glm::vec3(focusX,3.8,0.5);
                glUniform3fv(focusLoc,1,&posFocus[0]);
            }
            else{
                colFocus = glm::vec3(0.5,1.,0.);
                glUniform3fv(colorLoc,1,&colFocus[0]);
            
                focEspasa = glm::vec3 (legoTG * glm::vec4(1.85,20.,7.3,1.));
                glUniform3fv(focusLoc,1,&focEspasa[0]);
            }
            break;
        }
        
        default: {
            MyGLWidget::keyPressEvent(event);
            if(not luz_general){
                focEspasa = glm::vec3 (legoTG * glm::vec4(1.85,20.,7.3,1.));
                glUniform3fv(focusLoc,1,&focEspasa[0]);
            }
            break;
        }
  }
  update();
}

void NouGLWidget::actualitza_focus(int t){
    makeCurrent();
    focusX = float(t)/10;
    posFocus = glm::vec3(focusX,3.8,0.5);
    if(luz_general) glUniform3fv(focusLoc,1,&posFocus[0]);
    update();
}

  
