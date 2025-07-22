#include "App/App.h"

#include "Platform/OpenGL/OpenGLApi.h"
#include "Platform/Imgui/ImguiApi.h"

#include <iostream>

int main(void)
{
    if (!OpenGLApi::InitGLFW())
    {
        return -1;
    }

    App app;

    OpenGLApi::InitRendering();

    app.Run();

	OpenGLApi::DeinitGLFW();
    
    return 0;
}
