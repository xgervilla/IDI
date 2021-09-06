#include "MyGLWidget.h"

#include <iostream>

MyGLWidget::MyGLWidget (QWidget* parent) : QOpenGLWidget(parent), program(NULL){
  setFocusPolicy(Qt::StrongFocus);  //para recibir eventos de teclado
}

MyGLWidget::~MyGLWidget (){
  if (program != NULL)
    delete program;
}

//Inicializaciones
//contexto gráfico
void MyGLWidget::initializeGL (){
  initializeOpenGLFunctions();  

  glClearColor(0.5, 0.7, 1.0, 1.0); // color de fondo
  glEnable(GL_DEPTH_TEST);
  carregaShaders();
  iniEscena ();
  iniCamera ();
}
//escena
void MyGLWidget::iniEscena (){
  creaBuffersTerra();
  creaBuffersCub();
  creaBuffersHomer();
  creaBuffersPatricio();
  
  scale = 1;
  rota_patricio = 0; //rotación (tecla R)
  
  pos_x = pos_z = 10; //movimentos con teclado (flechas)
  centreEsc = glm::vec3(10,3,10);
  radiEsc = distance(glm::vec3(0,0,0),glm::vec3(20,0,20))/2.;
}
//camara
void MyGLWidget::iniCamera (){
  angleX = 0;
  angleY = M_PI/6.;
  
  obs = centreEsc + glm::vec3(0,centreEsc.y, 2*radiEsc);
  vrp = centreEsc;
  up = glm::vec3(0,1,0);
  
  ra = 1.0;
  fov = M_PI/3.0; //60º
  
  zn = radiEsc;
  zf = 3.0*radiEsc;
  
  zn_fp = max_pat/2.;
  zf_fp = 2*radiEsc;
  pos = 0; //auxiliar para saber en qué direccion miramos en fp
  
  dist = distance(centreEsc,obs);
  
  fp = false; //por defecto empieza en vista general
  
  projectTransform ();
  viewTransform ();
}


//pinta en pantalla
void MyGLWidget::paintGL (){

#ifdef __APPLE__
  GLint vp[4];
  glGetIntegerv (GL_VIEWPORT, vp);
  ample = vp[2];
  alt = vp[3];
#endif
  
  // Borramos frame-buffer y depth-buffer
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  //activamos el VAO para la Tierra 
  glBindVertexArray (VAO_Terra);
  modelTransformTerra();
  glDrawArrays(GL_TRIANGLES, 0, 6);

  //activamos el vao para las dos paredes
  glBindVertexArray (VAO_Cub);
  modelTransformParet1();
  glDrawArrays(GL_TRIANGLES, 0, 36);
  modelTransformParet2();
  glDrawArrays(GL_TRIANGLES,0,36);
  
  //activamos el vao para el patricio
  glBindVertexArray(VAO_Patricio);
  modelTransformPatricio();
  glDrawArrays(GL_TRIANGLES,0,patricio.faces().size()*3);
  
  glBindVertexArray(0);
}


//reajuste de los valores para la redimensión de la ventana
void MyGLWidget::resizeGL(int w, int h){
  ample = w;
  alt = h;
  ra = float(ample)/float(alt);
  if(ra<1.0) fov = 2.0*atan(tan((M_PI/3.0)/2) / ra);
  projectTransform ();
}


//transformaciones (TG)
//Homer
void MyGLWidget::modelTransformHomer(){
  glm::mat4 TG(1.f);
  glUniformMatrix4fv (transLoc, 1, GL_FALSE, &TG[0][0]);
}
//Patricio
void MyGLWidget::modelTransformPatricio(){
  glm::mat4 TG(1.f);
  //transformación para moverlo según el teclado
  TG = glm::translate(TG, glm::vec3 (pos_x,0,pos_z));
  
  //transformaciones para dejarlo en el centro de la tierra
  TG = glm::scale(TG, glm::vec3(scale*escalaPat));
  TG = glm::rotate(TG,-rota_patricio,glm::vec3(0,1,0));
  TG = glm::translate(TG,-centrePat); 
  glUniformMatrix4fv (transLoc, 1, GL_FALSE, &TG[0][0]);
}
//pared de la izquierda
void MyGLWidget::modelTransformParet1(){
  glm::mat4 TG(1.f);
  TG = glm::translate(TG,glm::vec3(10,0,16));
  TG = glm::scale(TG,glm::vec3(0.6,6,8));
  
  glUniformMatrix4fv (transLoc, 1, GL_FALSE, &TG[0][0]);
}
//pared de la derecha
void MyGLWidget::modelTransformParet2(){
  glm::mat4 TG(1.f);
  TG = glm::translate(TG,glm::vec3(10,0,4));
  TG = glm::scale(TG,glm::vec3(0.6,6,8));
  
  glUniformMatrix4fv (transLoc, 1, GL_FALSE, &TG[0][0]);
}
//tierra
void MyGLWidget::modelTransformTerra (){
  glm::mat4 TG(1.f);
  glUniformMatrix4fv (transLoc, 1, GL_FALSE, &TG[0][0]);
}


//transformaciones (Proj y View)
//Proj
void MyGLWidget::projectTransform(){
    glm::mat4 Proj;
    if(fp) Proj = glm::perspective(fov, ra, zn_fp, zf_fp);
    else Proj = glm::perspective(fov,ra,zn,zf);

    glUniformMatrix4fv (projLoc, 1, GL_FALSE, &Proj[0][0]);
}
//View
void MyGLWidget::viewTransform (){
    glm::mat4 View (1.);
    if(fp){
        
        if(pos == 3){
            dirx = 1;
            dirz = 0;
        }
        else if(pos == 2){
            dirx = 0;
            dirz = -1;
        }
        else if(pos == 1){
            dirx = -1;
            dirz = 0;
        }
        else if(pos == 0){
            dirx = 0;
            dirz = 1;
        }
        obs_fp = glm::vec3(pos_x,alturaPat,pos_z);
        vrp_fp = obs_fp + glm::vec3(dirx,0,dirz);
        View = glm::lookAt (obs_fp,vrp_fp,up);
    }
    else{
        View = glm::translate(View,-glm::vec3(0,0,2*radiEsc));
        
        View = glm::rotate(View,angleX,glm::vec3(1,0,0));
        View = glm::rotate(View,-angleY,glm::vec3(0,1,0));
        
        View = glm::translate(View,-centreEsc);
    }

    glUniformMatrix4fv (viewLoc, 1, GL_FALSE, &View[0][0]);
}


//Función auxiliar para evitar que el patricio atraviese la pared
bool MyGLWidget::xoca(){
    return (pos_x<(10.3+max_pat) and pos_x>(9.7-max_pat) and (pos_z>11 or pos_z<9));
}


//recibe eventos de teclado
void MyGLWidget::keyPressEvent(QKeyEvent* event){
  makeCurrent();
  switch (event->key()) {
    //rotamos 90º en sentido horario
    case Qt::Key_R: {
      rota_patricio += M_PI/2;
      pos+=1;
      pos=pos%4;
      break;
    }
    
    //volvemos a la orientación inicial
    case Qt::Key_I: {
      pos_x = pos_z = 10;
      rota_patricio = 0;
      pos = 0;
      break;
    }
    
    //movemos según la flecha del teclado: si una vez movido choca lo dejamos como estaba
    case Qt::Key_Left: { //movemos a x-
        if(pos_x>=max_pat){
            pos_x -=0.5;
            if(xoca()) pos_x+=0.5;
        }
        break;
    }
    case Qt::Key_Right: { //movemos a x+
        if(pos_x<=(20-max_pat)){
            pos_x += 0.5;
            if(xoca()) pos_x-=0.5;
        }
        break;
    }
    case Qt::Key_Up: { //movemos a z-
        if(pos_z>=max_pat){
            pos_z -= 0.5;
            if(xoca()) pos_z+=0.5;
        }
        break;
    }
    case Qt::Key_Down: { //movemos a z+
        if(pos_z<=(20-max_pat)){
            pos_z += 0.5;
            if(xoca()) pos_z -=0.5;
        }
        break;
    }
    
    //invertimos la vista entre general y primera persona
    case Qt::Key_C: { 
      fp = not fp;
      projectTransform();
      break;
    }
    
    default: event->ignore(); break;
    }
  viewTransform();
  update();
}


//movimiento del ratón
void MyGLWidget::mousePressEvent (QMouseEvent *e){
  xClick = e->x();
  yClick = e->y();

  if (e->button() & Qt::LeftButton &&
      ! (e->modifiers() & (Qt::ShiftModifier|Qt::AltModifier|Qt::ControlModifier)))
  {
    DoingInteractive = ROTATE;
  }
}

void MyGLWidget::mouseReleaseEvent( QMouseEvent *){
  DoingInteractive = NONE;
}

void MyGLWidget::mouseMoveEvent(QMouseEvent *e){
  makeCurrent();
  if (DoingInteractive == ROTATE && not fp){
    int xaux = abs(e->x() - xClick);
    int yaux = abs(e->y() - yClick);
    if(xaux>yaux){
        if(e->x() > xClick) angleY += xaux*0.02;
        else if(e->x() < xClick) angleY -= xaux*0.02;
    }
    else{
        if(e->y() > yClick) angleX -= yaux*0.02;
        else if(e->y() < yClick) angleX += yaux*0.02;
    }
    viewTransform ();
  }

  xClick = e->x();
  yClick = e->y();

  update ();
}


//cálculo de valores esenciales del modelo
void MyGLWidget::calculaCapsaModel (Model &p, float &escala, glm::vec3 &centreBase){
  float minx, miny, minz, maxx, maxy, maxz;
  minx = maxx = p.vertices()[0];
  miny = maxy = p.vertices()[1];
  minz = maxz = p.vertices()[2];
  for (unsigned int i = 3; i < p.vertices().size(); i+=3)
  {
    if (p.vertices()[i+0] < minx)
      minx = p.vertices()[i+0];
    if (p.vertices()[i+0] > maxx)
      maxx = p.vertices()[i+0];
    if (p.vertices()[i+1] < miny)
      miny = p.vertices()[i+1];
    if (p.vertices()[i+1] > maxy)
      maxy = p.vertices()[i+1];
    if (p.vertices()[i+2] < minz)
      minz = p.vertices()[i+2];
    if (p.vertices()[i+2] > maxz)
      maxz = p.vertices()[i+2];
  }
  escala = 2.0/(maxy-miny);
  alturaPat =maxy*escala-0.6; //altura de la camara en primera persona
  max_pat = (maxz-minz)/2.; //limit para que no choque con las paredes al rotar ni se salga de los extremos del suelo
  centreBase[0] = (minx+maxx)/2.0; 
  centreBase[1] = miny; 
  centreBase[2] = (minz+maxz)/2.0;
}


//creación de buffers
//cubo
void MyGLWidget::creaBuffersCub (){
  // Dades del cub
  glm::vec3 vertexs[8] = {
       /* 0*/ glm::vec3( -0.5, 0.0, -0.5),  /* 1*/ glm::vec3( 0.5, 0.0, -0.5),
       /* 2*/ glm::vec3( -0.5, 1.0, -0.5),  /* 3*/ glm::vec3( 0.5, 1.0, -0.5),
       /* 4*/ glm::vec3( -0.5, 0.0, 0.5),  /* 5*/ glm::vec3( 0.5, 0.0, 0.5),
       /* 6*/ glm::vec3( -0.5, 1.0, 0.5),  /* 7*/ glm::vec3( 0.5, 1.0, 0.5)
  };

  glm::vec3 poscub[36] = {
       vertexs[0], vertexs[2], vertexs[1],
       vertexs[1], vertexs[2], vertexs[3],
       vertexs[5], vertexs[1], vertexs[7],
       vertexs[1], vertexs[3], vertexs[7],
       vertexs[2], vertexs[6], vertexs[3],
       vertexs[3], vertexs[6], vertexs[7],
       vertexs[0], vertexs[4], vertexs[6],
       vertexs[0], vertexs[6], vertexs[2],
       vertexs[0], vertexs[1], vertexs[4],
       vertexs[1], vertexs[5], vertexs[4],
       vertexs[4], vertexs[5], vertexs[6],
       vertexs[5], vertexs[7], vertexs[6]
  };
  
  glm::vec3 color(0, 0, 1.0);

  glm::vec3 colorcub[36] = {
        color, color, color, color, color, color,
        color, color, color, color, color, color,
        color, color, color, color, color, color,
        color, color, color, color, color, color,
        color, color, color, color, color, color,
        color, color, color, color, color, color
  };
  
  // Creación del VAO del cubo
  glGenVertexArrays(1, &VAO_Cub);
  glBindVertexArray(VAO_Cub);

  GLuint VBO_Cub[2];
  glGenBuffers(2, VBO_Cub);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Cub[0]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(poscub), poscub, GL_STATIC_DRAW);

  glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(vertexLoc);

  glBindBuffer(GL_ARRAY_BUFFER, VBO_Cub[1]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(colorcub), colorcub, GL_STATIC_DRAW);

  glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(colorLoc);

  glBindVertexArray(0);
}
//homer
void MyGLWidget::creaBuffersHomer (){
  // Cargamos el objeto
  homer.load("./models/HomerProves.obj");
  
  // Creación del VAO del Homer
  glGenVertexArrays(1, &VAO_Homer);
  glBindVertexArray(VAO_Homer);

  GLuint VBO_Homer[2];
  // Buffer de posiciones
  glGenBuffers(2, VBO_Homer);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Homer[0]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*homer.faces().size()*3*3, homer.VBO_vertices(), GL_STATIC_DRAW);

  glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(vertexLoc);

  // Buffer de color
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Homer[1]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*homer.faces().size()*3*3, homer.VBO_matdiff(), GL_STATIC_DRAW);

  glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(colorLoc);

  glBindVertexArray(0);
}
//patricio
void MyGLWidget::creaBuffersPatricio (){
  patricio.load("./models/Patricio.obj");
  
  // Creación del VAO del Patricio
  glGenVertexArrays(1, &VAO_Patricio);
  glBindVertexArray(VAO_Patricio);

  GLuint VBO_Patricio[2];
  // Buffer de posiciones
  glGenBuffers(2, VBO_Patricio);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Patricio[0]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*patricio.faces().size()*3*3, patricio.VBO_vertices(), GL_STATIC_DRAW);

  glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(vertexLoc);

  // Buffer de color
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Patricio[1]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*patricio.faces().size()*3*3, patricio.VBO_matdiff(), GL_STATIC_DRAW);

  glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(colorLoc);

  glBindVertexArray(0);

  calculaCapsaModel(patricio,escalaPat,centrePat);
}
//tierra
void MyGLWidget::creaBuffersTerra (){
  // Dades del terra
  glm::vec3 posterra[6] = {
  glm::vec3(0.0, 0.0, 20.0),
  glm::vec3(20.0, 0.0, 20.0),
  glm::vec3(0.0, 0.0, 0.0),
  glm::vec3(0.0, 0.0, 0.0),
  glm::vec3(20.0, 0.0, 20.0),
  glm::vec3(20.0, 0.0, 0.0),
  }; 

  glm::vec3 color(0.0,0.6,0.0);

  glm::vec3 colterra[6] = {
  color, color, color, color, color, color
  };

  // Creación del VAO de tierra
  glGenVertexArrays(1, &VAO_Terra);
  glBindVertexArray(VAO_Terra);

  GLuint VBO_Terra[2];
  glGenBuffers(2, VBO_Terra);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Terra[0]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(posterra), posterra, GL_STATIC_DRAW);

  glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(vertexLoc);

  glBindBuffer(GL_ARRAY_BUFFER, VBO_Terra[1]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(colterra), colterra, GL_STATIC_DRAW);

  glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(colorLoc);

  glBindVertexArray(0);
}


//carga de shaders
void MyGLWidget::carregaShaders(){
  QOpenGLShader fs (QOpenGLShader::Fragment, this);
  QOpenGLShader vs (QOpenGLShader::Vertex, this);
  fs.compileSourceFile("./shaders/basicShader.frag");
  vs.compileSourceFile("./shaders/basicShader.vert");
  
  program = new QOpenGLShaderProgram(this);
  program->addShader(&fs);
  program->addShader(&vs);
  program->link();
  program->bind();

  vertexLoc = glGetAttribLocation (program->programId(), "vertex");
  colorLoc = glGetAttribLocation (program->programId(), "color");
  transLoc = glGetUniformLocation (program->programId(), "TG");
  projLoc = glGetUniformLocation (program->programId(), "proj");
  viewLoc = glGetUniformLocation (program->programId(), "view");
}
