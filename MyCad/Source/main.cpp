#include "App/App.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>

int main(void)
{
    if (!glfwInit())
    {
        return -1;
    }

    App app;
    app.Run();

    return 0;
}
