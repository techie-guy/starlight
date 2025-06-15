#version 100

attribute vec3 a_pos;
attribute vec4 a_color;
attribute vec2 a_tex_coord;
attribute vec3 a_normal;

varying vec4 f_color;
varying vec2 f_tex_coord;
varying vec3 f_normal;
varying vec3 f_pos;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 transform;
uniform mat3 transform_normal;

void main()
{
	gl_Position = projection * view * transform * vec4(a_pos, 1.0);
//	gl_Position = projection * view * vec4(a_pos, 1.0);

	f_color = a_color;
	f_tex_coord = a_tex_coord;
	f_pos = vec3(transform * vec4(a_pos, 1.0));
	f_normal = transform_normal * a_normal;
}
