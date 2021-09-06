#define GLM_FORCE_RADIANS
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLWidget>
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>
#include <QKeyEvent>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

class MyGLWidget : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core
{
  Q_OBJECT

  public:
    MyGLWidget (QWidget *parent=0);
    ~MyGLWidget ();
    
  protected:
    virtual void initializeGL ();
    virtual void paintGL ();
    virtual void resizeGL(int width,int height);
    virtual void keyPressEvent (QKeyEvent *event);

  private:
    void creaBuffersUmbrella ();
    void carregaShaders ();
    void pintaUmbrella ();    
    void modelTransformUmbrella (float anglegir, glm::vec3 posicioCentre ,  float escala, bool rodaDreta);

    GLuint vertexLoc;
    GLuint varLoc;
    GLuint tgLoc;
    GLuint colorLoc;
    QOpenGLShaderProgram *program;

    float val;
    float rotation_hor;
    float rotation_vert;
    
    
    GLuint VAO1;
    GLint ample, alt;

    int num_triang;
    int num_vertx;
    float angularOffset;
};
