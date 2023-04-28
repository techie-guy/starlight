#version 100

precision mediump float;

varying vec4 fColor;
varying vec2 fTexCoord;

uniform sampler2D textureSampler;

void main()
{
	gl_FragColor = texture2D(textureSampler, fTexCoord);
}
