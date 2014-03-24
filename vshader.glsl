#version 110

attribute  vec4 vPosition;

uniform mat4 modelview;
uniform vec4 vColor;
uniform mat4 Projection;

varying vec4 color;

void main() 
{ 
  gl_Position = modelview*vPosition*Projection;
  color = vColor;
}