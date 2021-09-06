#include "NouGLWidget.h"

#include <iostream>

NouGLWidget::NouGLWidget(QWidget *parent) : MyGLWidget(parent) {
  grabKeyboard();
}

NouGLWidget::~NouGLWidget (){
}

void NouGLWidget::paintGL() {
  MyGLWidget::paintGL();
}

void NouGLWidget::resizeGL(int w, int h) {
    rav = float(w)/float(h);
    if(rav < 1.0){
    fov = 2.0*atan(tan(float(M_PI/3.)/2.0)/rav);
    }
    projectTransform();
    
}

void NouGLWidget::iniCamera (){
    MyGLWidget::iniCamera();
    fov = float(M_PI/3.);
    glm::vec3 psFocus = glm::vec3(1,4,3);
    glUniform3fv(posFocusLoc, 1, &psFocus[0]);
    
    glm::vec3 colFocus = glm::vec3(1, 1, 1);
    glUniform3fv(colFocusLoc, 1, &colFocus[0]);
    camera_pat = false;
}

void NouGLWidget::iniEscena (){
    MyGLWidget::iniEscena();
    escMax = glm::vec3(9.,3.,6.);
    escMin = glm::vec3(0.,0.,0.);
    centreEsc = (escMax+escMin)/glm::vec3(2.);
    radiEsc = distance(escMax,escMin)/2;
    
    posPilota = glm::vec3(2,0,3);
    reinici = false;
}

void NouGLWidget::modelTransformTerra (){
    glm::mat4 TG(1.f);
    TG = glm::scale(TG,glm::vec3(3,1,2));
    glUniformMatrix4fv (transLoc, 1, GL_FALSE, &TG[0][0]);
}

void NouGLWidget::modelTransformPilota (){
  glm::mat4 pilTG = glm::mat4(1.0f);
  pilTG = glm::translate(pilTG, posPilota);
  pilTG = glm::scale(pilTG, glm::vec3(0.5*escalaPil, 0.5*escalaPil, 0.5*escalaPil));
  pilTG = glm::translate(pilTG, -centreBasePil);
  glUniformMatrix4fv(transLoc, 1, GL_FALSE, &pilTG[0][0]);
}

void NouGLWidget::modelTransformPatricio (){
  glm::mat4 patTG = glm::mat4(1.0f);
  
  patTG = glm::translate(patTG, glm::vec3(1,0,3));
  patTG = glm::rotate(patTG, float(M_PI/2.),glm::vec3(0,1,0));
  
  patTG = glm::scale(patTG, glm::vec3(3*escalaPat, 3*escalaPat, 3*escalaPat));
  patTG = glm::translate(patTG, -centreBasePat);
  glUniformMatrix4fv(transLoc, 1, GL_FALSE, &patTG[0][0]);
}

void NouGLWidget::viewTransform() {
    if(camera_pat){
        glm::vec3 obs = glm::vec3(9,3,6);
        glm::vec3 vrp = glm::vec3(1,1.5,3);
        View = glm::lookAt(obs,vrp,glm::vec3(0,1,0));
    }
    else{
        View = glm::translate(glm::mat4(1.f), glm::vec3(0, 0, -2*radiEsc));
        View = glm::rotate(View, angleX, glm::vec3(1, 0, 0));
        View = glm::rotate(View, -angleY, glm::vec3(0, 1, 0));
        View = glm::translate(View, -centreEsc);
    }
    glUniformMatrix4fv (viewLoc, 1, GL_FALSE, &View[0][0]);
}

void NouGLWidget::projectTransform() {
    float zn, zf;
    zn = radiEsc;
    zf = 3*radiEsc;
    fov = float(M_PI/3.0);
    glm::mat4 Proj;  // Matriu de projecció
    if(camera_pat){
        zn = 0.1;
        zf = 2*radiEsc;
    }

    Proj = glm::perspective(fov, rav, zn, zf);

    glUniformMatrix4fv (projLoc, 1, GL_FALSE, &Proj[0][0]);
}

void NouGLWidget::keyPressEvent(QKeyEvent* event){
  makeCurrent();
  switch (event->key()) {
    case Qt::Key_A: {
        if(reinici){
            glm::vec3 colFocus = glm::vec3(1, 1, 1);
            glUniform3fv(colFocusLoc, 1, &colFocus[0]);
            reinici = false;
        }
        if(posPilota.x>2.) posPilota.x -=0.5;
      break;
    }
    case Qt::Key_D: {
        if(posPilota.x<9.5){
            if(reinici){
                glm::vec3 colFocus = glm::vec3(1, 1, 1);
                glUniform3fv(colFocusLoc, 1, &colFocus[0]);
                reinici = false;
            }
            posPilota.x +=0.5;
            if(posPilota.x == 9.){
                posPilota.x = 2.;
                //canvia color colFocus
                reinici = true;
                glm::vec3 colFocus = glm::vec3(1, 0, 1);
                glUniform3fv(colFocusLoc, 1, &colFocus[0]);
            }
        }
        
      break;
    }
    case Qt::Key_C: {
        camera_pat = not camera_pat;
        projectTransform();
        viewTransform();
        if(camera_pat) emit activa_pat();
        else emit activa_general();
      break;
    }
    default: {
      MyGLWidget::keyPressEvent(event);
      break;
    }
  }
  update();
}

void NouGLWidget::change_camera(){
    makeCurrent();
    camera_pat = not camera_pat;
    projectTransform();
    viewTransform();
    update();
}

void NouGLWidget::move_dreta(){
    makeCurrent();
    if(posPilota.x<9.5){
        if(reinici){
            glm::vec3 colFocus = glm::vec3(1, 1, 1);
            glUniform3fv(colFocusLoc, 1, &colFocus[0]);
            reinici = false;
        }
        posPilota.x +=0.5;
        if(posPilota.x == 9.){
            posPilota.x = 2.;
            //canvia color colFocus
            reinici = true;
            glm::vec3 colFocus = glm::vec3(1, 0, 1);
            glUniform3fv(colFocusLoc, 1, &colFocus[0]);
        }
    }
    update();
}

void NouGLWidget::move_esquerra(){
    makeCurrent();
    if(reinici){
        glm::vec3 colFocus = glm::vec3(1, 1, 1);
        glUniform3fv(colFocusLoc, 1, &colFocus[0]);
        reinici = false;
    }
    if(posPilota.x>2.) posPilota.x -=0.5;
    update();
}

  
