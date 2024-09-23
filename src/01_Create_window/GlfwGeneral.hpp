#include "VKBase.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#pragma comment(lib, "glfw.lib");

GLFWwindow *pWindow;
GLFWmonitor *pMonitor;
const char *windowTitle = "EasyVK";

bool InitializeWindow(VkExtent2D size, bool fullscreen = false, bool isResizable = true, bool limitFrameRate = true)
{
    if (!glfwInit())
    {
        using namespace std;
    }
}

void TerminateWindow()
{
}