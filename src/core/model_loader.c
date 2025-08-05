#include "cglm/struct/mat4.h"
#include "cgltf.h"
#define CGLTF_IMPLEMENTATION

#include "model_loader.h"
#include "renderer.h"

#include "../utils/utils.h"

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

void gltf_load_mesh(Model* model, cgltf_node* gltf_node)
{
	Mesh mesh = {};
	mesh.mesh = gltf_node->mesh;
	cgltf_mesh* gltf_mesh = mesh.mesh;

	float matrix[16];
	cgltf_node_transform_local(gltf_node, matrix);

	mesh.mesh_transform = glms_mat4_mul(GLMS_MAT4_IDENTITY, *(mat4s*)matrix);

	if(mesh.mesh)
	{
		for(size_t i = 0; i < gltf_mesh->primitives_count; i++)
		{
			const cgltf_primitive* primitive = &gltf_mesh->primitives[i];
			size_t vertex_count;
			size_t index_count;

			const cgltf_accessor* position_accessor = NULL;
			const cgltf_accessor* color_accessor = NULL;
			const cgltf_accessor* tex_coord_accessor = NULL;
			const cgltf_accessor* normal_accessor = NULL;

	       	for(size_t j = 0; j < primitive->attributes_count; j++)
			{
				switch(primitive->attributes[j].type)
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
				log_debug("Loading Material: %s\n", primitive->material->name);
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


			if(primitive->indices)
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
						vec4s transformed_position = glms_mat4_mulv(mesh.mesh_transform, (vec4s){pos[0], pos[1], pos[2], 1.0f});
						vertex.position = (vec3s){transformed_position.x, transformed_position.y, transformed_position.z};
					}

					if(color_accessor)
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

			    	if(tex_coord_accessor)
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

					arrput(mesh.vertex_render_data, vertex);
				}
			}
		
			init_vertex_attributes(&mesh.vertex_attribs, mesh.vertex_render_data, mesh.vertex_count*sizeof(Vertex), mesh.index_data, mesh.index_count*sizeof(GLushort), true);

			shput(model->meshes_hashmap, gltf_mesh->name, mesh);
		}
}

// Sussy Code!

void gltf_model_load_node(Model* model, cgltf_node* node)
{
	log_debug("Loading node %s\n", node->name);
	gltf_load_mesh(model, node);

	for(size_t i = 0; i < node->children_count; i++)
	{
		cgltf_node* node_child = node->children[i];
		gltf_load_mesh(model, node_child);
	}
}

void gltf_model_parse_data(Model* model)
{
	cgltf_scene* scene = model->data->scene;

	for(size_t i = 0; i < scene->nodes_count; i++)
	{
		gltf_model_load_node(model, scene->nodes[i]);
	}
}

void model_load_from_file(Model* model, const char* filepath)
{
	switch (model->type)
	{
	case MODEL_GLTF:
		gltf_load_file(model, filepath);
	}
}


void model_parse_data(Model* model)
{
	switch (model->type)
	{
	case MODEL_GLTF:
		gltf_model_parse_data(model);
	}
}

void model_free(Model* model)
{
//	free(model->index_data);

	switch (model->type)
	{
	case MODEL_GLTF:
		cgltf_free(model->data);
	}
}
