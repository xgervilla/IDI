#include "MyGLWidget.h"
#include <iostream>


MyGLWidget::MyGLWidget (QWidget* parent) : QOpenGLWidget(parent), program(NULL){
    setFocusPolicy(Qt::StrongFocus);
    num_triang = 8;
    num_vertx = num_triang*3;
    angularOffset =  2*M_PI/(float)num_triang;
    //15º = 2*M_PI / 24 == M_PI/12
}

MyGLWidget::~MyGLWidget (){
    if (program != NULL)
        delete program;
}

void MyGLWidget::initializeGL (){
    rotation_hor = 0.;
    initializeOpenGLFunctions();
    glClearColor (0.5, 0.7, 1.0, 1.0);
    carregaShaders();
    creaBuffersUmbrella();
    rotation_vert = 0;
}

void MyGLWidget::pintaUmbrella(){
    glBindVertexArray(VAO1);
    glDrawArrays(GL_TRIANGLES,0,num_vertx);
}

void MyGLWidget::paintGL (){
    glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);
    //para indicar de qué color pintar modificamos val
    // 0 == blanco/rojo ; 1 == negro
    
    //externos
    val = 0.;
    glUniform1f(varLoc,val);
    modelTransformUmbrella (rotation_hor, glm::vec3(-0.5, 0.0, 0.0),0.5, false);
    pintaUmbrella();
    
    modelTransformUmbrella ((rotation_hor+M_PI/4), glm::vec3(0.5, 0.0, 0.0), 0.5, true);
    pintaUmbrella();
    
    //interiores
    val = 1.;
    glUniform1f(varLoc,val);
    modelTransformUmbrella (rotation_hor, glm::vec3(0.5, 0.0, 0.0), 0.25, true);
    pintaUmbrella();
    modelTransformUmbrella (rotation_hor, glm::vec3(-0.5, 0.0, 0.0), 0.25, false);
    pintaUmbrella();
    
    glBindVertexArray(0);
}

void MyGLWidget::modelTransformUmbrella (float anglegir, glm::vec3 posicioCentre, float escala , bool rodaDreta){
    glm::mat4 TG (1.0);
    //matrix identidad sobre la que hacemos las transformaciones
    TG = glm::translate(TG,posicioCentre);
    TG = glm::rotate(TG,rotation_vert,glm::vec3(1.,0.,0.));
    //rodaDreta indica si estamos en los umbrella de la derecha
    
    if(rodaDreta)
         TG = glm::rotate(TG,-anglegir, glm::vec3(0.,0.,1.));
    else
        TG = glm::rotate(TG,anglegir,glm::vec3(0.,0.,1.));
    
    TG = glm::scale(TG,glm::vec3(escala,escala,1.));
   
    glUniformMatrix4fv(tgLoc,1,GL_FALSE,&TG[0][0]);
    
}

void MyGLWidget::resizeGL (int w, int h){
    ample = w;
    alt = h;
}

void MyGLWidget::keyPressEvent(QKeyEvent* event){
    makeCurrent();
    switch (event->key()) {
    case Qt::Key_R: {
        rotation_hor-=M_PI/12;
        break;
    }
    case Qt::Key_T: {
        rotation_vert+=M_PI/12;
        break;
    }
    default: event->ignore();
        break;
    }
    update();
}

void MyGLWidget::creaBuffersUmbrella (){
    glm::vec3 Vertices[num_vertx];
    glm::vec3 colores[num_vertx];

    float a = 0;
    for (int i=0; i<num_vertx; i+=3) {
        Vertices[i] = glm::vec3(0.0, 0.0, 0.0);
        Vertices[i+1]= glm::vec3(cos(a), sin(a), 0);
        a+= angularOffset;
        Vertices[i+2]= glm::vec3(cos(a), sin(a), 0);
        if(i%2 == 0){
            colores[i] = glm::vec3(1.,0.,0.);
            colores[i+1] = glm::vec3(1.,0.,0.);
            colores[i+2] = glm::vec3(1.,0.,0.);
        }
        else{
            colores[i] = glm::vec3(1.,1.,1.);
            colores[i+1] = glm::vec3(1.,1.,1.);
            colores[i+2] = glm::vec3(1.,1.,1.);
        }
    }
    glGenVertexArrays(1, &VAO1);
    glBindVertexArray(VAO1);
    
    GLuint VBOs[2];
    glGenBuffers(2, VBOs);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(vertexLoc);
    
    
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colores), colores, GL_STATIC_DRAW);
    glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(colorLoc);

    glBindVertexArray(0);
}

void MyGLWidget::carregaShaders(){
    {
        QOpenGLShader fs (QOpenGLShader::Fragment, this);
    QOpenGLShader vs (QOpenGLShader::Vertex, this);
    fs.compileSourceFile("shaders/basicShader.frag");
    vs.compileSourceFile("shaders/basicShader.vert");
    program = new QOpenGLShaderProgram(this);
    program->addShader(&fs);
    program->addShader(&vs);
    program->link();
    program->bind();
    }
    
    vertexLoc = glGetAttribLocation (program->programId(), "vertex");
    //localizador de la variable val para indicar el color para pintar
    varLoc = glGetUniformLocation(program->programId(),"val");
    //localizador de la TG
    tgLoc = glGetUniformLocation (program->programId(),"TG");
    //localizador de los colores
    colorLoc = glGetAttribLocation(program->programId(),"color");
 }
