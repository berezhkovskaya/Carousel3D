/*VERTEX_PROGRAM*/

#include Layout.sp

in vec4 in_Vertex;
in vec4 in_TexCoord;

uniform vec4 GUI_Pos;

out vec2 TexCoord;

void main()
{
   float X1 = GUI_Pos.x;
   float Y1 = GUI_Pos.y;
   float X2 = GUI_Pos.z;
   float Y2 = GUI_Pos.w;

   float Width  = X2 - X1;
   float Height = Y2 - Y1;

   vec4 VertexPos = vec4( X1 + in_Vertex.x * Width,
                          Y1 + in_Vertex.y * Height,
                          in_Vertex.z, in_Vertex.w );

   gl_Position = in_ModelViewProjectionMatrix * VertexPos;

   TexCoord = in_TexCoord.xy;
}

/*FRAGMENT_PROGRAM*/

in vec2 TexCoord;

uniform sampler2D Texture0;

uniform float GUI_Opacity;

uniform mat4 ViewMtx;
uniform float LineThickness;

out vec4 out_FragColor;

void main()
{
	float u = TexCoord.x;
	float v = TexCoord.y;

	float nx = (u - 0.5) * 2.0;
	float ny = (v - 0.5) * 2.0;

	vec4 Colors[3];
	Colors[0] = vec4(1.0, 0.0, 0.0, 1.0);
	Colors[1] = vec4(0.0, 1.0, 0.0, 1.0);
	Colors[2] = vec4(0.0, 0.0, 1.0, 1.0);

	vec4 Result = vec4(0.0, 0.0, 0.0, 0.0);

	for (int i = 0 ; i != 3 ; i++)
	{
		vec3 Axis = normalize(vec3(ViewMtx[i][0], 0.0 /*ViewMtx[i][1]*/, ViewMtx[i][2]));
		if(ViewMtx[i][1] > 0.98 || ViewMtx[i][1] < -0.98) Axis = vec3(0.0,0.0,0.0);

		float t = nx * Axis.x + ny * Axis. z/*y*/;

		if (t < 0.0 || t > 0.5) continue;

		float dx = nx - t * Axis.x;
		float dy = ny - t * Axis.z /*y*/;

		float dist = sqrt(dx * dx + dy * dy);

		if(dist < LineThickness)
		{
			Result = Colors[i];
			break;
		}
	}

	out_FragColor = Result;
}
