; Bubble



Object("clRenderState")
{
   RenderingOrder 5
   Images()
   {
      Texture 0 "textures/particles/raindrop.bmp"
   }
;      DepthTest
   DepthWrites False
   BlendFunc SRC_ALPHA ONE_MINUS_SRC_ALPHA
   ShaderProgram "Shaders/Particles/Rain/Rain.sp"
}