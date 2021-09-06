#include "MyGLWidget.h"

class NouGLWidget : public MyGLWidget
{
 Q_OBJECT
	   
 public:
  NouGLWidget (QWidget *parent=NULL);
  ~NouGLWidget ();

 public slots:
     void actualitza_focus(int t);
 signals:
     void value_focus(int x);
     

 protected:
  virtual void initializeGL ();
  virtual void viewTransform ();
  virtual void modelTransformLego ();
  
  // keyPressEvent - Es cridat quan es prem una tecla
  virtual void keyPressEvent (QKeyEvent *event);
  
 private:
     bool luz_general;
     
     float focusX;
     glm::vec3 posFocus, colFocus;
     GLuint colorLoc, focusLoc;
     
     glm::vec3 focEspasa;
};
