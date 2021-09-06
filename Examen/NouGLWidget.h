#include "MyGLWidget.h"

class NouGLWidget : public MyGLWidget
{
 Q_OBJECT
	   
 public:
  NouGLWidget (QWidget *parent=NULL);
  ~NouGLWidget ();

 public slots:
     void change_camera();
     void move_dreta();
     void move_esquerra();
     
 signals:
     void activa_general();
     void activa_pat();

 protected:
  void paintGL ();
  void resizeGL (int w, int h);
  void iniCamera ();
  void iniEscena ();
  void viewTransform ();
  void projectTransform ();
  void modelTransformTerra ();
  void modelTransformPatricio ();
  void modelTransformPilota ();
  
  // keyPressEvent - Es cridat quan es prem una tecla
  void keyPressEvent (QKeyEvent *event);
  
 private:
     glm::vec3 escMin, escMax;
     float fov;
     glm::vec3 posPilota;
     bool reinici, camera_pat;
};
