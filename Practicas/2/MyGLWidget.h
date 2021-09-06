#define GLM_FORCE_RADIANS
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLWidget>
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>
#include <QKeyEvent>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "model.h"

class MyGLWidget : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core 
{
  Q_OBJECT

  public:
    MyGLWidget(QWidget *parent=0);
    ~MyGLWidget();

  protected:
    //inicializa contexto gráfico
    virtual void initializeGL( );
    
    //pinta en pantalla
    virtual void paintGL( );
    
    //reajusta los valores para la redimensión de la ventana
    virtual void resizeGL(int width, int height);
    
    //recibe eventos de teclado
    virtual void keyPressEvent(QKeyEvent *event);
    
    //movimiento del ratón
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);

  private:
    //FUNCIONES:
      
    //creación de buffers
    void creaBuffersTerra();
    void creaBuffersCub();
    void creaBuffersHomer();
    void creaBuffersPatricio();
    
    //carga de shaders
    void carregaShaders();
    
    //transformaciones
    void projectTransform(); //proj
    void viewTransform(); //view
    void modelTransformTerra(); //tierra (TG)
    void modelTransformParet1(); //pared 1 (TG)
    void modelTransformParet2(); //pared 2 (TG)
    void modelTransformHomer(); //homer (TG)
    void modelTransformPatricio(); //Patricio (TG)
    
    //cálculo de valores esenciales del modelo
    void calculaCapsaModel(Model &p, float &escala, glm::vec3 &centreBase);
    
    //inicialización de la escena
    void iniEscena();
    
    //inicialización de la cámara
    void iniCamera();
    
    //auxiliar para evitar que el patricio atraviese las paredes
    bool xoca();
    
    //VARIABLES:
    
    // Program
    QOpenGLShaderProgram *program;
    
    //variables para los VAOs
    GLuint VAO_Cub, VAO_Terra, VAO_Homer, VAO_Patricio;
    
    //variables para los localizadores
    GLuint transLoc, projLoc, viewLoc;
    GLuint vertexLoc, colorLoc;
    
    //varibales de tamaño de la aplicación
    GLint ample, alt;

    //variables de modelo
    Model homer, patricio;
    
    //variables para el Homer (no necesarias)
    glm::vec3 posHomer, centreBaseHomer;
    
    //variables para el Patricio
    glm::vec3 centrePat;
    float pos_x, pos_z, rota_patricio;
    float scale, escalaPat, max_pat, alturaPat;
    
    
    //variables generales de la escena
    glm::vec3 centreEsc;
    float radiEsc;

    //variables para la primera persona (fp)
    bool fp;
    float zn_fp,zf_fp, dirx, dirz;
    glm::vec3 obs_fp,vrp_fp;
    int pos;
    
    //variables para la vista general
    glm::vec3 obs, vrp, up;
    float ra, fov, zn, zf, dist;
    
    //variables para tener una vista correcta con euler y el movimiento de ratón
    float angleX, angleY;
    typedef  enum {NONE, ROTATE} InteractiveAction;
    InteractiveAction DoingInteractive;
    int xClick, yClick;
};
