#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include "Utility.hpp"
#include "Window.hpp"

#include "ncurses.h"

#include <iostream>

class Application
{
    private:
        enum Mode
        {
            HEX,
            ASCII
        } ;

    public:
            Application(int argc, char** argv);

        void    run();
    
    private:
        Window          mWindow;


           
};

#endif