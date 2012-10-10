; GUI panel



Object("clRenderState")
{
   Images()
   {
     Texture 0 "Textures/Interface/input_line.png"
   }

   DepthTest False
   BlendFunc SRC_ALPHA ONE_MINUS_SRC_ALPHA
   ShaderProgram "Interface/Programs/GUI.sp"
}