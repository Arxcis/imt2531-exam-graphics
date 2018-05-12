#include <iostream>
#include <string>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw_gl3.h>

#include <overkill/Config.hpp>
#include <overkill/Init.hpp>
#include <overkill/Input.hpp>
#include <overkill/gl_caller.hpp>
#include <overkill/Renderer.hpp>
#include <overkill/Util.hpp>

#include <overkill/GUI.hpp>

#include <overkill/TextureSystem.hpp>
#include <overkill/ShaderSystem.hpp>
#include <overkill/MaterialSystem.hpp>
#include <overkill/Scene.hpp>
#include <overkill/ModelSystem.hpp>
#include <overkill/EntityModel.hpp>
#include <overkill/EntityCamera.hpp>
#include <overkill/Watcher.hpp>


using namespace overkill;



int main(int argc, char** args)
{
    if (argc > 2)
    {
        LOG_ERROR("No more than 0 or 1 parameters. Parameter is path to scene file to load. Example: ./cube assets/scenes/_default.yml");
    }

    Init::Config();

    // Init libraries + Watcher callbacks
	C::window = Init::GLFW(
        C::GLVersionMajor, 
        C::GLVersionMinor, 
        C::WinWidth, 
        C::WinHeight,
        C::WinName);

    Init::GLEW();
    Init::OpenGL(C::ClearColor);
    Init::ImGUI(C::window);

    Watcher::pollEvents();

    // Load asset subsystems
    TextureSystem::load();
    ShaderSystem::createUniformBuffers();
    ShaderSystem::load();
    MaterialSystem::load();
    ModelSystem::load();
    

    
    LOG_DEBUG("Argc %d, args[1] %s", argc, args[1]);

    if (argc == 1)
    {
        Scene::load("assets/scenes/_default.yml");
    }
    else if (argc == 2)
    {
        Scene::load(std::string(args[1]));
    }


    float oldT = 0, t = 0, dt = 0;

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    bool show_demo_window = true;
    bool show_another_window = false;

    for(;;)
    {

        t = (float)glfwGetTime();
        

        dt = t - oldT;


        if ((glfwGetKey(C::window, GLFW_KEY_ESCAPE) == GLFW_PRESS || glfwWindowShouldClose(C::window) != 0))
            break;

        Renderer::clear();
        Scene::update(dt);
        

        // Draws all the models in the scene.
        Scene::draw(t);     


        //
        // CONTROL TIME
        //

        // 1. Elapsed time
        Scene::times.elapsed_time = t;


        // 2. Day time in seconds
        if (C::DayRunning) 
        {
            const auto TimeMultiplier = 10000; // 10k seconds for every second
            Scene::times.timeofday_seconds += dt * TimeMultiplier;
        }

        // 3. Year time in days
        if (C::YearRunning) 
        {
            const auto TimeMultiplier = 10; // 30 days for very second
            Scene::times.timeofyear_days += dt *  TimeMultiplier;

        }


        //This can be done to debug positions
        //glm::mat4 camPosDebugM2W = glm::translate(glm::mat4(1), glm::vec3(cameraTransform.position));
        //Renderer::draw(ModelSystem::getById(0), camPosDebugM2W, t);

 
        //
        // RENDER GUI
        //
        GUI::begin();
        GUI::updateTimeOfDay(Scene::times.timeofday_seconds);
        GUI::updateTimeOfYear(Scene::times.timeofyear_days);
        GUI::end();

        //
        // RENDER OpenGL
        //
        glfwSwapBuffers(C::window);
        glfwPollEvents();
        
        oldT = t;

        // break; // For testing load performance
    }

    Scene::clean();
    ModelSystem::clean();
    ShaderSystem::clean();
    TextureSystem::clean();


    ImGui_ImplGlfwGL3_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(C::window);
    glfwTerminate();
	return 0;
}
