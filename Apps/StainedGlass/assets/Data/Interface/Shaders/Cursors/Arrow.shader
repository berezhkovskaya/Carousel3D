; GUI cursor



Object("clRenderState")
{
;   IterateTextures 
   Images()
   {
      Texture 0 "textures/cursors/arrow/Arrow(Blue)_p%i.bmp" 12 16 2
   }
   DepthTest False
   BlendFunc SRC_ALPHA ONE_MINUS_SRC_ALPHA
   ShaderProgram "Interface/Programs/Cursor.sp"
}
