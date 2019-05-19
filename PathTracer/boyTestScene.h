#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include "Scene.h"

namespace GLSLPT
{
	void loadBoyTestScene(Scene* scene)
	{
		scene->renderOptions.maxDepth = 2;
		scene->renderOptions.maxSamples = 20;
		scene->renderOptions.numTilesY = 10;
		scene->renderOptions.numTilesX = 10;
		scene->renderOptions.hdrMultiplier = 2.0f;
		scene->renderOptions.rendererType = Renderer_Progressive;
		scene->addCamera(glm::vec3(0.3f, 0.11f, 0.0f), glm::vec3(0.2f, 0.095f, 0.0f), 35.0f);

		int mesh_id1 = scene->addMesh("./assets/Figurine/head.obj");
		int mesh_id2 = scene->addMesh("./assets/Figurine/body.obj");
		int mesh_id3 = scene->addMesh("./assets/Figurine/base.obj");
		int mesh_id4 = scene->addMesh("./assets/Figurine/background.obj");

		Material head;
		Material body;
		Material base;
		Material white;

		TexId headAlbedo = scene->addTexture("./assets/Figurine/textures/01_Head_Base_Color.png", TextureType::albedo);
		TexId bodyAlbedo = scene->addTexture("./assets/Figurine/textures/02_Body_Base_Color.png", TextureType::albedo);
		TexId baseAlbedo = scene->addTexture("./assets/Figurine/textures/03_Base_Base_Color.png", TextureType::albedo);
		TexId bgAlbedo = scene->addTexture("./assets/Figurine/textures/grid.jpg", TextureType::albedo);

		TexId headMatRgh = scene->addTexture("./assets/Figurine/textures/01_Head_MetallicRoughness.png", TextureType::metallic_roughness);
		TexId bodyMatRgh = scene->addTexture("./assets/Figurine/textures/02_Body_MetallicRoughness.png", TextureType::metallic_roughness);
		TexId baseMatRgh = scene->addTexture("./assets/Figurine/textures/03_Base_MetallicRoughness.png", TextureType::metallic_roughness);

		head.setTexture(headAlbedo);
		body.setTexture(bodyAlbedo);
		base.setTexture(baseAlbedo);
		white.setTexture(bgAlbedo);

		head.setTexture(headMatRgh);
		body.setTexture(bodyMatRgh);
		base.setTexture(baseMatRgh);

		int head_mat_id = scene->addMaterial(head);
		int body_mat_id = scene->addMaterial(body);
		int base_mat_id = scene->addMaterial(base);
		int white_mat_id = scene->addMaterial(white);

		Light light;
		light.type = LightType::AreaLight;
		light.position = glm::vec3(-0.103555f, 0.284840f, 0.606827f);
		light.u = glm::vec3(-0.103555f, 0.465656f, 0.521355f) - light.position;
		light.v = glm::vec3(0.096445f, 0.284840f, 0.606827f) - light.position;
		light.area = glm::length(glm::cross(light.u, light.v));
		light.emission = glm::vec3(40, 41, 41);

		Light light2;
		light2.type = LightType::AreaLight;
		light2.position = glm::vec3(0.303145f, 0.461806f, -0.450967f);
		light2.u = glm::vec3(0.362568f, 0.280251f, -0.510182f) - light2.position;
		light2.v = glm::vec3(0.447143f, 0.461806f, -0.306465f) - light2.position;
		light2.area = glm::length(glm::cross(light2.u, light2.v));
		light2.emission = glm::vec3(40, 41, 41);

		int light1_id = scene->addLight(light);
		int light2_id = scene->addLight(light2);

		glm::mat4 xform;
		//xform *= glm::scale(glm::vec3(10.0, 10.0, 10.0));

		MeshInstance instance1(mesh_id1, xform, head_mat_id);
		MeshInstance instance2(mesh_id2, xform, body_mat_id);
		MeshInstance instance3(mesh_id3, xform, base_mat_id);
		MeshInstance instance4(mesh_id4, xform, white_mat_id);

		scene->addMeshInstance(instance1);
		scene->addMeshInstance(instance2);
		scene->addMeshInstance(instance3);
		//scene->addMeshInstance(instance4);

		//scene->addHDR("./assets/ajax/sunset.hdr");

		scene->createAccelerationStructures();
	}
}