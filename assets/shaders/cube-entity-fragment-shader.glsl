#version 100

precision mediump float;

varying vec4 f_color;
varying vec2 f_tex_coord;
varying vec3 f_normal;
varying vec3 f_pos;

uniform vec3 view_pos;

struct Material
{
	sampler2D diffuse;
	sampler2D specular;
	sampler2D normal_map;

	float shininess;
};

struct Light
{
	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

uniform Material material;
uniform Light light;

void main()
{
	// Ambient
	vec3 ambient = light.ambient * vec3(texture2D(material.diffuse, f_tex_coord));

	// Diffuse
	vec3 norm = texture2D(material.normal_map, f_tex_coord).rgb;
	norm = normalize(norm * 2.0 - 1.0);
	vec3 light_dir = normalize(light.position - f_pos);
	float diff = max(dot(norm, light_dir), 0.0);
	vec3 diffuse = light.diffuse * (diff * vec3(texture2D(material.diffuse, f_tex_coord)));

	// Specular
	vec3 view_dir = normalize(view_pos - f_pos);
	vec3 halfway_dir = normalize(light_dir + view_dir);
	float spec = pow(max(dot(view_dir, halfway_dir), 0.0), material.shininess);
	vec3 specular = light.specular * (spec * vec3(texture2D(material.specular, f_tex_coord)));
	
	gl_FragColor = vec4(ambient + diffuse + specular, 1.0);
}
