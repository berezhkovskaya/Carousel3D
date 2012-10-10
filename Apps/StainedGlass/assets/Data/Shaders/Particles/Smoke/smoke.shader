; Bubble

Object("clRenderState")
{
   RenderingOrder 5
   Images()
   {
      Texture 0 "textures/particles/smoke.bmp"
   }
   DepthWrites False
   BlendFunc SRC_ALPHA ONE_MINUS_SRC_ALPHA
   ShaderProgram "Shaders/Particles/Smoke/Smoke.sp"
}