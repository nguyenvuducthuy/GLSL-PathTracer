#define _USE_MATH_DEFINES

// third-party libraries
#include <GL/glew.h>
#include <GLFW/glfw3.h>	
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <time.h>
#include <math.h>

#include "Scene.h"
#include "TiledRenderer.h"
#include "ProgressiveRenderer.h"
#include "Camera.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "teapotTestScene.h"
#include "boyTestScene.h"

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

using namespace glm;
using namespace std;
using namespace GLSLPT;

float moveSpeed = 0.5f;
float mouseSensitivity = 0.05f;
bool keyPressed = false;
Scene *scene = nullptr;
Renderer *renderer = nullptr;

RenderOptions renderOptions;

bool initRenderer()
{
    delete renderer;
    if (scene->renderOptions.rendererType == Renderer_Tiled)
    {
        renderer = new TiledRenderer(scene, "../PathTracer/shaders/Tiled/");
    }
    else 
	if (scene->renderOptions.rendererType == Renderer_Progressive)
    {
        renderer = new ProgressiveRenderer(scene, "../PathTracer/shaders/Progressive/");
    }
	else
	{
		printf("Invalid Renderer Type\n");
        return false;
	}
    renderer->init();
    return true;
}

void render(GLFWwindow *window)
{
	renderer->render();
    const glm::ivec2 screenSize = renderer->getScreenSize();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, screenSize.x, screenSize.y);
    renderer->present();

    // Rendering
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	glfwSwapBuffers(window);
}

void update(float secondsElapsed, GLFWwindow *window)
{
	renderer->update(secondsElapsed);
	scene->camera->isMoving = false;
	//Camera Movement
	if (glfwGetKey(window, 'W')){
		scene->camera->offsetPosition(secondsElapsed * moveSpeed * scene->camera->forward);
		scene->camera->isMoving = true;
	}
	else if (glfwGetKey(window, 'S')){
		scene->camera->offsetPosition(secondsElapsed * moveSpeed * -scene->camera->forward);
		scene->camera->isMoving = true;
	}
    if (glfwGetKey(window, 'A')){
		scene->camera->offsetPosition(secondsElapsed * moveSpeed * -scene->camera->right);
		scene->camera->isMoving = true;
	}
	else if (glfwGetKey(window, 'D')){
		scene->camera->offsetPosition(secondsElapsed * moveSpeed * scene->camera->right);
		scene->camera->isMoving = true;
	}
    
	//Mouse Handling
   if (!ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow) && ImGui::IsMouseDown(0))
    {
        ImVec2 mouseDelta = ImGui::GetMouseDragDelta();
        scene->camera->offsetOrientation(mouseSensitivity * mouseDelta.x, mouseSensitivity * mouseDelta.y);
        scene->camera->isMoving = true;
        ImGui::ResetMouseDragDelta();
    }
}

void main()
{
	srand(unsigned int(time(0)));

	scene = new Scene();
	loadBoyTestScene(scene);

	renderOptions = scene->renderOptions;
	GLFWwindow *window;
	glfwInit();
	
	window = glfwCreateWindow((int)scene->renderOptions.resolution.x, (int)scene->renderOptions.resolution.y, "PathTracer", 0, 0);
	glfwSetWindowPos(window, 300, 100);
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	//glfwSetCursorPos(window, 0, 0);
	glfwMakeContextCurrent(window);
	glfwSwapInterval(0);
	glewInit();

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

#if __APPLE__
// GL 3.2 + GLSL 150
    const char* glsl_version = "#version 150";
    /*glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
    */
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    if (!initRenderer())
        return;

	double lastTime = glfwGetTime();
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        {
            ImGui::Begin("GLSL PathTracer");  // Create a window called "Hello, world!" and append into it.

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

            bool renderOptionsChanged = false;
            renderOptionsChanged |= ImGui::Combo("Render Type", &renderOptions.rendererType, "Progressive\0Tiled\0");
            renderOptionsChanged |= ImGui::InputInt2("Resolution", &renderOptions.resolution.x);
            renderOptionsChanged |= ImGui::InputInt("Max Samples", &renderOptions.maxSamples);
            renderOptionsChanged |= ImGui::InputInt("Max Depth", &renderOptions.maxDepth);
            renderOptionsChanged |= ImGui::InputInt("Tiles X", &renderOptions.numTilesX);
            renderOptionsChanged |= ImGui::InputInt("Tiles Y", &renderOptions.numTilesY);
            renderOptionsChanged |= ImGui::Checkbox("Use envmap", &renderOptions.useEnvMap);
            renderOptionsChanged |= ImGui::InputFloat("HDR multiplier", &renderOptions.hdrMultiplier);

            if (renderOptionsChanged)
            {
                scene->renderOptions = renderOptions;
                initRenderer();
            }
            ImGui::End();
        }


		if (glfwGetKey(window, GLFW_KEY_ESCAPE))
			glfwSetWindowShouldClose(window, GL_TRUE);
		double presentTime = glfwGetTime();
		update((float)(presentTime - lastTime), window);
		lastTime = presentTime;
        
		render(window);
	}

    delete renderer;
    delete scene;

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

	glfwTerminate();
}

