; GUI cursor



Object("clRenderState")
{
;   IterateTextures 9 16 1

   Images()
   {
      Texture 0 "textures/cursors/SizeNWSE/SizeNWSE_p%i.bmp"
   }

   DepthTest False
   BlendFunc SRC_ALPHA ONE_MINUS_SRC_ALPHA
   ShaderProgram "Interface/Programs/Cursor.sp"
}