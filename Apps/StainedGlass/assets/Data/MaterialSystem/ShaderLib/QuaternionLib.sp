// GLSL quaternion library

vec4 QuatMul( in vec4 q1, in vec4 q2 )
{
   vec3  im = q1.w * q2.xyz + q1.xyz * q2.w + cross ( q1.xyz, q2.xyz );
   vec4  dt = q1 * q2;
   float re = dot ( dt, vec4 ( -1.0, -1.0, -1.0, 1.0 ) );

   return vec4( im, re );
}

vec3 RotateVector( in vec3 p, in vec4 q )
{
//   vec4 temp = QuatMul ( q, vec4 ( p, 0.0 ) );
//   return QuatMul( temp, vec4 ( -q.x, -q.y, -q.z, q.w ) ).xyz;

   return p + 2.0 * cross( q.xyz, cross( q.xyz, p ) + q.w * p );
}

vec4 Slerp( vec4 Q1, vec4 Q2, float Coef )
{
   float cosHalfTheta = dot( Q1.xyz, Q2.xyz ) + Q1.w * Q2.w;

   if ( abs(cosHalfTheta) >= 1.0 ) return Q1;

	if ( cosHalfTheta < 0.0 )
   {
      Q2 = -Q2;
		cosHalfTheta = -cosHalfTheta; 
	}

	float halfTheta = acos(cosHalfTheta);
	float sinHalfTheta = sqrt(1.0 - cosHalfTheta*cosHalfTheta);

	if ( abs(sinHalfTheta) < 0.001) return 0.5 * ( Q1+Q2 );

	float ratioA = sin((1 - Coef) * halfTheta) / sinHalfTheta;
	float ratioB = sin(Coef * halfTheta) / sinHalfTheta; 

   return ratioA * Q1 + ratioB * Q2;
}
