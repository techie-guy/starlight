#include "VertexAttributes.h"
#include "cgltf.h"
#define CGLTF_IMPLEMENTATION

#include "3D-Loader.h"
#include "Utils.h"
#include "Texture.h"

#include <stb_ds.h>
#include <stb_image.h>

void gltf_load_file(Model* model, const char* filepath)
{
	cgltf_result result = cgltf_parse_file(&model->options, filepath, &model->data);

	if(result == cgltf_result_success)
	{
		cgltf_load_buffers(&model->options, model->data, filepath);
	}
	else
	{
		log_debug("Couldn't Load: %d\n", result);

		model_free(model);
	}
}

void gltf_load_image(cgltf_image* image, Texture* texture)
{
	if(image->uri)
	{
		init_texture_from_file(texture, image->uri);
	}
	else if(image->buffer_view)
	{
		unsigned char* image_data = (unsigned char*)(image->buffer_view->buffer->data + image->buffer_view->offset);
		size_t image_size = image->buffer_view->size;

		int channels;
		unsigned char* data = stbi_load_from_memory(image_data, image_size, &texture->width, &texture->height, &channels, 4);

		init_texture_from_data(texture, 0, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, data);

		stbi_image_free(data);
	}
}


void gltf_load_texture(cgltf_texture* gltf_texture, TextureHashMap** textures, const char* texture_name)
{
	Texture texture;
	
	if(gltf_texture->sampler)
	{
		texture.texture_config.is_init = true;
		texture.texture_config.wrap_s = gltf_texture->sampler->wrap_s;
		texture.texture_config.wrap_t = gltf_texture->sampler->wrap_t;
		texture.texture_config.min_filter = gltf_texture->sampler->min_filter;
		texture.texture_config.mag_filter = gltf_texture->sampler->mag_filter;
	}

	gltf_load_image(gltf_texture->image, &texture);

	shput(*textures, texture_name, texture);
}


//void gltf_model_parse_data(Model* model, Vertex** vertex_data, GLushort** index_data)
void gltf_model_parse_data(Model* model)
{
	for(size_t t = 0; t < model->data->meshes_count; t++)
	{
		cgltf_mesh* gltf_mesh = &model->data->meshes[t];

		Mesh mesh = {};
		mesh.mesh = gltf_mesh;

		for(size_t i = 0; i < gltf_mesh->primitives_count; i++)
		{
			const cgltf_primitive* primitive = &gltf_mesh->primitives[i];
			size_t vertex_count;
			size_t index_count;

			const cgltf_accessor* position_accessor = NULL;
			const cgltf_accessor* color_accessor = NULL;
			const cgltf_accessor* tex_coord_accessor = NULL;
			const cgltf_accessor* normal_accessor = NULL;

        	for (size_t j = 0; j < primitive->attributes_count; j++)
			{
				switch (primitive->attributes[j].type)
				{
				case cgltf_attribute_type_position:
	            	position_accessor = primitive->attributes[j].data;
		            vertex_count = position_accessor->count;
					mesh.vertex_count = position_accessor->count;
    	    	    break;
	        	case cgltf_attribute_type_color:
    	        	color_accessor = primitive->attributes[j].data;
	    	        break;
    	    	case cgltf_attribute_type_texcoord:
	        	    tex_coord_accessor = primitive->attributes[j].data;
    	        	break;
				case cgltf_attribute_type_normal:
        		    normal_accessor = primitive->attributes[j].data;
            		break;
		        default:
    		        break;
			    }
	    	}

			if(primitive->material)
			{
				mesh.material.gltf_material = primitive->material;

				if(mesh.material.gltf_material->has_pbr_metallic_roughness)
				{
					mesh.material.metallic_factor = mesh.material.gltf_material->pbr_metallic_roughness.metallic_factor;
					mesh.material.roughness_factor = mesh.material.gltf_material->pbr_metallic_roughness.roughness_factor;
/*
					mesh.material.base_color.r = mesh.material.gltf_material->pbr_metallic_roughness.base_color_factor[0];
					mesh.material.base_color.g = mesh.material.gltf_material->pbr_metallic_roughness.base_color_factor[1];
					mesh.material.base_color.b = mesh.material.gltf_material->pbr_metallic_roughness.base_color_factor[2];
					mesh.material.base_color.a = mesh.material.gltf_material->pbr_metallic_roughness.base_color_factor[3];
					*/

					if(mesh.material.gltf_material->pbr_metallic_roughness.base_color_texture.texture)
					{
						gltf_load_texture(mesh.material.gltf_material->pbr_metallic_roughness.base_color_texture.texture, &mesh.material.texture_hashmap, "base_color");
					}

					if(mesh.material.gltf_material->pbr_metallic_roughness.metallic_roughness_texture.texture)
					{
						gltf_load_texture(mesh.material.gltf_material->pbr_metallic_roughness.metallic_roughness_texture.texture, &mesh.material.texture_hashmap, "metallic_roughness");
					}
				}

				if(mesh.material.gltf_material->normal_texture.texture)
				{
					gltf_load_texture(mesh.material.gltf_material->normal_texture.texture, &mesh.material.texture_hashmap, "normal");
				}

				if(mesh.material.gltf_material->occlusion_texture.texture)
				{
					gltf_load_texture(mesh.material.gltf_material->occlusion_texture.texture, &mesh.material.texture_hashmap, "occlusion");
				}

	  		}

			if (primitive->indices)
        	{
    	        index_count = primitive->indices->count;
				mesh.index_count = index_count;
				model->index_count += index_count;
			
				mesh.index_data = (GLushort*)(primitive->indices->buffer_view->buffer->data + primitive->indices->buffer_view->offset);
	        }
 
			for(size_t k = 0; k < vertex_count; k++)
			{
				Vertex vertex;

				if(position_accessor)
				{
					float pos[3] = {0};
	  				cgltf_accessor_read_float(position_accessor, k, pos, 3);
					vertex.position = (vec3s){pos[0], pos[1], pos[2]};
				}

				if (color_accessor)
				{
		    	    float col[4] = {0.0f, 0.0f, 0.0f, 1.0f};
        			cgltf_accessor_read_float(color_accessor, k, col, 4);
			        vertex.color = (vec4s){col[0], col[1], col[2], col[3]};
					glms_vec4_print(vertex.color, stdout);
				}
				else
				{
					vertex.color = (vec4s){1.0f, 1.0f, 1.0f, 1.0f};
				}

		    	if (tex_coord_accessor)
				{
    	    		float tex[2] = {0};
			        cgltf_accessor_read_float(tex_coord_accessor, k, tex, 2);
        			vertex.tex_coord = (vec2s){tex[0], tex[1]};
		    	}

				if(normal_accessor)
				{
					float normal[3] = {0};
	  				cgltf_accessor_read_float(normal_accessor, k, normal, 3);
					vertex.normal = (vec3s){normal[0], normal[1], normal[2]};
				}

//				arrput(model->vertex_render_data, vertex);
				arrput(mesh.vertex_render_data, vertex);
			}
		}
		
		init_vertex_attributes(&mesh.vertex_attribs, mesh.vertex_render_data, mesh.vertex_count*sizeof(Vertex), mesh.index_data, mesh.index_count*sizeof(GLushort), true);

		shput(model->meshes_hashmap, gltf_mesh->name, mesh);
	}
/*
	model->index_data = malloc(model->index_count * sizeof(GLushort));
	for(int i = 0; i < shlen(model->meshes_hashmap); i++)
	{
		memcpy(model->index_data, model->meshes_hashmap[i].value.index_data, sizeof(GLushort) * model->meshes_hashmap[i].value.index_count);
	}
	*/
}
/*
void gltf_load_materials(Model* model, TextureHashMap** textures)
{	
	for(size_t i = 0; i < model->data->materials_count; i++)
	{
		cgltf_material* material = &model->data->materials[i];
		log_debug("%ld %s\n", model->data->materials_count, material->name);

		if(material->has_pbr_metallic_roughness)
		{
			gltf_load_texture(material->pbr_metallic_roughness.base_color_texture.texture, textures, "base_color");
			gltf_load_texture(material->pbr_metallic_roughness.metallic_roughness_texture.texture, textures, "metallic_roughness");
//			gltf_load_texture(material->pbr_metallic_roughness.metallic_roughness_texture.texture, textures, "specular");
		}

		if(material->normal_texture.texture)
		{
			gltf_load_texture(material->normal_texture.texture, textures, "normal");
		}
	}
}
*/


void model_load_from_file(Model* model, const char* filepath)
{
	switch (model->type)
	{
	case MODEL_GLTF:
		gltf_load_file(model, filepath);
	}
}


//void model_parse_data(Model *model, Vertex **vertex_data, GLushort **index_data)
//void model_parse_data(Model *model)
void model_parse_data(Model* model)
{
	switch (model->type)
	{
	case MODEL_GLTF:
		gltf_model_parse_data(model);
	}
}
/*
void model_load_materials(Model* model, TextureHashMap** textures)
{
	switch (model->type)
	{
	case MODEL_GLTF:
		gltf_load_materials(model, textures);
	}
}
*/
void model_free(Model* model)
{
//	free(model->index_data);

	switch (model->type)
	{
	case MODEL_GLTF:
		cgltf_free(model->data);
	}
}
