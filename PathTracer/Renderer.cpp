#include "Config.h"
#include "Renderer.h"
#include "Scene.h"
#include <vector>

namespace GLSLPT
{
    Program *loadShaders(const std::string &vertex_shader_fileName, const std::string &frag_shader_fileName)
    {
        std::vector<Shader> shaders;
        shaders.push_back(Shader(vertex_shader_fileName, GL_VERTEX_SHADER));
        shaders.push_back(Shader(frag_shader_fileName, GL_FRAGMENT_SHADER));
        return new Program(shaders);
    }

    Renderer::Renderer(const Scene *scene, const std::string& shadersDirectory) : albedoMapTex(0)
        , metallicRoughnessMapTex(0)
        , normalMapTex(0)
        , hdrTex(0)
        , hdrMarginalDistTex(0)
        , hdrConditionalDistTex(0)
        , initialized(false)
        , scene(scene)
        , screenSize(scene->renderOptions.resolution)
        , shadersDirectory(shadersDirectory)
    {
    }
    Renderer::~Renderer()
    {
        if (initialized)
            this->finish();
    }

    void Renderer::finish()
    {
        if (!initialized)
            return;

        glDeleteTextures(1, &BVHTexture);
        glDeleteTextures(1, &vertexIndicesTex);
        glDeleteTextures(1, &verticesTex);
		glDeleteTextures(1, &normalIndicesTex);
		glDeleteTextures(1, &normalsTex);
		glDeleteTextures(1, &uvIndicesTex);
		glDeleteTextures(1, &uvTex);
        glDeleteTextures(1, &materialsTex);
		glDeleteTextures(1, &transformsTex);
		glDeleteTextures(1, &lightsTex);
        glDeleteTextures(1, &albedoMapTex);
        glDeleteTextures(1, &metallicRoughnessMapTex);
		glDeleteTextures(1, &normalMapTex);
        glDeleteTextures(1, &hdrTex);
        glDeleteTextures(1, &hdrMarginalDistTex);
        glDeleteTextures(1, &hdrConditionalDistTex);

        glDeleteBuffers(1, &vertexIndicesBuffer);
        glDeleteBuffers(1, &verticesBuffer);
		glDeleteBuffers(1, &normalIndicesBuffer);
		glDeleteBuffers(1, &normalsBuffer);
		glDeleteBuffers(1, &uvIndicesBuffer);
		glDeleteBuffers(1, &uvBuffer);
		glDeleteBuffers(1, &materialsBuffer);
        glDeleteBuffers(1, &lightsBuffer);
        glDeleteBuffers(1, &BVHBuffer);
        

        initialized = false;
		printf("Renderer finished!\n");
    }

    void Renderer::init()
    {
        if (initialized)
            return;

        if (scene == nullptr)
        {
			printf("Error: No Scene Found\n");
            return ;
        }

        quad = new Quad();

        //Create Texture for BVH Tree
        glGenBuffers(1, &BVHBuffer);
        glBindBuffer(GL_TEXTURE_BUFFER, BVHBuffer);
        glBufferData(GL_TEXTURE_BUFFER, sizeof(RadeonRays::BvhTranslator::Node) * scene->bvhTranslator.nodes.size(), &scene->bvhTranslator.nodes[0], GL_STATIC_DRAW);
        glGenTextures(1, &BVHTexture);
        glBindTexture(GL_TEXTURE_BUFFER, BVHTexture);
        glTexBuffer(GL_TEXTURE_BUFFER, GL_RGB32F, BVHBuffer);

        //Create Buffer and Texture for VertexIndices
        glGenBuffers(1, &vertexIndicesBuffer);
        glBindBuffer(GL_TEXTURE_BUFFER, vertexIndicesBuffer);
        glBufferData(GL_TEXTURE_BUFFER, sizeof(glm::vec3) * scene->vertIndices.size(), &scene->vertIndices[0], GL_STATIC_DRAW);
        glGenTextures(1, &vertexIndicesTex);
        glBindTexture(GL_TEXTURE_BUFFER, vertexIndicesTex);
        glTexBuffer(GL_TEXTURE_BUFFER, GL_RGB32F, vertexIndicesBuffer);

        //Create Buffer and Texture for Vertices
        glGenBuffers(1, &verticesBuffer);
        glBindBuffer(GL_TEXTURE_BUFFER, verticesBuffer);
        glBufferData(GL_TEXTURE_BUFFER, sizeof(glm::vec3) * scene->vertices.size(), &scene->vertices[0], GL_STATIC_DRAW);
        glGenTextures(1, &verticesTex);
        glBindTexture(GL_TEXTURE_BUFFER, verticesTex);
        glTexBuffer(GL_TEXTURE_BUFFER, GL_RGB32F, verticesBuffer);

		//Create Buffer and Texture for Normal Indices
		glGenBuffers(1, &normalIndicesBuffer);
		glBindBuffer(GL_TEXTURE_BUFFER, normalIndicesBuffer);
		glBufferData(GL_TEXTURE_BUFFER, sizeof(glm::vec3) * scene->normalIndices.size(), &scene->normalIndices[0], GL_STATIC_DRAW);
		glGenTextures(1, &normalIndicesTex);
		glBindTexture(GL_TEXTURE_BUFFER, normalIndicesTex);
		glTexBuffer(GL_TEXTURE_BUFFER, GL_RGB32F, normalIndicesBuffer);

        //Create Buffer and Texture for Normals
        glGenBuffers(1, &normalsBuffer);
        glBindBuffer(GL_TEXTURE_BUFFER, normalsBuffer);
        glBufferData(GL_TEXTURE_BUFFER, sizeof(glm::vec3) * scene->normals.size(), &scene->normals[0], GL_STATIC_DRAW);
        glGenTextures(1, &normalsTex);
        glBindTexture(GL_TEXTURE_BUFFER, normalsTex);
        glTexBuffer(GL_TEXTURE_BUFFER, GL_RGB32F, normalsBuffer);

		//Create Buffer and Texture for TexCoords Indices
		glGenBuffers(1, &uvIndicesBuffer);
		glBindBuffer(GL_TEXTURE_BUFFER, uvIndicesBuffer);
		glBufferData(GL_TEXTURE_BUFFER, sizeof(glm::vec3) * scene->uvIndices.size(), &scene->uvIndices[0], GL_STATIC_DRAW);
		glGenTextures(1, &uvIndicesTex);
		glBindTexture(GL_TEXTURE_BUFFER, uvIndicesTex);
		glTexBuffer(GL_TEXTURE_BUFFER, GL_RGB32F, uvIndicesBuffer);

		//Create Buffer and Texture for TexCoords
		glGenBuffers(1, &uvBuffer);
		glBindBuffer(GL_TEXTURE_BUFFER, uvBuffer);
		glBufferData(GL_TEXTURE_BUFFER, sizeof(glm::vec2) * scene->uvs.size(), &scene->uvs[0], GL_STATIC_DRAW);
		glGenTextures(1, &uvTex);
		glBindTexture(GL_TEXTURE_BUFFER, uvTex);
		glTexBuffer(GL_TEXTURE_BUFFER, GL_RG32F, uvBuffer);

        //Create Buffer and Texture for Materials
        glGenBuffers(1, &materialsBuffer);
        glBindBuffer(GL_TEXTURE_BUFFER, materialsBuffer);
        glBufferData(GL_TEXTURE_BUFFER, sizeof(Material) * scene->materials.size(), &scene->materials[0], GL_STATIC_DRAW);
        glGenTextures(1, &materialsTex);
        glBindTexture(GL_TEXTURE_BUFFER, materialsTex);
        glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, materialsBuffer);

		//Create Buffer and Texture for Transforms
		glGenBuffers(1, &transformsBuffer);
		glBindBuffer(GL_TEXTURE_BUFFER, transformsBuffer);
		glBufferData(GL_TEXTURE_BUFFER, sizeof(glm::mat4) * scene->transforms.size(), &scene->transforms[0], GL_STATIC_DRAW);
		glGenTextures(1, &transformsTex);
		glBindTexture(GL_TEXTURE_BUFFER, transformsTex);
		glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, transformsBuffer);

        //Create Buffer and Texture for Lights
        numOfLights = int(scene->lights.size());

        if (numOfLights > 0)
        {
            glGenBuffers(1, &lightsBuffer);
            glBindBuffer(GL_TEXTURE_BUFFER, lightsBuffer);
            glBufferData(GL_TEXTURE_BUFFER, sizeof(Light) * scene->lights.size(), &scene->lights[0], GL_STATIC_DRAW);
            glGenTextures(1, &lightsTex);
            glBindTexture(GL_TEXTURE_BUFFER, lightsTex);
            glTexBuffer(GL_TEXTURE_BUFFER, GL_RGB32F, lightsBuffer);
        }

        // Albedo Texture
        if (scene->albedoMapTexCnt > 0)
        {
            glGenTextures(1, &albedoMapTex);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D_ARRAY, albedoMapTex);
            glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGB8, scene->texWidth, scene->texHeight, scene->albedoMapTexCnt, 0, GL_RGB, GL_UNSIGNED_BYTE, &scene->albedoMapTexArray[0]);
            glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
        }

        //Metallic Roughness
        if (scene->metallicRoughnessMapTexCnt > 0)
        {
            glGenTextures(1, &metallicRoughnessMapTex);
            glBindTexture(GL_TEXTURE_2D_ARRAY, metallicRoughnessMapTex);
            glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGB8, scene->texWidth, scene->texHeight, scene->metallicRoughnessMapTexCnt, 0, GL_RGB, GL_UNSIGNED_BYTE, &scene->metallicRoughnessMapTexArray[0]);
            glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
        }

        //NormalMap
        if (scene->normalMapTexCnt > 0)
        {
            glGenTextures(1, &normalMapTex);
            glBindTexture(GL_TEXTURE_2D_ARRAY, normalMapTex);
            glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGB8, scene->texWidth, scene->texHeight, scene->normalMapTexCnt, 0, GL_RGB, GL_UNSIGNED_BYTE, &scene->normalMapTexArray[0]);
            glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
        }

        // Environment Map
        if (scene->hdrData != nullptr)
        {
            glGenTextures(1, &hdrTex);
            glBindTexture(GL_TEXTURE_2D, hdrTex);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, scene->hdrData->width, scene->hdrData->height, 0, GL_RGB, GL_FLOAT, scene->hdrData->cols);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glBindTexture(GL_TEXTURE_2D, 0);

            glGenTextures(1, &hdrMarginalDistTex);
            glBindTexture(GL_TEXTURE_1D, hdrMarginalDistTex);
            glTexImage1D(GL_TEXTURE_1D, 0, GL_RG32F, scene->hdrData->height, 0, GL_RG, GL_FLOAT, scene->hdrData->marginalDistData);
            glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glBindTexture(GL_TEXTURE_1D, 0);

            glGenTextures(1, &hdrConditionalDistTex);
            glBindTexture(GL_TEXTURE_2D, hdrConditionalDistTex);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, scene->hdrData->width, scene->hdrData->height, 0, GL_RG, GL_FLOAT, scene->hdrData->conditionalDistData);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        initialized = true;
    }
}