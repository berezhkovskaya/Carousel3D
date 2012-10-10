// Transformation matrices uniforms buffer layout

uniform mat4 in_ProjectionMatrix;
uniform mat4 in_ViewMatrix;
uniform mat4 in_ViewMatrixInverse;
uniform mat4 in_ModelMatrix;
uniform mat4 in_ModelViewMatrix;
uniform mat4 in_ModelViewProjectionMatrix;   
uniform mat4 in_NormalMatrix; // mat4 is used due to std140 padding
