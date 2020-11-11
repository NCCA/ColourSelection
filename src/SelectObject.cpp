#include "SelectObject.h"
#include <ngl/ShaderLib.h>
#include <ngl/VAOPrimitives.h>
#include <ngl/NGLStream.h>
// set the initial colour to 0
unsigned char SelectionObject::s_gColourID[3] = {0, 0, 0};

SelectionObject::SelectionObject(ngl::Vec3 pos)
{
  // set the initial values
  m_pos=pos;
  m_active=false;
  // generate an automatic colour id value for selection
  // grab the current active colour id values
  m_colourID[0] = s_gColourID[0];
  m_colourID[1] = s_gColourID[1];
  m_colourID[2] = s_gColourID[2];
  // now we need to increment the values for the next one
  //1 add to the red chan value
  ++s_gColourID[0];
  // if we are at full value
  if(s_gColourID[0] == 255)
  {
    // set red channel to 0
    s_gColourID[0] = 0;
    // inc blue channel
    ++s_gColourID[1];
  // if full go to next
    if(s_gColourID[1] == 255)
    {
      s_gColourID[1] = 0;
      ++s_gColourID[2];
    }
  }
}

bool SelectionObject::checkSelectionColour(const unsigned char col[3])
{

  // see if the colour passed in is the same as our objects one
  if(m_colourID[0] == col[0] && m_colourID[1] == col[1] && m_colourID[2] == col[2])
  {
   m_active^=true;
   return true;
  }
  return false;
}


void SelectionObject::loadMatricesToShader(ngl::Transformation &_tx, const ngl::Mat4 &_globalTx, const std::string &_name, const ngl::Mat4 &_view, const ngl::Mat4 &_project)
{
  ngl::ShaderLib::use(_name);

  ngl::Mat4 MV;
  ngl::Mat4 MVP;
  ngl::Mat3 normalMatrix;
  ngl::Mat4 M;
  M=_globalTx*_tx.getMatrix();
  MV=_view * M ;
  MVP=_project * MV ;
  normalMatrix=MV;
  normalMatrix.inverse().transpose();
  ngl::ShaderLib::setUniform("MVP",MVP);
  ngl::ShaderLib::setUniform("normalMatrix",normalMatrix);
}

void SelectionObject::loadMatricesToColourShader(
                                              ngl::Transformation &_tx,
                                              const ngl::Mat4 &_globalTx,
                                              const std::string &_name,
                                              const ngl::Mat4 &_view, const ngl::Mat4 &_project
                                              )
{
  ngl::ShaderLib::use(_name);
  ngl::Mat4 MV;
  ngl::Mat4 MVP;

  MV=_view  * _globalTx*_tx.getMatrix();
  MVP=_project*MV;
  ngl::ShaderLib::setUniform("MVP",MVP);
}



void SelectionObject::draw(bool _selection,const std::string &_shaderName,const ngl::Mat4 &_globalTx,const ngl::Mat4 &_view, const ngl::Mat4 &_project )
{
  // grab the VBO instance
  ngl::ShaderLib::use(_shaderName);

  ngl::Transformation t;
  t.setPosition(m_pos);
  if(_selection)
  {
    ngl::ShaderLib::setUniform("Colour",(float)m_colourID[0]/255.0f,(float) m_colourID[1]/255.0f,(float)m_colourID[2]/255.0f,1.0f);

    loadMatricesToColourShader(t,_globalTx,_shaderName,_view,_project);
  }
  else
  {
    loadMatricesToShader(t,_globalTx,_shaderName,_view,_project);
  }
  if(m_active == true)
  {
    // draw a teapot
    ngl::VAOPrimitives::draw("teapot");
  }
  else
  {
    // draw a cube
    ngl::VAOPrimitives::draw("cube");
  }
}
