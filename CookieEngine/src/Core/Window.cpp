#include "Core/Window.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace Cookie::Core;

constexpr int initWidth = 1280;
constexpr int initHeight = 720;

void debugGLFWCallback(int error_code, const char* description)
{
    printf("GLFW error (%x) = %s\n", error_code, description);
}

/*============== CONSTRUCTORS/DESTRUCTORS =================*/

Window::Window() : width{ initWidth }, height{initHeight}
{
    // Init glfw
    if (!glfwInit())
    {
        fprintf(stderr, "glfwInit failed");
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwSetErrorCallback(debugGLFWCallback);

    //create window
    window = glfwCreateWindow(initWidth, initHeight, "Cookie Engine", nullptr, nullptr);
    if (window == nullptr)
    {
        fprintf(stderr, "glfwCreateWindow failed");
    }

    SetIcon();
}

Window::~Window()
{
    glfwDestroyWindow(window);
    glfwTerminate();
}

void Window::SetIcon()
{
    /* a bit rough, but working. makes it that you should always have a Assets file with CookieEngine_Icon.png in it */
    GLFWimage images[1];
    images[0].pixels = stbi_load("Assets/CookieEngine_Icon.png", &images[0].width, &images[0].height, 0, 4); //rgba channels 
    glfwSetWindowIcon(window, 1, images);
    stbi_image_free(images[0].pixels);
}