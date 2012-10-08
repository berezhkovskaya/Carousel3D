// Transformation matrices uniforms buffer layout

/*
layout (std140) uniform Matrices
{
   // this must match C++ declaration of sMatrices
   mat4 in_ProjectionMatrix;
   mat4 in_ViewMatrix;
   mat4 in_ViewMatrixInverse;
   mat4 in_ModelMatrix;
   mat4 in_ModelMatrixInverse;
   mat4 in_ModelViewMatrix;
   mat4 in_ModelViewMatrixInverse;
   mat4 in_ModelViewProjectionMatrix;   
   mat4 in_NormalMatrix; // mat4 is used due to std140 padding
};
*/

uniform mat4 in_ProjectionMatrix;
uniform mat4 in_ViewMatrix;
uniform mat4 in_ViewMatrixInverse;
uniform mat4 in_ModelMatrix;
uniform mat4 in_ModelMatrixInverse;
uniform mat4 in_ModelViewMatrix;
uniform mat4 in_ModelViewMatrixInverse;
uniform mat4 in_ModelViewProjectionMatrix;   
uniform mat4 in_NormalMatrix; // mat4 is used due to std140 padding
