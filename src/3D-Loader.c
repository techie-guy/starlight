#include "cgltf.h"
#define CGLTF_IMPLEMENTATION

#include "3D-Loader.h"
#include "Utils.h"
#include "Texture.h"

#include <stb_ds.h>
#include <stb_image.h>


void load_gltf_file(Model* model, const char* filepath)
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

void gltf_model_parse_data(Model* model, Vertex** vertex_data, GLushort** index_data)
{
	for(size_t i = 0; i < model->data->meshes->primitives_count; i++)
	{
		const cgltf_primitive* primitive = &model->data->meshes->primitives[i];
		size_t vertex_count;
		size_t index_count;

		const cgltf_accessor* position_accessor = NULL;
		const cgltf_accessor* color_accessor = NULL;
		const cgltf_accessor* tex_coord_accessor = NULL;
		const cgltf_accessor* index_accessor = NULL;

        for (size_t j = 0; j < primitive->attributes_count; j++)
		{
			switch (primitive->attributes[j].type)
			{
			case cgltf_attribute_type_position:
            	position_accessor = primitive->attributes[j].data;
	            vertex_count = position_accessor->count;
				model->vertex_count = vertex_count;
    	        break;
        	case cgltf_attribute_type_color:
            	color_accessor = primitive->attributes[j].data;
	            break;
    	    case cgltf_attribute_type_texcoord:
        	    tex_coord_accessor = primitive->attributes[j].data;
            	break;
	        default:
    	        break;
		    }
    	}

		if (primitive->indices)
        {
            index_accessor = primitive->indices;
            index_count = index_accessor->count;
			model->index_count = index_count;

//			*index_data = malloc(index_count * sizeof(GLushort));
			*index_data = (GLushort*)(index_accessor->buffer_view->buffer->data + index_accessor->buffer_view->offset);

//			cgltf_accessor_unpack_indices(index_accessor, (void**)*index_data, sizeof(GLushort), index_count);
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

			arrput(*vertex_data, vertex);
		}
	}
}

void gltf_model_load_texture(Model* model, Texture* texture)
{
	for(size_t i = 0; i < model->data->images_count; i++)
	{
		cgltf_image* image = &model->data->images[i];

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
}


void model_load_from_file(Model* model, const char* filepath)
{
	switch (model->type)
	{
	case MODEL_GLTF:
		load_gltf_file(model, filepath);
	}
}


void model_parse_data(Model *model, Vertex **vertex_data, GLushort **index_data)
{
	switch (model->type)
	{
	case MODEL_GLTF:
		gltf_model_parse_data(model, vertex_data, index_data);
	}
}

void model_load_texture(Model* model, Texture* texture)
{
	switch (model->type)
	{
	case MODEL_GLTF:
		gltf_model_load_texture(model, texture);
	}
}

void model_free(Model* model)
{
	switch (model->type)
	{
	case MODEL_GLTF:
		cgltf_free(model->data);
	}

}
