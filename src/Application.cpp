#include "Application.hpp"

#include "Utility.hpp"

Application::Application(int argc, char** argv)
: mWindow(argc, argv)
{
}

void Application::run()
{
    bool appClosed = false;
    while (!appClosed)
    {
        Event event = getch();
        if (event == CTRL_C)
        {
            appClosed = true;
        }
        mWindow.handleEvent(event);
        mWindow.update();
        mWindow.draw();
    }
}