#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include "Scene.h"
#include <math.h>

namespace GLSLPT
{
	void loadPantherTestScene(Scene* scene, RenderOptions &renderOptions)
	{
		renderOptions.maxDepth = 3;
		renderOptions.numTilesY = 5;
		renderOptions.numTilesX = 5;
		renderOptions.hdrMultiplier = 2.0f;
		scene->addCamera(glm::vec3(-2.62887, 0.390677, 0.009184 ), scene->sceneBounds.center() + glm::vec3(0, -2, 0), 17.0f);

		int mesh_id1 = scene->addMesh("./assets/panther/background.obj");
		int mesh_id2 = scene->addMesh("./assets/panther/panther.obj");

		Material orange;
		orange.albedo = glm::vec3(0.8, 0.289, 0.006);
		orange.roughness = 0.2;

		int orange_mat_id = scene->addMaterial(orange);

		Light light;
		light.type = LightType::AreaLight;
		light.position = glm::vec3(-3.104297, 0.428073, -1.188536);
		light.v = glm::vec3(-3.104297, 3.108073, -1.188536) - light.position;
		light.u = glm::vec3(-1.446963, 0.428073, -3.294632) - light.position;
		light.area = glm::length(glm::cross(light.u, light.v));
		light.emission = glm::vec3(3, 3, 3);

		Light light2;
		light2.type = LightType::AreaLight;
		light2.position = glm::vec3(-0.618198, 2.776746, - 1.119280);
		light2.v = glm::vec3(0.521802, 2.776746, - 1.119280) - light2.position;
		light2.u = glm::vec3(-0.618198, 2.776746, - 2.239280) - light2.position;
		light2.area = glm::length(glm::cross(light2.u, light2.v));
		light2.emission = glm::vec3(20, 20, 20);

		Light light3;
		light3.type = LightType::AreaLight;
		light3.position = glm::vec3(-0.428198, 4.113605, - 1.421092);
		light3.v = glm::vec3(0.331802, 4.113605, - 1.421092) - light3.position;
		light3.u = glm::vec3(-0.428198, 4.671239, - 1.937468) - light3.position;
		light3.area = glm::length(glm::cross(light3.u, light3.v));
		light3.emission = glm::vec3(30, 30, 30);

		int light1_id = scene->addLight(light);
		int light2_id = scene->addLight(light2);
		int light3_id = scene->addLight(light3);

		glm::mat4 xform;

		MeshInstance instance1(mesh_id1, xform, orange_mat_id);
		MeshInstance instance2(mesh_id2, xform, orange_mat_id);

		scene->addMeshInstance(instance1);
		scene->addMeshInstance(instance2);

		scene->createAccelerationStructures();

		
	}
}