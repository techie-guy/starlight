#version 100

precision mediump float;

varying vec3 f_pos;
varying vec4 f_color;
varying vec2 f_tex_coord;
varying vec3 f_normal;

uniform vec3 cam_pos;
uniform vec3 model_pos;

// Copied from Learn OpenGL

float DistributionGGX(vec3 N, vec3 H, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
vec3 fresnelSchlick(float cosTheta, vec3 F0);

struct Material
{
	sampler2D base_color; // albedo
	sampler2D metallic;
	sampler2D roughness;
	sampler2D normal_map;
	sampler2D occlusion;
};

struct Light
{
	vec3 position;
	vec3 color;
	float intensity;
};

uniform Material material;
uniform Light light;

const float PI = 3.14159265359;

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
	return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
	float a = roughness*roughness;
	float a2 = a*a;
	float NdotH = max(dot(N, H), 0.0);
	float NdotH2 = NdotH*NdotH;
	
	float num = a2;
	float denom = (NdotH2 * (a2 - 1.0) + 1.0);
	denom = PI * denom * denom;
	return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
	float r = (roughness + 1.0);
	float k = (r*r) / 8.0;
	float num = NdotV;
	float denom = NdotV * (1.0 - k) + k;
	return num / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
	float NdotV = max(dot(N, V), 0.0);
	float NdotL = max(dot(N, L), 0.0);
	float ggx2 = GeometrySchlickGGX(NdotV, roughness);
	float ggx1 = GeometrySchlickGGX(NdotL, roughness);
	return ggx1 * ggx2;
}


void main()
{
	/*
	vec3 ambient = light.ambient * vec3(texture2D(material.base_color, f_tex_coord));

	// Diffuse
	vec3 norm = texture2D(material.normal_map, f_tex_coord).rgb;
	norm = normalize(norm * 2.0 - 1.0);
	vec3 light_dir = normalize(light.position - f_pos);
	float diff = max(dot(norm, light_dir), 0.0);
	vec3 diffuse = light.diffuse * (diff * vec3(texture2D(material.base_color, f_tex_coord)));

	// Specular
	vec3 view_dir = normalize(view_pos - f_pos);
	vec3 halfway_dir = normalize(light_dir + view_dir);
	float spec = pow(max(dot(view_dir, halfway_dir), 0.0), vec3(texture2D(material.metallic_roughness, f_tex_coord)).r);
	vec3 specular = light.specular * (spec * vec3(texture2D(material.metallic_roughness, f_tex_coord)));
	
	gl_FragColor = vec4(ambient + diffuse + specular, 1.0);

	gl_FragColor = texture2D(material.base_color, f_tex_coord);
//	gl_FragColor = f_color;
	*/

	vec3 albedo = pow(texture2D(material.base_color, f_tex_coord).rgb, vec3(2.2));
//	vec3 normal = getNormalFromNormalMap();
	float metallic = texture2D(material.metallic, f_tex_coord).b;
	float roughness = texture2D(material.metallic, f_tex_coord).g;
	float ao = texture2D(material.metallic, f_tex_coord).r;
//	float ao = texture(aoMap, TexCoords).r;

	vec3 N = normalize(f_normal);
	vec3 V = normalize(cam_pos - f_pos);
	vec3 F0 = vec3(0.04);
	F0 = mix(F0, albedo, metallic);
	// reflectance equation
	vec3 Lo = vec3(0.0);

	// One light
//	for(int i = 0; i < 4; ++i)
//	{
		// calculate per-light radiance
		vec3 L = normalize(light.position - f_pos);
		vec3 H = normalize(V + L);
		float distance = length(light.position - f_pos);
		float attenuation = 1.0 / (distance * distance);
		vec3 radiance = light.color * attenuation;
		// cook-torrance brdf
		float NDF = DistributionGGX(N, H, roughness);
		float G = GeometrySmith(N, V, L, roughness);
		vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);
		vec3 kS = F;
		vec3 kD = vec3(1.0) - kS;
		kD *= 1.0 - metallic;
		vec3 numerator = NDF * G * F;
		float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
		vec3 specular = numerator / max(denominator, 0.001);
		// add to outgoing radiance Lo
		float NdotL = max(dot(N, L), 0.0);
		Lo += (kD * albedo / PI + specular) * radiance * NdotL;
//	}

	vec3 ambient = vec3(0.03) * albedo * ao;
	vec3 color = ambient + Lo;
	color = color / (color + vec3(1.0));
	color = pow(color, vec3(1.0/2.2)) * light.intensity;
	gl_FragColor = vec4(color, 1.0);
}
