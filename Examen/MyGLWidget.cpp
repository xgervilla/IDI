#include "MyGLWidget.h"

MyGLWidget::MyGLWidget (QWidget* parent) : QOpenGLWidget(parent), program(NULL)
{
  setFocusPolicy(Qt::StrongFocus);  // per rebre events de teclat
  xClick = yClick = 0;
  DoingInteractive = NONE;
}

MyGLWidget::~MyGLWidget ()
{
  if (program != NULL)
    delete program;
}

void MyGLWidget::initializeGL ()
{
  // Cal inicialitzar l'ús de les funcions d'OpenGL
  initializeOpenGLFunctions();  

  glClearColor(0.05, 0.3, 0.1, 1.0); // defineix color de fons (d'esborrat)
  glEnable(GL_DEPTH_TEST);
  carregaShaders();
  iniEscena ();
  iniCamera ();
}

void MyGLWidget::iniEscena ()
{
  creaBuffersPatricio();
  creaBuffersPilota();
  creaBuffersTerra();

  centreEsc = glm::vec3 (2, 2, 2);
  radiEsc = 4;
}

void MyGLWidget::iniCamera ()
{
  rav = 1.0;
  angleY = 0.0;
  angleX = M_PI/6.0;

  projectTransform ();
  viewTransform ();
}

void MyGLWidget::paintGL () 
{
  // Aquest codi és necessari únicament per a MACs amb pantalla retina.
#ifdef __APPLE__
  GLint vp[4];
  glGetIntegerv (GL_VIEWPORT, vp);
  ample = vp[2];
  alt = vp[3];
#endif

  // En cas de voler canviar els paràmetres del viewport, descomenteu la crida següent i
  // useu els paràmetres que considereu (els que hi ha són els de per defecte)
  //  glViewport (0, 0, ample, alt);
  
  // Esborrem el frame-buffer i el depth-buffer
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  //--------------------------------------------------------
  // Activem el VAO per pintar el terra
  glBindVertexArray (VAO_Terra);  
  // pintem terra
  modelTransformTerra ();
  glDrawArrays(GL_TRIANGLES, 0, 12);

  //--------------------------------------------------------
  // Activem el VAO per pintar la Pilota
  glBindVertexArray (VAO_Pil);
  // pintem la Pilota
  modelTransformPilota();
  glDrawArrays(GL_TRIANGLES, 0, pilModel.faces().size()*3);
  
  //--------------------------------------------------------
  // Activem el VAO per pintar el Patricio
  glBindVertexArray (VAO_Pat);
  // pintem el Patricio
  modelTransformPatricio();
  glDrawArrays(GL_TRIANGLES, 0, patModel.faces().size()*3);

  //--------------------------------------------------------
  glBindVertexArray(0);
}

void MyGLWidget::resizeGL (int w, int h)
{
  ample = w;
  alt = h;

  projectTransform ();
}

void MyGLWidget::modelTransformTerra ()
{
  glm::mat4 TG(1.f);  // Matriu de transformació
  glUniformMatrix4fv (transLoc, 1, GL_FALSE, &TG[0][0]);
}

void MyGLWidget::modelTransformPatricio ()
{
  // Codi per a la TG necessària
  glm::mat4 patTG = glm::mat4(1.0f);
  patTG = glm::scale(patTG, glm::vec3(escalaPat, escalaPat, escalaPat));
  patTG = glm::translate(patTG, -centreBasePat);
  glUniformMatrix4fv(transLoc, 1, GL_FALSE, &patTG[0][0]);
}

void MyGLWidget::modelTransformPilota ()
{
  glm::mat4 pilTG = glm::mat4(1.0f);
  pilTG = glm::scale(pilTG, glm::vec3(0.5*escalaPil, 0.5*escalaPil, 0.5*escalaPil));
  pilTG = glm::translate(pilTG, -centreBasePil);
  glUniformMatrix4fv(transLoc, 1, GL_FALSE, &pilTG[0][0]);
}

void MyGLWidget::projectTransform ()
{
  float fov, zn, zf;
  glm::mat4 Proj;  // Matriu de projecció
  
  fov = float(M_PI/3.0);
  zn = 6;
  zf = 15;

  Proj = glm::perspective(fov, rav, zn, zf);

  glUniformMatrix4fv (projLoc, 1, GL_FALSE, &Proj[0][0]);
}

void MyGLWidget::viewTransform ()
{
  // Matriu de posició i orientació
  View = glm::translate(glm::mat4(1.f), glm::vec3(0, 0, -2*radiEsc));
  View = glm::rotate(View, angleX, glm::vec3(1, 0, 0));
  View = glm::rotate(View, -angleY, glm::vec3(0, 1, 0));

  glUniformMatrix4fv (viewLoc, 1, GL_FALSE, &View[0][0]);
}

void MyGLWidget::keyPressEvent(QKeyEvent* event)
{
  makeCurrent();
  switch (event->key()) {
    default: event->ignore(); break;
  }

  update();
}

void MyGLWidget::mousePressEvent (QMouseEvent *e)
{
  xClick = e->x();
  yClick = e->y();

  if (e->button() & Qt::LeftButton &&
      ! (e->modifiers() & (Qt::ShiftModifier|Qt::AltModifier|Qt::ControlModifier)))
  {
    DoingInteractive = ROTATE;
  }
}

void MyGLWidget::mouseReleaseEvent( QMouseEvent *)
{
  DoingInteractive = NONE;
}

void MyGLWidget::mouseMoveEvent(QMouseEvent *e)
{
  makeCurrent();
  if (DoingInteractive == ROTATE)
  {
    // Fem la rotació
    angleY += (e->x() - xClick) * M_PI / 180.0;    
    angleX += (yClick - e->y()) * M_PI / 180.0;    
    viewTransform ();
  }

  xClick = e->x();
  yClick = e->y();

  update ();
}

void MyGLWidget::calculaCapsaModel (Model &m, float &escala, glm::vec3 &centreBase)
{
  // Càlcul capsa contenidora i valors transformacions inicials
  float minx, miny, minz, maxx, maxy, maxz;
  minx = maxx = m.vertices()[0];
  miny = maxy = m.vertices()[1];
  minz = maxz = m.vertices()[2];
  for (unsigned int i = 3; i < m.vertices().size(); i+=3)
  {
    if (m.vertices()[i+0] < minx)
      minx = m.vertices()[i+0];
    if (m.vertices()[i+0] > maxx)
      maxx = m.vertices()[i+0];
    if (m.vertices()[i+1] < miny)
      miny = m.vertices()[i+1];
    if (m.vertices()[i+1] > maxy)
      maxy = m.vertices()[i+1];
    if (m.vertices()[i+2] < minz)
      minz = m.vertices()[i+2];
    if (m.vertices()[i+2] > maxz)
      maxz = m.vertices()[i+2];
  }
  escala = 1.0/(maxy-miny);
  centreBase[0] = (minx+maxx)/2.0;
  centreBase[1] = miny;
  centreBase[2] = (minz+maxz)/2.0;
}

void MyGLWidget::creaBuffersPatricio() {
  creaBuffersModel(patModel, "./models/Patricio.obj", &VAO_Pat, escalaPat, centreBasePat);
}

void MyGLWidget::creaBuffersPilota() {
  creaBuffersModel(pilModel, "./models/Pilota.obj", &VAO_Pil, escalaPil, centreBasePil);
}

void MyGLWidget::creaBuffersModel(Model &model, const char *fileName,
				  GLuint *VAO,  float &escala,
				  glm::vec3 &centreBase)
{
  // Carreguem el model de l'OBJ - Atenció! Abans de crear els buffers!
  model.load(fileName);

  // Calculem la capsa contenidora del model
  calculaCapsaModel(model, escala, centreBase);
  
  // Creació del Vertex Array Object del model
  glGenVertexArrays(1, VAO);
  glBindVertexArray(*VAO);

  // Creació dels buffers del model
  GLuint VBO_Model[6];
  // Buffer de posicions
  glGenBuffers(6, VBO_Model);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Model[0]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*model.faces().size()*3*3, model.VBO_vertices(), GL_STATIC_DRAW);

  // Activem l'atribut vertexLoc
  glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(vertexLoc);

  // Buffer de normals
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Model[1]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*model.faces().size()*3*3, model.VBO_normals(), GL_STATIC_DRAW);

  glVertexAttribPointer(normalLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(normalLoc);

  // Buffer de component ambient
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Model[2]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*model.faces().size()*3*3, model.VBO_matamb(), GL_STATIC_DRAW);

  glVertexAttribPointer(matambLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(matambLoc);

  // Buffer de component difusa
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Model[3]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*model.faces().size()*3*3, model.VBO_matdiff(), GL_STATIC_DRAW);

  glVertexAttribPointer(matdiffLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(matdiffLoc);

  // Buffer de component especular
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Model[4]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*model.faces().size()*3*3, model.VBO_matspec(), GL_STATIC_DRAW);

  glVertexAttribPointer(matspecLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(matspecLoc);

  // Buffer de component shininness
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Model[5]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*model.faces().size()*3, model.VBO_matshin(), GL_STATIC_DRAW);

  glVertexAttribPointer(matshinLoc, 1, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(matshinLoc);

  glBindVertexArray(0);
}

void MyGLWidget::creaBuffersTerra ()
{
  // VBO amb la posició dels vèrtexs
  glm::vec3 posterra[6] = {
    glm::vec3(0.0, 0.0, 3.0),
    glm::vec3(3.0, 0.0, 3.0),
    glm::vec3(0.0, 0.0, 0.0),
    glm::vec3(0.0, 0.0, 0.0),
    glm::vec3(3.0, 0.0, 3.0),
    glm::vec3(3.0, 0.0, 0.0)
  }; 

  // VBO amb la normal de cada vèrtex
  glm::vec3 norm1 (0,1,0);
  glm::vec3 norm2 (0,0,1);
  glm::vec3 normterra[12] = {
    norm1, norm1, norm1, norm1, norm1, norm1, // la normal (0,1,0) per als primers dos triangles
    norm2, norm2, norm2, norm2, norm2, norm2  // la normal (0,0,1) per als dos últims triangles
  };

  // Definim el material del terra
  glm::vec3 amb(0.150000, 0.15000 ,0.100000);
  glm::vec3 diff(0.600000, 0.600000 ,0.400000);
  glm::vec3 spec(0.7,0.7,0.7);
  float shin = 100;

  // Fem que aquest material afecti a tots els vèrtexs per igual
  glm::vec3 matamb[12] = {
    amb, amb, amb, amb, amb, amb, amb, amb, amb, amb, amb, amb
  };
  glm::vec3 matdiff[12] = {
    diff, diff, diff, diff, diff, diff, diff, diff, diff, diff, diff, diff
  };
  glm::vec3 matspec[12] = {
    spec, spec, spec, spec, spec, spec, spec, spec, spec, spec, spec, spec
  };
  float matshin[12] = {
    shin, shin, shin, shin, shin, shin, shin, shin, shin, shin, shin, shin
  };

// Creació del Vertex Array Object del terra
  glGenVertexArrays(1, &VAO_Terra);
  glBindVertexArray(VAO_Terra);

  GLuint VBO_Terra[6];
  glGenBuffers(6, VBO_Terra);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Terra[0]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(posterra), posterra, GL_STATIC_DRAW);

  // Activem l'atribut vertexLoc
  glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(vertexLoc);

  glBindBuffer(GL_ARRAY_BUFFER, VBO_Terra[1]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(normterra), normterra, GL_STATIC_DRAW);

  // Activem l'atribut normalLoc
  glVertexAttribPointer(normalLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(normalLoc);

  // Buffer de component ambient
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Terra[2]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(matamb), matamb, GL_STATIC_DRAW);

  glVertexAttribPointer(matambLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(matambLoc);

  // Buffer de component difusa
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Terra[3]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(matdiff), matdiff, GL_STATIC_DRAW);

  glVertexAttribPointer(matdiffLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(matdiffLoc);

  // Buffer de component especular
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Terra[4]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(matspec), matspec, GL_STATIC_DRAW);

  glVertexAttribPointer(matspecLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(matspecLoc);

  // Buffer de component shininness
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Terra[5]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(matshin), matshin, GL_STATIC_DRAW);

  glVertexAttribPointer(matshinLoc, 1, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(matshinLoc);

  glBindVertexArray(0);
}

void MyGLWidget::carregaShaders()
{
  // Creem els shaders per al fragment shader i el vertex shader
  QOpenGLShader fs (QOpenGLShader::Fragment, this);
  QOpenGLShader vs (QOpenGLShader::Vertex, this);
  // Carreguem el codi dels fitxers i els compilem
  fs.compileSourceFile("./shaders/basicLlumShader.frag");
  vs.compileSourceFile("./shaders/basicLlumShader.vert");
  // Creem el program
  program = new QOpenGLShaderProgram(this);
  // Li afegim els shaders corresponents
  program->addShader(&fs);
  program->addShader(&vs);
  // Linkem el program
  program->link();
  // Indiquem que aquest és el program que volem usar
  program->bind();

  // Obtenim identificador per a l'atribut “vertex” del vertex shader
  vertexLoc = glGetAttribLocation (program->programId(), "vertex");
  // Obtenim identificador per a l'atribut “normal” del vertex shader
  normalLoc = glGetAttribLocation (program->programId(), "normal");
  // Obtenim identificador per a l'atribut “matamb” del vertex shader
  matambLoc = glGetAttribLocation (program->programId(), "matamb");
  // Obtenim identificador per a l'atribut “matdiff” del vertex shader
  matdiffLoc = glGetAttribLocation (program->programId(), "matdiff");
  // Obtenim identificador per a l'atribut “matspec” del vertex shader
  matspecLoc = glGetAttribLocation (program->programId(), "matspec");
  // Obtenim identificador per a l'atribut “matshin” del vertex shader
  matshinLoc = glGetAttribLocation (program->programId(), "matshin");

  // Demanem identificadors per als uniforms dels shaders
  transLoc = glGetUniformLocation (program->programId(), "TG");
  projLoc = glGetUniformLocation (program->programId(), "proj");
  viewLoc = glGetUniformLocation (program->programId(), "view");
  posFocusLoc = glGetUniformLocation (program->programId(), "posFocus");
  colFocusLoc = glGetUniformLocation (program->programId(), "colFocus");
}

