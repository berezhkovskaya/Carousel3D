/*VERTEX_PROGRAM*/

#include Layout.sp

in vec4 in_Vertex;
in vec4 in_TexCoord;

out vec2 TexCoord0;

void main()
{
   TexCoord0 = vec2( in_TexCoord );

   gl_Position = in_ModelViewProjectionMatrix * in_Vertex;
}

/*FRAGMENT_PROGRAM*/

in vec2 TexCoord0;

uniform sampler2D Texture0;     /* Color                     */
uniform sampler2D Texture1;     /* Avg. luminance            */
uniform sampler2D Texture2;     /* Bloom                     */

uniform float BloomStrength;
uniform float MidGray;
uniform float White;

out vec4 out_FragColor;

void main()
{
  /* Reinhard tone mapping operator */
                    
  float AvgLuminance = texture( Texture1, vec2(0.5, 0.5) ).x;

  vec4  Color        = vec4( texture(Texture0, TexCoord0) );
  vec4  Bloom        = vec4( texture(Texture2, TexCoord0) );

  float Exposure = MidGray / ( AvgLuminance + 0.001 );

  Color *= Exposure; 

  Color = Color * ( 1.0 + Color / (White*White) ) / ( 1.0 + Color );

  out_FragColor = Color + BloomStrength * Bloom;
}

/*
float4 PS_Reinhard02( VSOutput input ) : COLOR
{
  // Get color of the pixel
  float4 color = tex2D( sceneTextureSampler, input.TexCoords );  	
  // Get lumianance values from 1x1 lumianance texture
  float4 luminanceSample = tex2D( luminanceTextureSampler, float2( 0.5, 0.5 ));
  // Average scene luminance is stored in R channel
  float avgLuminance = luminanceSample.r; 

  // RGB -> XYZ conversion
  const float3x3 RGB2XYZ = {0.5141364, 0.3238786,  0.16036376,
                             0.265068,  0.67023428, 0.06409157,
                             0.0241188, 0.1228178,  0.84442666};				                    
  float3 XYZ = mul(RGB2XYZ, color.rgb);
  
  // XYZ -> Yxy conversion
  float3 Yxy;
  Yxy.r = XYZ.g;                            // copy luminance Y
  Yxy.g = XYZ.r / (XYZ.r + XYZ.g + XYZ.b ); // x = X / (X + Y + Z)
  Yxy.b = XYZ.g / (XYZ.r + XYZ.g + XYZ.b ); // y = Y / (X + Y + Z)
    
  // (Lp) Map average luminance to the middlegrey zone by scaling pixel luminance
  float Lp = Yxy.r * exposure / avgLuminance;                       
  // (Ld) Scale all luminance within a displayable range of 0 to 1
  Yxy.r = (Lp * (1.0 + Lp/(whitePoint * whitePoint)))/(1.0 + Lp);
  
  // Yxy -> XYZ conversion
  XYZ.r = Yxy.r * Yxy.g / Yxy. b;               // X = Y * x / y
  XYZ.g = Yxy.r;                                // copy luminance Y
  XYZ.b = Yxy.r * (1 - Yxy.g - Yxy.b) / Yxy.b;  // Z = Y * (1-x-y) / y
    
  // XYZ -> RGB conversion
  const float3x3 XYZ2RGB  = { 2.5651,-1.1665,-0.3986,
                              -1.0217, 1.9777, 0.0439, 
                               0.0753, -0.2543, 1.1892};
  color.rgb = mul(XYZ2RGB, XYZ);
  color.a = 1.0;
  return color;
}
*/