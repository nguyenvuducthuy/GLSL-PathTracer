#pragma once

#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <map>
#include "hdrloader.h"
#include "bvh.h"
#include "Renderer.h"
#include "Mesh.h"
#include "Camera.h"
#include "bvh_translator.h"
#include "Texture.h"
#include "Material.h"

namespace GLSLPT
{
    class Camera;

	struct TexId
	{
		int id;
		TextureType type;
	};

	enum LightType
	{
		AreaLight,
		SphereLight
	};

    struct Light
    {
        glm::vec3 position;
        glm::vec3 emission;
        glm::vec3 u;
        glm::vec3 v;
		float radius;
		float area;
		LightType type;
    };

    class Scene
    {
    public:
        Scene() : camera(nullptr), hdrData(nullptr) {
			sceneBvh = new RadeonRays::Bvh(10.0f, 64, false);
		}
		~Scene() { delete camera; delete sceneBvh; };
        void addCamera(glm::vec3 pos, glm::vec3 lookAt, float fov);
		int addMesh(const std::string &filename);
		TexId addTexture(const std::string &filename, TextureType type);
		int addMaterial(const Material &material);
		int addMeshInstance(const MeshInstance &meshInstance);
		int setTexture(int matID, TexId texId);
		int addLight(const Light &light);
		void addHDR(const std::string &filename);
		void createAccelerationStructures();

		//Options
		RenderOptions renderOptions;

		//Mesh Data
		std::vector<Mesh*> meshes;
		
		//Instance Data
		std::vector<Material> materials;
		std::vector<MeshInstance> meshInstances;

		//Lights
		std::vector<Light> lights;
		
		//HDR
		HDRData *hdrData;

		//Camera
		Camera *camera;

		// Scene Mesh Data 
		std::vector<glm::vec3> vertIndices;
		std::vector<glm::vec3> vertices;
		std::vector<glm::vec3> normalIndices;
		std::vector<glm::vec3> normals;
		std::vector<glm::vec3> uvIndices;
		std::vector<glm::vec2> uvs;
		std::vector<glm::mat4> transforms;

		//Bvh
		RadeonRays::BvhTranslator bvhTranslator;

		//Texture Data
		std::vector<Texture *> textures;
		std::vector<unsigned char> albedoMapTexArray;
		std::vector<unsigned char> metallicRoughnessMapTexArray;
		std::vector<unsigned char> normalMapTexArray;
		int albedoMapTexCnt = 0;
		int metallicRoughnessMapTexCnt = 0;
		int normalMapTexCnt = 0;
		int texWidth, texHeight; // TODO: allow textures of different sizes

	private:
		std::map<std::string, int> meshMap;
		std::map<std::string, TexId> textureMap;
		RadeonRays::Bvh *sceneBvh;
		void createBLAS();
		void createTLAS();
	};
}