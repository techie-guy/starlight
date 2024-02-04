#version 100

precision mediump float;

varying vec4 f_color;
varying vec2 f_tex_coord;

uniform sampler2D text;
uniform vec3 text_color;

void main()
{
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture2D(text, f_tex_coord).r);
    gl_FragColor = vec4(text_color, 1.0) * sampled;
}
