; GUI window back



Object("clRenderState")
{
   Images()
   {
     Texture 0 "textures/interface/panel.bmp"
   }

   DepthTest False
   BlendFunc SRC_ALPHA ONE_MINUS_SRC_ALPHA
   ShaderProgram "Interface/Programs/WindowComposer.sp"
}