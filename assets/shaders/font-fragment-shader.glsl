#version 100

precision mediump float;

varying vec4 fColor;
varying vec2 fTexCoord;

uniform sampler2D text;
uniform vec3 textColor;

void main()
{
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture2D(text, fTexCoord).r);
    gl_FragColor = vec4(textColor, 1.0) * sampled;
}
