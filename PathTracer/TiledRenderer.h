#pragma once

#include "Renderer.h"

namespace GLSLPT
{
    class Scene;
    class TiledRenderer : public Renderer
    {
    private:
		GLuint gBufferFBO, pathTraceFBO, accumFBO, outputFBO;
		Program  *gBufferShader, *pathTraceShader, *accumShader, *tileOutputShader, *outputShader;
		GLuint gBufferTexture, pathTraceTexture, accumTexture, tileOutputTexture[2];
		int tileX, tileY, numTilesX, numTilesY, tileWidth, tileHeight, maxSamples, maxDepth, currentBuffer;
		bool renderCompleted;
		float sampleCounter, totalTime;

    public:
        TiledRenderer(Scene *scene, const std::string& shadersDirectory);
		void rasterizeMeshes(Program *shader);
        ~TiledRenderer();
        
        void init();
        void finish();

        void render();
        void present() const;
        void update(float secondsElapsed);
        float getProgress() const;
    };
}