#include <iostream>

#include "Scene.h"
#include "Camera.h"

namespace GLSLPT
{
    void Scene::addCamera(glm::vec3 pos, glm::vec3 lookAt, float fov)
    {
        delete camera;
        camera = new Camera(pos, lookAt, fov);
    }

	int Scene::addMesh(const std::string& filename)
	{
		// Check if mesh was already loaded
		int id = -1;
		std::map<std::string, int>::iterator it = meshMap.find(filename);

		if (it == meshMap.end()) // New Mesh
		{
			id = meshes.size();

			Mesh *mesh = new Mesh;

			if (mesh->loadFromFile(filename))
			{
				meshes.push_back(mesh);
				meshMap[filename] = id;
				printf("Model %s loaded\n", filename.c_str());
			}
			else
				id = -1;
		}
		else // Existing Mesh
		{
			id = meshMap[filename];
		}

		return id;
	}

	int Scene::addTexture(const std::string& filename)
	{
		// Check if texture was already loaded
		std::map<std::string, int>::iterator it = textureMap.find(filename);
		int id = -1;

		if (it == textureMap.end()) // New Texture
		{
			id = textures.size();

			Texture *texture = new Texture;

			if (texture->loadTexture(filename))
			{
				textures.push_back(texture);
				textureMap[filename] = id;
				printf("Texture %s loaded\n", filename.c_str());
			}
			else
				id = -1;
		}
		else // Existing Mesh
		{
			id = meshMap[filename];
		}

		return id;
	}

	int Scene::addMaterial(const Material& material)
	{
		int id = materials.size();
		materials.push_back(material);
		return id;
	}

	void Scene::addHDR(const std::string& filename)
	{
		delete hdrData;
		hdrData = HDRLoader::load(filename.c_str());
		if (hdrData == nullptr)
			printf("Unable to load HDR\n");
		else
		{
			printf("HDR %s loaded\n", filename.c_str());
			renderOptions.useEnvMap = true;
		}
	}

	int Scene::addMeshInstance(const MeshInstance &meshInstance)
	{
		int id = meshInstances.size();
		meshInstances.push_back(meshInstance);
		return id;
	}

	int Scene::addLight(const Light &light)
	{
		int id = lights.size();
		lights.push_back(light);
		return id;
	}

	void Scene::createTLAS()
	{
		// Loop through all the mesh Instances and build a Top Level BVH
		std::vector<RadeonRays::bbox> bounds;
		bounds.resize(meshInstances.size());

		#pragma omp parallel for
		for (int i = 0; i < meshInstances.size(); i++)
		{
			RadeonRays::bbox bbox = meshes[meshInstances[i].meshID]->bvh->Bounds();
			glm::mat4 matrix = meshInstances[i].transform;

			glm::vec3 minBound = bbox.pmin;
			glm::vec3 maxBound = bbox.pmax;

			glm::vec3 right       = glm::vec3(matrix[0][0], matrix[0][1], matrix[0][2]);
			glm::vec3 up          = glm::vec3(matrix[1][0], matrix[1][1], matrix[1][2]);
			glm::vec3 forward     = glm::vec3(matrix[2][0], matrix[2][1], matrix[2][2]);
			glm::vec3 translation = glm::vec3(matrix[3][0], matrix[3][1], matrix[3][2]);

			glm::vec3 xa = right * minBound.x;
			glm::vec3 xb = right * maxBound.x;

			glm::vec3 ya = up * minBound.y;
			glm::vec3 yb = up * maxBound.y;

			glm::vec3 za = forward * minBound.z;
			glm::vec3 zb = forward * maxBound.z;

			minBound = glm::min(xa, xb) + glm::min(ya, yb) + glm::min(za, zb) + translation;
			maxBound = glm::max(xa, xb) + glm::max(ya, yb) + glm::max(za, zb) + translation;

			RadeonRays::bbox bound;
			bound.pmin = minBound;
			bound.pmax = maxBound;

			bounds[i] = bound;
		}
		printf("Building scene BVH\n");
		sceneBvh->Build(&bounds[0], bounds.size());
		sceneBounds = sceneBvh->Bounds();
	}

	void Scene::createBLAS()
	{
		// Loop through all meshes and build BVHs
		#pragma omp parallel for
		for (int i = 0; i < meshes.size(); i++)
		{
			printf("Building BVH for %s\n", meshes[i]->meshName.c_str());
			meshes[i]->buildBVH();
		}
	}

	void Scene::createAccelerationStructures()
	{
		createBLAS();
		createTLAS();

		// Flatten BVH
		bvhTranslator.Process(sceneBvh, meshes, meshInstances);

		int verticesCnt = 0;
		int normalsCnt = 0;
		int uvsCnt = 0;

		//Copy mesh data
		for (int i = 0; i < meshes.size(); i++)
		{
			// Copy indices from BVH and not from Mesh
			int numIndices = meshes[i]->bvh->GetNumIndices();
			const int * triIndices = meshes[i]->bvh->GetIndices();

			for (int j = 0; j < numIndices; j++)
			{
				int index = triIndices[j];
				int v1 = meshes[i]->vert_indices[index * 3 + 0] + verticesCnt;
				int v2 = meshes[i]->vert_indices[index * 3 + 1] + verticesCnt;
				int v3 = meshes[i]->vert_indices[index * 3 + 2] + verticesCnt;

				int n1 = meshes[i]->nrm_indices[index * 3 + 0] + normalsCnt;
				int n2 = meshes[i]->nrm_indices[index * 3 + 1] + normalsCnt;
				int n3 = meshes[i]->nrm_indices[index * 3 + 2] + normalsCnt;

				int t1 = meshes[i]->uv_indices[index * 3 + 0] + uvsCnt;
				int t2 = meshes[i]->uv_indices[index * 3 + 1] + uvsCnt;
				int t3 = meshes[i]->uv_indices[index * 3 + 2] + uvsCnt;

				vertIndices.push_back(Indices{ v1, v2, v3 });
				normalIndices.push_back(Indices{ n1, n2, n3 });
				uvIndices.push_back(Indices{ t1, t2, t3 });
			}

			vertices.insert(vertices.end(), meshes[i]->vertices.begin(), meshes[i]->vertices.end());
			normals.insert(normals.end(), meshes[i]->normals.begin(), meshes[i]->normals.end());
			uvs.insert(uvs.end(), meshes[i]->uvs.begin(), meshes[i]->uvs.end());

			verticesCnt += meshes[i]->vertices.size();
			normalsCnt += meshes[i]->normals.size();
			uvsCnt += meshes[i]->uvs.size();
		}

		// Resize to power of 2
		indicesTexWidth  = (int)(sqrt(vertIndices.size()) + 1); // size of array is the same for vertices/normals/uvs
		verticesTexWidth = (int)(sqrt(vertices.size())    + 1);
		normalsTexWidth  = (int)(sqrt(normals.size())     + 1);
		uvsTexWidth      = (int)(sqrt(uvs.size())         + 1);

		vertIndices.resize(indicesTexWidth * indicesTexWidth);
		normalIndices.resize(indicesTexWidth * indicesTexWidth);
		uvIndices.resize(indicesTexWidth * indicesTexWidth);

		vertices.resize(verticesTexWidth * verticesTexWidth);
		normals.resize(normalsTexWidth * normalsTexWidth);
		uvs.resize(uvsTexWidth * uvsTexWidth);

		for (int i = 0; i < vertIndices.size(); i++)
		{
			vertIndices[i].x = ((vertIndices[i].x % verticesTexWidth) << 12) | (vertIndices[i].x / verticesTexWidth);
			vertIndices[i].y = ((vertIndices[i].y % verticesTexWidth) << 12) | (vertIndices[i].y / verticesTexWidth);
			vertIndices[i].z = ((vertIndices[i].z % verticesTexWidth) << 12) | (vertIndices[i].z / verticesTexWidth);

			normalIndices[i].x = ((normalIndices[i].x % normalsTexWidth) << 12) | (normalIndices[i].x / normalsTexWidth);
			normalIndices[i].y = ((normalIndices[i].y % normalsTexWidth) << 12) | (normalIndices[i].y / normalsTexWidth);
			normalIndices[i].z = ((normalIndices[i].z % normalsTexWidth) << 12) | (normalIndices[i].z / normalsTexWidth);

			uvIndices[i].x = ((uvIndices[i].x % uvsTexWidth) << 12) | (uvIndices[i].x / uvsTexWidth);
			uvIndices[i].y = ((uvIndices[i].y % uvsTexWidth) << 12) | (uvIndices[i].y / uvsTexWidth);
			uvIndices[i].z = ((uvIndices[i].z % uvsTexWidth) << 12) | (uvIndices[i].z / uvsTexWidth);
		}

		//Copy transforms
		transforms.resize(meshInstances.size());
		#pragma omp parallel for
		for (int i = 0; i < meshInstances.size(); i++)
			transforms[i] = meshInstances[i].transform;

		//Copy Textures
		for (int i = 0; i < textures.size(); i++)
		{
			texWidth = textures[i]->width;
			texHeight = textures[i]->height;
			int texSize = texWidth * texHeight;
			textureMapsArray.insert(textureMapsArray.end(), &textures[i]->texData[0], &textures[i]->texData[texSize * 3]);
		}
	}
}