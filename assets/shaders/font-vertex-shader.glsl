#version 100

attribute vec3 a_pos;
attribute vec4 a_color;
attribute vec2 a_tex_coord;

varying vec4 f_color;
varying vec2 f_tex_coord;

uniform mat4 projection;

void main()
{
	gl_Position = projection * vec4(a_pos, 1.0);

	f_color = a_color;
	f_tex_coord = a_tex_coord;
}
