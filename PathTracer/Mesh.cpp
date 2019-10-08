#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include "Mesh.h"
#include <iostream>

namespace GLSLPT
{
	bool Mesh::loadFromFile(const std::string &filename)
	{
		meshName = filename;
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string err;
		bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, filename.c_str(), 0, true);

		if (!ret)
		{
			printf("Unable to load model\n");
			return false;
		}

		int x = 0;
		// Loop over shapes
		for (size_t s = 0; s < shapes.size(); s++) 
		{
			// Loop over faces(polygon)
			size_t index_offset = 0;

			for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) 
			{
				int fv = shapes[s].mesh.num_face_vertices[f];
				
				// Loop over vertices in the face.
				for (size_t v = 0; v < fv; v++)
				{
					// access to vertex
					tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
					tinyobj::real_t vx = attrib.vertices[3 * idx.vertex_index + 0];
					tinyobj::real_t vy = attrib.vertices[3 * idx.vertex_index + 1];
					tinyobj::real_t vz = attrib.vertices[3 * idx.vertex_index + 2];
					tinyobj::real_t nx = attrib.normals[3 * idx.normal_index + 0];
					tinyobj::real_t ny = attrib.normals[3 * idx.normal_index + 1];
					tinyobj::real_t nz = attrib.normals[3 * idx.normal_index + 2];

					tinyobj::real_t tx, ty;
					
					// temporary fix
					if (attrib.texcoords.size() > 0)
					{
						tx = attrib.texcoords[2 * idx.texcoord_index + 0];
						ty = attrib.texcoords[2 * idx.texcoord_index + 1];
					}
					else
					{
						tx = ty = 0;
					}
					
					vertices_uvx.push_back(glm::vec4(vx, vy, vz, tx));
					normals_uvy.push_back(glm::vec4(nx, ny, nz, ty));
				}

				const glm::vec3 v1 = glm::vec3(vertices_uvx[x * 3 + 0]);
				const glm::vec3 v2 = glm::vec3(vertices_uvx[x * 3 + 1]);
				const glm::vec3 v3 = glm::vec3(vertices_uvx[x * 3 + 2]);

				const glm::vec3 n1 = glm::vec3(normals_uvy[x * 3 + 0]);
				const glm::vec3 n2 = glm::vec3(normals_uvy[x * 3 + 1]);
				const glm::vec3 n3 = glm::vec3(normals_uvy[x * 3 + 2]);

				const glm::vec2 t1 = glm::vec2(vertices_uvx[x * 3 + 0].w, 1.0-normals_uvy[x * 3 + 0].w);
				const glm::vec2 t2 = glm::vec2(vertices_uvx[x * 3 + 1].w, 1.0-normals_uvy[x * 3 + 1].w);
				const glm::vec2 t3 = glm::vec2(vertices_uvx[x * 3 + 2].w, 1.0-normals_uvy[x * 3 + 2].w);

				x++;

				MeshVertex vertex = { v1,n1,t1,v2,n2,t2,v3,n3,t3 };
				data.push_back(vertex);
	
				index_offset += fv;
			}
		}

		return true;
	}

	void Mesh::buildBVH()
	{
		const int numTris = vertices_uvx.size() / 3;
		std::vector<RadeonRays::bbox> bounds(numTris);

		#pragma omp parallel for
		for (int i = 0; i < numTris; ++i)
		{
			const glm::vec3 v1 = glm::vec3(vertices_uvx[i * 3 + 0]);
			const glm::vec3 v2 = glm::vec3(vertices_uvx[i * 3 + 1]);
			const glm::vec3 v3 = glm::vec3(vertices_uvx[i * 3 + 2]);

			bounds[i].grow(v1);
			bounds[i].grow(v2);
			bounds[i].grow(v3);
		}

		bvh->Build(&bounds[0], numTris);
	}

	void Mesh::draw(Program *shader)
	{
		shader->use();
		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, data.size() * 3);
		glBindVertexArray(0);
		shader->stopUsing();
	}

	void Mesh::generateBuffers()
	{
		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);
		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);

		glBufferData(GL_ARRAY_BUFFER, sizeof(MeshVertex) * data.size(), &data[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
		glBindVertexArray(0);
	}
}