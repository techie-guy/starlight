#version 100

precision mediump float;

varying vec4 f_color;
varying vec2 f_tex_coord;

uniform vec3 light_color;

void main()
{
//	gl_FragColor = texture2D(texture_sampler, f_tex_coord);
//	gl_FragColor = vec4(light_color * object_color, 1.0);
	gl_FragColor = vec4(light_color, 1.0);
}
