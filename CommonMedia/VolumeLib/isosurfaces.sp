/*VERTEX_PROGRAM*/
#include "Layout.sp"

in vec4 in_Vertex;

out vec4 Pos;
out vec3 EyeRay_Origin;
out vec3 EyeRay_Dir;

void main()
{
   gl_Position = in_ModelViewProjectionMatrix * in_Vertex;

   Pos = in_ModelViewProjectionMatrix * in_Vertex;

   EyeRay_Origin = ( in_ModelViewMatrixInverse * vec4(0.0, 0.0, 0.0, 1.0) ).xyz;
   // direction
   EyeRay_Dir = in_Vertex.xyz - EyeRay_Origin;
}

/*FRAGMENT_PROGRAM*/

uniform float SURFACE_INDEX;

in vec4  Pos;
in vec3  EyeRay_Origin;
in vec3  EyeRay_Dir;

out vec4 out_FragColor;

//uniform float ENGINE_TIME;

float iso_cube(vec3 p)
{
   vec3 p2 =  p * p;
   vec3 p4 = p2 * p2;
   vec3 p6 = p2 * p4;

   return (p6.x + p6.y + p6.z) - 0.1;
}

float iso_daisy(vec3 p)
{
  vec3 p2 = p * p;

  float d1 = (p2.x - p2.y * p.y);
  float d2 = (p2.z - p2.y);

  return (d1*d1 - d2*d2*d2);
}

float iso_distel(vec3 p)
{
  // x2+y2+z2+1000 (x2+y2)(x2+z2)(y2+z2)=1
  vec3 p2 = p*p;

  return (p2.x + p2.y + p2.z) + 1000.0 * (p2.x + p2.y) * (p2.x + p2.z) * (p2.y + p2.z) - 1.0;
}

float iso_eistute(vec3 p)
{
  vec3 p1 = p * 3.0;
  // (x2+y2)3 = 4x2y2(z2+1)
  vec3 p2 = p1*p1;

  float d = p2.x + p2.y;
  d = d*d*d;

  return d - 4 * p2.x * p2.y * (p2.z + 1.0);
}

float iso_tobel(vec3 p1)
{
  vec3 p = p1 * 12.0;
  // x3 z + x2 + yz3 + z4 = 3xyz
  vec3 p2 = p*p;

  return p2.x * p.x * p.z + p2.x + p.y * p2.z * p.z + p2.z * p2.z - 3.0 * p.x * p.y * p.z;
}

float iso_visavis(vec3 p1)
{
  vec3 p = p1 * 3.0;
  // x2-x3+y2+y4+z3-z4=0
  vec3 p2 = p*p;

  return p2.x - p2.x * p.x + p2.y + p2.y * p2.y + p2.z * p.z - p2.z * p2.z;
}

float iso_calyx(vec3 p1)
{
  vec3 p = p1 * 3.0;
  // x2+y2z3 = z4
  vec3 p2 = p*p;

  return p2.x + p2.y * p2.z * p.z - p2.z * p.z;
}

float iso_suss(vec3 p1)
{
  vec3 p = p1 * 3.0;

  // (x2+9/4y2+z2-1)3 - x2z3-9/80y2z3=0
  vec3 p2 = p * p;

  float d = (p2.x + (9.0/4.0) * p2.y + p2.z - 1.0);

  return d * d * d - p2.x * p2.z * p.z - (9.0/80.0) * p2.y * p2.z * p.z;
}

float iso_spitz(vec3 p)
{
  vec3 p1 = p;
  // (y3-x2-z2)3 = 27 x2 y3 z2

  vec3 p2 = p*p;

  float d = (p2.y * p.y - p2.x - p2.z);

  return d * d * d - 27.0 * p2.x * p2.y * p.y * p2.z;
}

float iso_leopold(vec3 p)
{
  // 100 x2 y2 z2 + 3x2 + 3y2 + z2 = 1

  vec3 p2 = p * p;

  return 100.0 * p2.x * p2.y * p2.z + 3.0 * p2.x + 3.0 * p2.y + p2.z - 1.0;
}

float iso_sofa(vec3 p)
{
  // x2+y3+z5 = 0
  vec3 p2 = p * p;

  return p2.x + p2.y * p.y + p2.z * p2.z * p.z;
}

float iso_himmel_und_holle(vec3 p)
{
  // x2 - y2 z2 = 0
  vec3 p2 = p * p;

  return p2.x - p2.y * p2.z;
}

float iso_harlekin(vec3 p)
{
  // x3 z + 10 x2 y + x y2 + y z2 = z3
  vec3 p2 = p * p;

  return p2.x * p.x * p.z + 10.0 * p2.x * p.y + p.x * p2.y + p.y * p2.z - p2.z * p.z;
}

float iso_zeck(vec3 p)
{
  // x2+y2-z3(1-z) = 0
  vec3 p2 = p * p;

  return p2.x + p2.y - p2.z * p.z * (1.0 - p.z);
}

float iso_subway(vec3 p1)
{
  vec3 p = p1 * 4.0;
  // x2y2 + (z2-1)3=0

  vec3 p2 = p * p;

  float d = (p2.z - 1.0);

  return p2.x * p2.y + d * d * d;
}

float iso_crixxi(vec3 p)
{
  // (y2+z2-1)2 +(x2+y2-1)3 = 0
  vec3 p2 = p * p;

  float d = p2.y + p2.z - 1.0;
  float w = p2.x + p2.y - 1.0;

  return d * d + w * w * w;
}

float iso_nepali(vec3 p)
{
  vec3 p2 = p*p;

  // (xy-z3-1)2 + (x2+y2-1)3 = 0
  float d = p.x * p.y - p2.z * p.z - 1.0;
  float w = p2.x + p2.y - 1.0;

  return d * d + w * w * w;
}

float iso_trichter(vec3 p)
{
  vec3 p2 = p * p;
  // x2 + z3 = y2z2
  return p2.x + p2.z * p.z - p2.y * p2.z;
}

float iso_pilzchen(vec3 p)
{
  // (z3 - 1)2 + (x2+y2-1)3=0
  vec3 p2 = p * p;

  float d = p2.z * p.z - 1.0;
  float w = p2.x + p2.y - 1.0;

  return d * d + w * w * w;
}

float iso_polsterzipf(vec3 p)
{
  vec3 d = vec3( p.x * p.x * p.x - 1.0, p.y * p.y * p.y - 1.0, p.z * p.z - 1.0);
  vec3 pp = d * d;
//  (x3-1)2 + (y3-1)2+ (z2-1)3 = 0

  return pp.x + pp.y + pp.z * d.z;
}

float iso_berg(vec3 p)
{
  vec3 p2 = p*p;
  // x2+y2z2+z3 = 0
  return p2.x + p2.y * p2.z + p2.z * p.z;
}

float iso_gupf(vec3 p)
{
  // x2+y2+z=0
  return p.x * p.x + p.y * p.y + p.z;
}

float iso_kegel(vec3 p)
{
  return p.x * p.x + p.y * p.y - p.z * p.z;
}	 

float iso_wigwam(vec3 p)
{
  // x2+y2z3=0
  return p.x * p.x + p.y * p.y * p.z * p.z * p.z;
}

float iso_tuelle(vec3 p)
{
//  yz(x2+y-z) = 0
  return p.y * p.z * ( p.x * p.x + p.y - p.z );
}

float iso_pipe(vec3 p)
{
  return p.x * p.x - p.z;
} 	 

float iso_fanfare(vec3 p)
{
  vec3 p2 = p * p;
  // -x3+z2+y2=0
  return -p2.x * p.x + p2.z + p2.y;
}

float iso_kreuz(vec3 p)
{
  return p.x * p.y * p.z;
}

float iso_spindel(vec3 p)
{
  vec3 p2 = p * p;
  // x2+y2-z2=1
  return p2.x + p2.y - p2.z - 1.0;
}

float iso_calypso(vec3 p)
{
  // x2+y2z = z2
  vec3 p2 = p * p;

  return p2.x + p2.y * p.z - p2.z;
}

float iso_columpius(vec3 p)
{
  // x3y+xz3+y3z+z3+7z2+5z=0
  vec3 p2 = p * p;
  vec3 p3 = p2 * p;

  return p3.x * p.y + p.x * p3.z + p3.y * p.z + p3.z + 7.0 * p2.z + 5.0 * p.z;
}

float iso_dattel(vec3 p)
{
  // 3x2+3y2+z2=1
  vec3 p2 = p * p;

  return 3.0 * p2.x + 3.0 * p2.y + p2.z - 1.0;
}

float iso_dingdong(vec3 p)
{
  vec3 p2 = p * p;
  // x2 +y2 +z3 = z2

  return p2.x + p2.y + p2.z * p.z - p2.z;
}

float iso_durchblick(vec3 p)
{
  vec3 p2 = p * p;
  vec3 p3 = p2 * p;
  // x3y+xz3+y3z+z3+5z = 0

  return p2.x * p.x * p.y + p.x * p2.z * p.z + p3.y * p.z + p2.z * p.z + 5.0 * p.z;
}


float f_value(vec3 p)
{
  if(SURFACE_INDEX <= 1.5) return iso_cube(p);
  if(SURFACE_INDEX <= 2.5) return iso_daisy(p);
  if(SURFACE_INDEX <= 3.5) return iso_distel(p);
//  return iso_eistute(p);
//  return iso_tobel(p);
//  return iso_visavis(p);
//  return iso_calyx(p);
//  return iso_suss(p);
//  return iso_spitz(p);
//  return iso_leopold(p);
//  return iso_sofa(p);
//  return iso_himmel_und_holle(p);
//  return iso_harlekin(p);
//  return iso_zeck(p);
//  return iso_subway(p);
//  return iso_crixxi(p);
  return iso_nepali(p);
}

vec3 f_gradient(vec3 p)
{
   float eps = 1.0/64.0;

   float gx1 = f_value(p - vec3(eps, 0.0, 0.0));
   float gx2 = f_value(p/* + vec3(eps, 0.0, 0.0)*/);
   float gy1 = f_value(p - vec3(0.0, eps, 0.0));
   float gy2 = f_value(p/* + vec3(0.0, eps, 0.0)*/);
   float gz1 = f_value(p - vec3(0.0, 0.0, eps));
   float gz2 = f_value(p/* + vec3(0.0, 0.0, eps)*/);

   return normalize(vec3(gx2-gx1, gy2-gy1, gz2-gz1));

}

vec4 TraceIsosurface( vec3 FrontPos, vec3 DeltaDir, int Steps, float MaxLength )
{
   vec3 Vec       = FrontPos;

   float LengthAcc = 0.0;
   float StepSize  = length( DeltaDir );

   float Eps = 0.1;

   vec4 Color;
   for(int i = 0; i < Steps; i++)
   {  

      float vv = f_value(Vec);
      if (  vv > -Eps && vv < +Eps)
      {
         // estimate gradient ... 
         Color = vec4( f_gradient(Vec), 1.0);
         return Color;
      }

      Vec += DeltaDir;
      LengthAcc += StepSize;

      if ( LengthAcc >= MaxLength ) break;
   }

   return vec4(1.0);
}

#include "MaterialSystem/ShaderLib/VolumeRendering.sp"

void main()
{
   // calculate ray intersection with bounding box
   float tnear, tfar;
   Ray EyeRay;
   EyeRay.o = EyeRay_Origin;
   EyeRay.d = normalize( EyeRay_Dir );
   bool hit = IntersectBox( EyeRay, BBoxMin, BBoxMax, tnear, tfar);
   if (!hit) discard;
   if (tnear < 0.0) tnear = 0.0;

   vec2 Coord = ( ( Pos.xy / Pos.w ) + 1.0 ) / 2.0;

   vec3 BackPosition  = EyeRay.o + EyeRay.d * tfar;
   vec3 FrontPosition = EyeRay.o + EyeRay.d * tnear;

   int   Steps = 512; //int(TRACING_STEPS);
   float StepSize =  1.0 / Steps;

   // DO NOT convert to texture space (keep in -1..+1)
//   BackPosition  = BackPosition *  0.5 + vec3( 0.5 );
//   FrontPosition = FrontPosition * 0.5 + vec3( 0.5 );

   vec3 Dir = BackPosition - FrontPosition;
   vec3 NormDir = normalize( Dir );
   float Length = length( Dir );

   float Delta = StepSize;
   vec3 DeltaDir = NormDir * Delta;

   out_FragColor = TraceIsosurface( FrontPosition, DeltaDir, Steps, Length );
}
