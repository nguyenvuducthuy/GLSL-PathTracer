
#include <map>
#include <stack> 
#include "Scene.h"

namespace GLSLPT
{
	int(*Log)(const char* szFormat, ...) = printf;

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

	TexId Scene::addTexture(const std::string& filename, TextureType type)
	{
		// Check if texture was already loaded
		std::map<std::string, TexId>::iterator it = textureMap.find(filename);
		TexId texId;
		texId.id = -1;

		if (it == textureMap.end()) // New Texture
		{
			switch (type)
			{
				case TextureType::albedo:
					texId.id = albedoMapTexCnt++;
					break;
				case TextureType::metallic_roughness:
					texId.id = metallicRoughnessMapTexCnt++;
					break;
				case TextureType::normal:
					texId.id = normalMapTexCnt++;
					break;
			}

			Texture *texture = new Texture;

			if (texture->loadTexture(filename, type))
			{
				texId.type = type;
				textures.push_back(texture);
				textureMap[filename] = texId;
			}
			else
				texId.id = -1;
		}
		else // Existing Mesh
		{
			texId.id = meshMap[filename];
		}

		return texId;
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
	}

	int Scene::addMeshInstance(const MeshInstance &meshInstance)
	{
		int id = meshInstances.size();
		meshInstances.push_back(meshInstance);
		return id;
	}

	int Scene::setTexture(int matId, TexId texId)
	{
		if (matId >= materials.size() || matId < 0)
			return -1;

		switch (texId.type)
		{
			case TextureType::albedo:
				materials[matId].albedoTexID = texId.id;
				break;
			case TextureType::metallic_roughness:
				materials[matId].metallicRoughnessTexID = texId.id;
				break;
			case TextureType::normal:
				materials[matId].normalmapTexID = texId.id;
				break;
		}
		return 0;
	}

	int Scene::addLight(const Light &light)
	{
		int id = lights.size();
		lights.push_back(light);
		return id;
	}

	/*glm::vec3 transform_point(glm::vec3 const& p, glm::mat4 const& m)
	{
		glm::vec3 res = glm::vec3(m * glm::vec4(p,1.0));
		res.x += m[0][3];
		res.y += m[1][3];
		res.z += m[2][3];
		return res;
	}

	RadeonRays::bbox transform_bbox(RadeonRays::bbox const& b, glm::mat4 const& m)
	{
		// Get extents
		glm::vec3 extents = b.extents();

		// Transform the box to correct instance space
		RadeonRays::bbox newbox(transform_point(b.pmin, m));
		newbox.grow(transform_point(b.pmin + glm::vec3(extents.x, 0, 0), m));
		newbox.grow(transform_point(b.pmin + glm::vec3(extents.x, extents.y, 0), m));
		newbox.grow(transform_point(b.pmin + glm::vec3(0, extents.y, 0), m));
		newbox.grow(transform_point(b.pmin + glm::vec3(extents.x, 0, extents.z), m));
		newbox.grow(transform_point(b.pmin + glm::vec3(extents.x, extents.y, extents.z), m));
		newbox.grow(transform_point(b.pmin + glm::vec3(0, extents.y, extents.z), m));
		newbox.grow(transform_point(b.pmin + glm::vec3(0, 0, extents.z), m));

		return newbox;
	}*/

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
			//bounds[i] = transform_bbox(bbox, matrix);

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
		sceneBvh->Build(&bounds[0], bounds.size());
	}

	void Scene::createBLAS()
	{
		// Loop through all meshes and build BVHs
		#pragma omp parallel for
		for (int i = 0; i < meshes.size(); i++)
			meshes[i]->buildBVH();
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

				vertIndices.push_back(glm::vec3(v1, v2, v3));
				normalIndices.push_back(glm::vec3(n1, n2, n3));
				uvIndices.push_back(glm::vec3(t1, t2, t3));
			}
			
			vertices.insert(vertices.end(), meshes[i]->vertices.begin(), meshes[i]->vertices.end());
			normals.insert(normals.end(), meshes[i]->normals.begin(), meshes[i]->normals.end());
			uvs.insert(uvs.end(), meshes[i]->uvs.begin(), meshes[i]->uvs.end());

			verticesCnt += meshes[i]->vertices.size();
			normalsCnt += meshes[i]->normals.size();
			uvsCnt += meshes[i]->uvs.size();
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

			switch (textures[i]->texType)
			{
				case TextureType::albedo:
					albedoMapTexArray.insert(albedoMapTexArray.end(), &textures[i]->texData[0], &textures[i]->texData[texSize * 3]);
					break;
				case TextureType::metallic_roughness:
					metallicRoughnessMapTexArray.insert(metallicRoughnessMapTexArray.end(), &textures[i]->texData[0], &textures[i]->texData[texSize * 3]);
					break;
				case TextureType::normal:
					normalMapTexArray.insert(normalMapTexArray.end(), &textures[i]->texData[0], &textures[i]->texData[texSize * 3]);
					break;
			}
		}
	}
}