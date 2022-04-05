#ifndef WINDOW_HPP
#define WINDOW_HPP

#include "Data.hpp"
#include "Utility.hpp"

#include <functional>

class Window
{
    private:
        enum class EditorMode
        {
            HEX,
            ASCII,
        };

    public:
                Window(int argc, char** argv);
                ~Window();
        
        void    update();
        void    draw();
        void    handleEvent(Event event);
        
        void    move_right();
        void    move_left();
        void    move_up();
        void    move_down();
        void    edit_byte(int ch);

        void    goto_byte(uint8_t byte);


        void    draw_line(int line);
        void    save();

        void    activateFindMode();
        void    deactivateFindMode();

    
    private:
        const Area              mHexArea;
        const Area              mAsciiArea;
        Area_1D                 mVisibleLines;
        Data                    mData;
        Point                   mCursor;
        EditorMode              mEditMode;
        int                     mCurrentByte;
        int                     mCurrentByteOffset;

        // for HEX mod
        
};

#endif