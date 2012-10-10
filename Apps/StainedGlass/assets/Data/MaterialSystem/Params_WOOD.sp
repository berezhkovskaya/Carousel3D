//

vec4 GetAmbient()
{
   float Ka = 0.2;

   return vec4( Ka );
}

vec4 GetDiffuse()
{
   float Kd = 0.89;

   vec4  darkWood      = vec4(0.44,     0.210526, 0.0,      1.0);
   vec4  liteWood      = vec4(0.917293, 0.503759, 0.127820, 1.0);
   float frequency     = 6.72;
   float noiseScale    = 0.96;
   float ringSharpness = 21.0;

   vec3 vScaledPosition = LocalPosition.xzy * 0.05;

   float snoise = 2.0 * texture(Texture5, vScaledPosition).x - 1.0;
   /* Rings goes along z axis, perturbed with some noise */
   float r = fract(frequency * vScaledPosition.z + noiseScale * snoise);

   float invMax = pow(ringSharpness, ringSharpness / (ringSharpness - 1.0)) / (ringSharpness - 1.0);
   float ring = invMax * (r - pow(r, ringSharpness));

   /* Add some noise and get base color */
   float lrp = ring + snoise;
   vec4  base = mix(darkWood, liteWood, lrp);

   return base * Kd;
}

vec4 GetSpecular()
{
   float Ks = 0.66;

   return vec4( Ks );
}