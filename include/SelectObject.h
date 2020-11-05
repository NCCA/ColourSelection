#ifndef SELECT_OBJECT_H_
#define SELECT_OBJECT_H_

#include <ngl/Transformation.h>
#include <ngl/Vec3.h>

//----------------------------------------------------------------------------------------------------------------------
/// @brief a class to hold a selection object for OpenGL Selection */
/// this example is modified from
/// http://gpwiki.org/index.php/OpenGL_Selection_Using_Unique_Color_IDs
/// beware their version is not the best but serves as a good basis
//----------------------------------------------------------------------------------------------------------------------
class SelectObject
{
  public :
  SelectObject()=default;

  //----------------------------------------------------------------------------------------------------------------------
  /// @brief ctor
  /// @param[in] _pos the position of the object
  //----------------------------------------------------------------------------------------------------------------------
  SelectObject(ngl::Vec3 _pos);
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief draw method
  /// @param[in] _selection flag to draw with colour for selection shader
  /// @param[in] _shaderName the name of the shader to use when drawing
  /// @param[in] _transformStack the name of the transformation stack for drawing
  //----------------------------------------------------------------------------------------------------------------------
  void draw(bool _selection,const std::string &_shaderName,const ngl::Mat4 &_globalTx,const ngl::Mat4 &_view, const ngl::Mat4 &_project );
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief check the current object againt the colour values passed in
  /// @param[in] _col[3] rgb values to check agains object colour
  //----------------------------------------------------------------------------------------------------------------------

  bool checkSelectionColour(const unsigned char col[3]);
 private:
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief the colour id for the selection
    //----------------------------------------------------------------------------------------------------------------------
    unsigned char m_colourID[3];
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief container for  the next colour id.
    //----------------------------------------------------------------------------------------------------------------------
    static unsigned char s_gColourID[3];
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief the position of the  object
    //----------------------------------------------------------------------------------------------------------------------
    ngl::Vec3 m_pos;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief indicates if the point is selected or not
    //----------------------------------------------------------------------------------------------------------------------
    bool m_active;
    void loadMatricesToColourShader( ngl::Transformation &_tx,const ngl::Mat4 &_globalTx,const std::string &_name,const ngl::Mat4 &_view, const ngl::Mat4 &_project ) ;
    void loadMatricesToShader( ngl::Transformation &_tx,const ngl::Mat4 &_globalTx,const std::string &_name,const ngl::Mat4 &_view, const ngl::Mat4 &_project ) ;

};


#endif
