// GPU skinning utils

vec4 GetPoint( int X, int Y )
{
// This is for nVidia
//   return texelFetch( Texture0, ivec2( X, Y ), 0 );

// This is for AMD...
   ivec2 Size = textureSize( Texture0, 0 );

   return textureLod( Texture0, vec2( ( float(X) + 0.5 ) / float(Size.x), ( float(Y) + 0.5 ) / float(Size.y) ), 0 );
}

vec4 SkinVertex()
{
   vec3 Pos = vec3( 0.0, 0.0, 0.0 );

   int WeightIndex = int(in_Bones.x);
   int WeightCount = int(in_Bones.y);

   int VStart1 = int(KeyFrame.x) * 4;
   int VStart2 = int(KeyFrame.y) * 4;

   float LerpCoef = KeyFrame.z;
   
   for ( int k = 0; k < WeightCount; k++ )
   {
      vec4  PosWeight  =     GetPoint( WeightIndex + k, VStart1 + 0 );
      int   JointIndex = int(GetPoint( WeightIndex + k, VStart1 + 1 ).x);

      // extract current keyframe joint
      vec4 J1Orientation = GetPoint( JointIndex, VStart1 + 2 );
      vec3 J1Position    = GetPoint( JointIndex, VStart1 + 3 ).xyz;

      // extract next keyframe joint
      vec4 J2Orientation = GetPoint( JointIndex, VStart2 + 2 );
      vec3 J2Position    = GetPoint( JointIndex, VStart2 + 3 ).xyz;

      // SLERP blending of current and next keyframes
      vec4 JOrientation = Slerp( J1Orientation, J2Orientation, LerpCoef );
      vec3 JPosition    = mix(   J1Position,    J2Position,    LerpCoef );

      Pos += ( RotateVector( PosWeight.xyz, JOrientation ) + JPosition ) * PosWeight.w;
   }

   return vec4( Pos, 1.0 );
}