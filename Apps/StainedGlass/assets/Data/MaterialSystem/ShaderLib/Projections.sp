// Projections utils

mat4 GetProjScaleBiasMat()
{ 
   // transform from -1..1 to 0..1
   return mat4( 0.5, 0.0, 0.0, 0.0, 
                0.0, 0.5, 0.0, 0.0, 
                0.0, 0.0, 0.5, 0.0, 
                0.5, 0.5, 0.5, 1.0 );
}
