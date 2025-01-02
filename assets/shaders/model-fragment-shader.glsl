#version 100

precision mediump float;

varying vec4 f_color;
varying vec2 f_tex_coord;

uniform sampler2D texture_sampler;

void main()
{
	gl_FragColor = texture2D(texture_sampler, f_tex_coord);
//	gl_FragColor = f_color;
}
