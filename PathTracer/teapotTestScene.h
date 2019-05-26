#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include "Scene.h"

namespace GLSLPT
{
	void loadTeapotTestScene(Scene* scene)
	{
		int mesh_id1 = scene->addMesh("./assets/Teapot/head.obj");
		int mesh_id2 = scene->addMesh("./assets/Teapot/body.obj");
		int mesh_id3 = scene->addMesh("./assets/Teapot/base.obj");
		//int mesh_id2 = scene->addMesh("./assets/Teapot/quad.obj");
		//int mesh_id3 = scene->addMesh("./assets/Figurine/teapot2.obj");

		Material gold;
		gold.albedo = glm::vec3(1.0, 0.71, 0.29);
		gold.roughness = 0.2;
		gold.metallic = 1.0;

		Material silver;
		silver.albedo = glm::vec3(1.0, 1.0, 1.0);
		silver.roughness = 0.0;
		silver.metallic = 1.0;

		Material red_plastic;
		red_plastic.albedo = glm::vec3(1.0, 0.0, 0.0);
		red_plastic.roughness = 0.01;
		red_plastic.metallic = 0.0;

		Material white;
		white.albedo = glm::vec3(1.0, 1.0, 1.0);
		white.roughness = 0.01;
		white.metallic = 0.0;

		Material matte_black;
		matte_black.albedo = glm::vec3(0.1, 0.1, 0.1);
		matte_black.roughness = 0.5;
		matte_black.metallic = 1.0;

		Material black;
		black.albedo = glm::vec3(0.1, 0.1, 0.1);
		black.roughness = 0.01;
		black.metallic = 1.0;

		Material glass;
		glass.albedo = glm::vec3(1.0, 1.0, 1.0);
		glass.materialType = 1.0;

		Material emissive;
		emissive.albedo = glm::vec3(0.0, 0.0, 0.0);
		emissive.emission = glm::vec4(7.0, 7.0, 3.0, 1.0);

		int gold_id = scene->addMaterial(gold);
		int silver_id = scene->addMaterial(silver);
		int red_id = scene->addMaterial(red_plastic);
		int white_id = scene->addMaterial(white);
		int black_id = scene->addMaterial(black);
		int matte_black_id = scene->addMaterial(matte_black);
		int glass_id = scene->addMaterial(glass);
		int emissive_id = scene->addMaterial(emissive);

		int headAlbedo = scene->addTexture("./assets/Teapot/textures/01_Head_Base_Color.png");
		int bodyAlbedo = scene->addTexture("./assets/Teapot/textures/02_Body_Base_Color.png");
		int baseAlbedo = scene->addTexture("./assets/Teapot/textures/03_Base_Base_Color.png");

		//scene->addCamera(glm::vec3(0.3f, 0.11f, 0.0f), glm::vec3(0.2f, 0.095f, 0.0f), 16.0f);
		
		/*for (int i = 0; i < 20; i++)
		{
			for (int j = 0; j < 20; j++)
			{
				int mat_id = (int)(((float)rand() / (RAND_MAX)) * scene->materials.size() - 1);

				if ((i == 0 && j == 0) || (i == 5 && j == 5) || (i == 4 && j == 7) || (i == 8 && j == 3))
					mat_id = 6;

				glm::mat4 xform;
				xform = glm::translate(xform, glm::vec3(j*0.5, 0.0, i*0.5));

				MeshInstance instance1(mesh_id1, xform, mat_id);

				scene->addMeshInstance(instance1);
			}
		}*/

		scene->renderOptions.maxDepth = 2;
		scene->renderOptions.maxSamples = 20;
		scene->renderOptions.numTilesY = 10;
		scene->renderOptions.numTilesX = 10;
		scene->renderOptions.rendererType = Renderer_Progressive;
		//scene->addCamera(glm::vec3(0.8f, 0.6f, 2.3f), glm::vec3(-0.25f, 0.15f, 0.0f), 16.0f);
		scene->addCamera(glm::vec3(0.0f, 1.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), 35.0f);
		//scene->addCamera(glm::vec3(0.0f, 0.5f, -1.0f), glm::vec3(0.0f, 0.5f, 0.0f), 35.0f);

		/*for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				int mat_id = (int)(((float)rand() / (RAND_MAX)) * scene->materials.size() - 1);

				if ((i == 0 && j == 0) || (i == 3 && j == 3) || (i == 4 && j == 7) || (i == 8 && j == 3))
					mat_id = glass_id;

				glm::mat4 xform;
				xform = glm::translate(glm::vec3(j*0.4, 0.0, i*0.4));
				xform *= glm::rotate(((float)rand() / (RAND_MAX)) * 45.0f, glm::vec3(0, 1, 0));

				MeshInstance instance1(mesh_id1, xform, mat_id);

				scene->addMeshInstance(instance1);
			}
		}*/

		glm::mat4 xform;
		
		//xform = glm::translate(glm::vec3(0.8, 0.0, 0.0));
		//xform *= glm::rotate(90.0f, glm::vec3(1, 1, 0));
		xform *= glm::scale(glm::vec3(10.0, 10.0, 10.0));
		MeshInstance instance1(mesh_id1, xform, white_id);
		MeshInstance instance2(mesh_id2, xform, white_id);
		MeshInstance instance3(mesh_id3, xform, white_id);


		//MeshInstance instance3(mesh_id1, xform, gold_id);

		scene->addMeshInstance(instance1);
		scene->addMeshInstance(instance2);
		scene->addMeshInstance(instance3);
		//scene->addMeshInstance(instance3);
		
		/*glm::mat4 xform2;
		xform2 = glm::translate(glm::vec3(0.6, -0.0005, 0.6));
		MeshInstance instance2(mesh_id2, xform2, matte_black_id);
		scene->addMeshInstance(instance2);
		*/
		scene->addHDR("./assets/Teapot/vankleef.hdr");

		scene->createAccelerationStructures();
	}
}