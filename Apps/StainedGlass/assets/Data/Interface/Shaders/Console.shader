; This shader emulates Quake3-like animated console



Object("clRenderState")
{
   Images()
   {
		Texture 0 "Textures/Interface/Console1.png"
   }

   DepthTest False
   BlendFunc SRC_ALPHA ONE_MINUS_SRC_ALPHA
   ShaderProgram "Interface/Programs/Console.sp"
}