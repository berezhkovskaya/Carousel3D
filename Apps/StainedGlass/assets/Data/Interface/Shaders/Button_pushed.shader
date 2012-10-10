; GUI button pushed



Object("clRenderState")
{
   Images()
   {
      Texture 0 "Textures/Interface/Button_pushed.png"
   }
   DepthTest False
   BlendFunc SRC_ALPHA ONE_MINUS_SRC_ALPHA
   ShaderProgram "Interface/Programs/GUI.sp"
}