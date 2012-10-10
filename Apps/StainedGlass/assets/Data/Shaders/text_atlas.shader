;
; "Text_Shader"



Object("clRenderState")
{
   DepthTest False
	BlendFunc SRC_ALPHA ONE_MINUS_SRC_ALPHA
   ShaderProgram "text_atlas.sp"
}
