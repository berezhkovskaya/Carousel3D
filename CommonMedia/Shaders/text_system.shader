;
; "Text_Shader"



Object("clRenderState")
{
   Images()
   { 
     Texture2D 0 "Shaders/text_system.bmp"
   }

   DepthTest False
	BlendFunc SRC_ALPHA ONE_MINUS_SRC_ALPHA
   ShaderProgram "Shaders/text_system.sp"
}
