#pragma once

#include <vector>
#include <glm/glm.hpp>
#include "split_bvh.h"
#include "GL/gl3w.h"
#include "Program.h"

namespace GLSLPT
{	

	struct MeshVertex
	{
		glm::vec3 v1;
		glm::vec3 n1;
		glm::vec2 t1;
		glm::vec3 v2;
		glm::vec3 n2;
		glm::vec2 t2;
		glm::vec3 v3;
		glm::vec3 n3;
		glm::vec2 t3;
	};

	class Mesh
	{
	public:
		Mesh()
		{ 
			bvh = new RadeonRays::SplitBvh(2.0f, 64, 0, 0.001f, 2.5f); 
		}

		~Mesh() { delete bvh; }
		
		// Mesh Data
		std::vector<glm::vec4> vertices_uvx;
		std::vector<glm::vec4> normals_uvy;

		// Test
		std::vector<MeshVertex> data;

		RadeonRays::Bvh *bvh;
		std::string meshName;
		GLuint vao, vbo;
		void buildBVH();
		void draw(Program *shader);
		bool loadFromFile(const std::string &filename);
		void generateBuffers();
	};

	class MeshInstance
	{
	public:
		MeshInstance(int mesh_id, glm::mat4 xform, int mat_id) : meshID(mesh_id), transform(xform) , materialID(mat_id) {}
		~MeshInstance() {};

		glm::mat4 transform;
		int materialID;
		int meshID;
	};
}
