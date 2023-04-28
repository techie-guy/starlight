#version 100

attribute vec3 aPos;
attribute vec4 aColor;
attribute vec2 aTexCoord;

varying vec4 fColor;
varying vec2 fTexCoord;

uniform mat4 mvp;

void main()
{
	gl_Position = mvp * vec4(aPos, 1.0);

	fColor = aColor;
	fTexCoord = aTexCoord;
}
