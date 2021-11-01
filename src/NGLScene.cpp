#include <QMouseEvent>
#include <QGuiApplication>

#include "NGLScene.h"
#include <ngl/NGLInit.h>
#include <ngl/VAOPrimitives.h>
#include <ngl/ShaderLib.h>
#include <ngl/Random.h>
#include <array>
#include <algorithm>



NGLScene::NGLScene(int _numObjects)
{
  setTitle("Selection of Objects");
  m_numObjects=_numObjects;
  m_displayMode=true;
  m_objectArray.resize(m_numObjects);
  std::generate(std::begin(m_objectArray),std::end(m_objectArray),[this](){
     return SelectionObject(ngl::Random::getRandomPoint()*10);
   });
  
}


NGLScene::~NGLScene()
{
  std::cout<<"Shutting down NGL, removing VAO's and Shaders\n";
}



void NGLScene::resizeGL(int _w , int _h)
{
  m_project=ngl::perspective(45.0f,static_cast<float>(_w)/_h,0.05f,350.0f);
  m_win.width  = static_cast<int>( _w * devicePixelRatio() );
  m_win.height = static_cast<int>( _h * devicePixelRatio() );

}
void NGLScene::initializeGL()
{
  // we must call this first before any other GL commands to load and link the
  // gl commands from the lib, if this is not done program will crash
  ngl::NGLInit::initialize();

  glClearColor(0.4f, 0.4f, 0.4f, 1.0f);			   // Grey Background
  glViewport(0,0,m_win.width,m_win.height);
  // enable depth testing for drawing
  glEnable(GL_DEPTH_TEST);
  // enable multisampling for smoother drawing
  glEnable(GL_MULTISAMPLE);
  // Now we will create a basic Camera from the graphics library
  // This is a static camera so it only needs to be set once
  // First create Values for the camera position
  ngl::Vec3 from(0.0f,15.0f,25.0f);
  ngl::Vec3 to(0.0f,0.0f,0.0f);
  ngl::Vec3 up(0.0f,1.0f,0.0f);
  m_view=ngl::lookAt(from,to,up);
  // set the shape using FOV 45 Aspect Ratio based on Width and Height
  // The final two are near and far clipping planes of 0.5 and 10
  m_project=ngl::perspective(45.0f,720.0f/576.0f,0.5f,150.0f);
  ngl::ShaderLib::use(ngl::nglDiffuseShader);
  ngl::ShaderLib::setUniform("Colour",1.0f,1.0f,1.0f,1.0f);

  ngl::ShaderLib::use(ngl::nglDiffuseShader);
  ngl::ShaderLib::setUniform("Colour",1.0f,1.0f,0.0f,1.0f);
  ngl::ShaderLib::setUniform("lightPos",1.0f,1.0f,1.0f);
  ngl::ShaderLib::setUniform("lightDiffuse",1.0f,1.0f,1.0f,1.0f);


  // as re-size is not explicitly called we need to do this.
  glViewport(0, 0, width() * devicePixelRatio(), height() * devicePixelRatio());
  std::cout<<devicePixelRatio()<<"\n";
}



void NGLScene::paintGL()
{
  // clear the screen and depth buffer
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  // Rotation based on the mouse position for our global transform
   ngl::Mat4 rotX;
   ngl::Mat4 rotY;
   // create the rotation matrices
   rotX.rotateX(m_win.spinXFace);
   rotY.rotateY(m_win.spinYFace);
   // multiply the rotations
   m_mouseGlobalTX=rotY*rotX;
   // add the translations
   m_mouseGlobalTX.m_m[3][0] = m_modelPos.m_x;
   m_mouseGlobalTX.m_m[3][1] = m_modelPos.m_y;
   m_mouseGlobalTX.m_m[3][2] = m_modelPos.m_z;


  if (m_displayMode==true)
  {

    for(SelectionObject s : m_objectArray)
    {
      s.draw(false,"nglDiffuseShader",m_mouseGlobalTX,m_view,m_project);
    }
  }
  else
  {

    for(SelectionObject s : m_objectArray)
    {
      s.draw(true,"nglColourShader",m_mouseGlobalTX,m_view,m_project);
    }
  }

}

//----------------------------------------------------------------------------------------------------------------------
void NGLScene::mouseMoveEvent (QMouseEvent * _event)
{
  // note the method buttons() is the button state when event was called
  // this is different from button() which is used to check which button was
  // pressed when the mousePress/Release event is generated
  if(m_win.rotate && _event->buttons() == Qt::LeftButton)
  {
    int diffx=_event->x()-m_win.origX;
    int diffy=_event->y()-m_win.origY;
    m_win.spinXFace +=  0.5f * diffy;
    m_win.spinYFace +=  0.5f * diffx;
    m_win.origX = _event->x();
    m_win.origY = _event->y();
    update();

  }
        // right mouse translate code
  else if(m_win.translate && _event->buttons() == Qt::RightButton)
  {
    int diffX = (int)(_event->x() - m_win.origXPos);
    int diffY = (int)(_event->y() - m_win.origYPos);
    m_win.origXPos=_event->x();
    m_win.origYPos=_event->y();
    m_modelPos.m_x += INCREMENT * diffX;
    m_modelPos.m_y -= INCREMENT * diffY;
    update();

   }
}


//----------------------------------------------------------------------------------------------------------------------
void NGLScene::mousePressEvent ( QMouseEvent * _event)
{
  // this method is called when the mouse button is pressed in this case we
  // store the value where the maouse was clicked (x,y) and set the Rotate flag to true
  if(_event->button() == Qt::LeftButton)
  {
    m_win.origX = _event->x();
    m_win.origY = _event->y();
    m_win.rotate =true;
  }
  // right mouse translate mode
  else if(_event->button() == Qt::RightButton)
  {
    m_win.origXPos = _event->x();
    m_win.origYPos = _event->y();
    m_win.translate=true;
  }
  doSelection(_event->x(),_event->y());

}

//----------------------------------------------------------------------------------------------------------------------
void NGLScene::mouseReleaseEvent ( QMouseEvent * _event )
{
  // this event is called when the mouse button is released
  // we then set Rotate to false
  if (_event->button() == Qt::LeftButton)
  {
    m_win.rotate=false;
  }
        // right mouse translate mode
  if (_event->button() == Qt::RightButton)
  {
    m_win.translate=false;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NGLScene::wheelEvent(QWheelEvent *_event)
{

  // check the diff of the wheel position (0 means no change)
  if(_event->angleDelta().x() > 0)
  {
    m_modelPos.m_z+=ZOOM;
  }
  else if(_event->angleDelta().x() <0 )
  {
    m_modelPos.m_z-=ZOOM;
  }
  update();
}
//----------------------------------------------------------------------------------------------------------------------

void NGLScene::keyPressEvent(QKeyEvent *_event)
{
  // this method is called every time the main window recives a key event.
  // we then switch on the key value and set the camera in the GLWindow
  switch (_event->key())
  {
  // escape key to quite
  case Qt::Key_Escape : QGuiApplication::exit(EXIT_SUCCESS); break;
  // turn on wirframe rendering
  case Qt::Key_W : glPolygonMode(GL_FRONT_AND_BACK,GL_LINE); break;
  // turn off wire frame
  case Qt::Key_S : glPolygonMode(GL_FRONT_AND_BACK,GL_FILL); break;
  // show full screen
  case Qt::Key_F : showFullScreen(); break;
  // show windowed
  case Qt::Key_N : showNormal(); break;
  case Qt::Key_D : m_displayMode^=true; break;
  default : break;
  }
  // finally update the GLWindow and re-draw
  //if (isExposed())
    update();
}

void NGLScene::doSelection(const int _x, const int _y)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  // Rotation based on the mouse position for our global transform
   ngl::Mat4 rotX;
   ngl::Mat4 rotY;
   // create the rotation matrices
   rotX.rotateX(m_win.spinXFace);
   rotY.rotateY(m_win.spinYFace);
   // multiply the rotations
   m_mouseGlobalTX=rotY*rotX;
   // add the translations
   m_mouseGlobalTX.m_m[3][0] = m_modelPos.m_x;
   m_mouseGlobalTX.m_m[3][1] = m_modelPos.m_y;
   m_mouseGlobalTX.m_m[3][2] = m_modelPos.m_z;

  for(SelectionObject &s : m_objectArray)
  {
    s.draw(true,"nglColourShader",m_mouseGlobalTX,m_view,m_project);
  }

  // get color information from frame buffer
  std::array<unsigned char,3> pixel;
  // get the viweport
  std::array<GLint,4> viewport;
  glGetIntegerv(GL_VIEWPORT, &viewport[0]);
  // read the pixels (1,1 at present but could do wider area)
  glReadPixels(_x*devicePixelRatio(), viewport[3] - _y*devicePixelRatio(), 1, 1, GL_RGB, GL_UNSIGNED_BYTE, &pixel[0]);
  // now loop for each object and see if the colour matches
  // need to use a reference object as we will change the class Active value
  int num=0;
  for(SelectionObject &s : m_objectArray)
  {
    if(s.checkSelectionColour(&pixel[0]) ==true)
      break;
    ++num;
  }
  std::cout<<"num "<<num<<"\n";
  update();
}


