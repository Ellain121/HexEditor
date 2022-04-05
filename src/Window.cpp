#include "Window.hpp"

#include "ncurses.h"

#include <cctype>
#include <algorithm>

#define LINE_CRUNCH 24


Window::Window(int argc, char** argv)
: mHexArea(11, 1, 57, LINE_CRUNCH - 2)
, mAsciiArea(1, 1, 1, 1)
, mData()
, mCursor(mHexArea.startX, mHexArea.startY)
, mEditMode(EditorMode::HEX)
, mCurrentByte(0)
, mCurrentByteOffset(0)
{
    bool help_flag = check_flags(argc - 1, argv + 1, {"-h", "--help"});
    if (help_flag)
    {
        print_help(*argv);
        std::exit(EXIT_SUCCESS);
    }

    auto input_file = get_arg(argc - 1, argv + 1, "-f");
    mData.init(input_file);

    initscr();
    raw();
    start_color();
    init_pair(1, COLOR_BLACK, COLOR_GREEN);
    init_pair(2, COLOR_GREEN, 0);
    init_pair(3, COLOR_BLACK, COLOR_CYAN);
    keypad(stdscr, true);
    

    // mHexArea = Area(11, 1, 57, LINES - 2);
    mVisibleLines = Area_1D(0, LINES - 2);
}

Window::~Window()
{
    endwin();
}

void Window::update()
{
    move(1, 11);
}

void Window::goto_byte(uint8_t byte)
{
    mCurrentByte = byte;
    mVisibleLines.start = mCurrentByte / BYTES_PER_LINE;
    mVisibleLines.end = mVisibleLines.start + LINES - 2;
}

void Window::draw_line(int line)
{
    int col = 11;
    int lineIndx  = mVisibleLines.start + line;
    int byteIndex = lineIndx * BYTES_PER_LINE;
    int bytesToDraw = BYTES_PER_LINE;

    if (mData.mTotalLines - 1 == lineIndx 
        && mData.mBuff.size() % BYTES_PER_LINE)
        bytesToDraw = mData.mBuff.size() % BYTES_PER_LINE;

    mvprintw(line + 1, 1, "%08X", byteIndex);
    // HEX
    for (int i = 0; i < bytesToDraw; ++i, byteIndex++, col+=3)
    {
        if (mEditMode == EditorMode::ASCII && byteIndex == mCurrentByte)
        {
            attron(A_REVERSE);
            mvprintw(line + 1, col, "%02X", mData.mBuff[byteIndex]);
            attroff(A_REVERSE);
            continue;
        }
        if (mData.mUnsavedChanges.find(byteIndex) != mData.mUnsavedChanges.end())
        {
            int color = (mCurrentByte == byteIndex ? 2 : 1);
            attron(COLOR_PAIR(color));
            mvprintw(line + 1, col, "%02X", mData.mBuff[byteIndex]);
            attroff(COLOR_PAIR(color));
            continue;
        }
        
        mvprintw(line + 1, col, "%02X", mData.mBuff[byteIndex]);
    }
    mvprintw(line + 1, col, "  ");

    byteIndex = lineIndx * BYTES_PER_LINE;

    // ASCII
    for (int i = 0; i < bytesToDraw; ++i, ++col, ++byteIndex)
    {
        char c = mData.mBuff[byteIndex];
        if (mEditMode == EditorMode::HEX && byteIndex == mCurrentByte)
        {
            attron(A_REVERSE);
            mvprintw(line + 1, col, "%c", std::isprint(c) ? c : '.');
            attroff(A_REVERSE);
            continue;
        }
        if (mData.mUnsavedChanges.find(byteIndex) != mData.mUnsavedChanges.end()
            && mCurrentByte != byteIndex)
        {
            int color = (mCurrentByte == byteIndex ? 2 : 1);
            attron(COLOR_PAIR(color));
            mvprintw(line + 1, col, "%c", std::isprint(c) ? c : '.');
            attroff(COLOR_PAIR(color));
            continue;
        }
        mvprintw(line + 1, col, "%c", std::isprint(c) ? c : '.');
    }
}

void Window::draw()
{
    clear();
    box(stdscr, 0, 0);

    for (int line = 0; line < LINES - 2; ++line)
        draw_line(line);
    
    const std::string filename = mData.mName.filename();
    const char mode = mEditMode == EditorMode::ASCII ? 'A' : 'X';
    const int percentage = static_cast<float>(mData.mLastLine) / 
                                    mData.mTotalLines * 100;

    // mvprintw(LINES - 1, 3, "Debug: x: %d  y: %d  mCurByte: %d  mCurByOffs: %d, vl: start:%d end:%d", mCursor.x, mCursor.y, mCurrentByte, mCurrentByteOffset, 
    // mVisibleLines.start, mVisibleLines.end);
    mvprintw(LINES - 1, COLS - 20, "Mode: %c|file: %d%%", mode, percentage);
    move(mCursor.y, mCursor.x);
}

void Window::handleEvent(Event event)
{
    switch (event)
    {
        case KEY_RIGHT:
            move_right();
            break;
        case KEY_LEFT:
            move_left();
            break;
        case KEY_UP:
            move_up();
            break;
        case KEY_DOWN:
            move_down();
            break;
        case CTRL_S:
            mData.save();
            break;
        case CTRL_F:
            
            break;
        default:
            if (isprint(event))
                edit_byte(event);
            break;
        // win.move_up();
        // break;
    }
}

void Window::edit_byte(int ch)
{
    if (mEditMode == EditorMode::HEX)
    {
        if (!is_valid_hex_char(ch)) return;
        ch = toupper(ch);

        uint8_t byte = mData.mBuff[mCurrentByte];
        std::string byteHex = byte_to_hex(byte);
        byteHex[mCurrentByteOffset] = ch;

        
        mData.mBuff[mCurrentByte] = hex_to_byte(byteHex);
        mData.mUnsavedChanges.insert(mCurrentByte);
    }
    if (mEditMode == EditorMode::ASCII)
    {

    }
}

void Window::move_up()
{
    if (mCursor.y > mHexArea.startY)
    {
        mCursor.y--;
        mCurrentByte -= BYTES_PER_LINE;
    }
    else
    {
        if (mVisibleLines.start > 0)
        {
            mVisibleLines--;
            mCurrentByte -= BYTES_PER_LINE;
        }
    }
}

void Window::move_down()
{
    if (mCursor.y < mHexArea.endY)
    {
        mCursor.y++;
        mCurrentByte += BYTES_PER_LINE;
    }
    else
    {
        mVisibleLines++;
        mCurrentByte += BYTES_PER_LINE;
    }
}

void Window::move_left()
{
    if (mCurrentByte == 0 && mCurrentByteOffset == 0) return;

    if (mEditMode == EditorMode::HEX)
    {
        // if we need to go to the previous byte
        if (mCurrentByteOffset == 0)
        {
            // if we need to go to the previous line
            if (mCursor.x <= mHexArea.startX)
            {
                // if we don't need lines from above
                if (mCursor.y > mHexArea.startY)
                {
                    mCursor.x = mHexArea.endX;
                    mCurrentByte--;
                    mCurrentByteOffset = 1;
                    mCursor.y--;
                }
                else
                {
                    // if we need lines from above
                    // and there are some above
                    if (mVisibleLines.start > 0)
                    {
                        mVisibleLines--;
                        mCursor.x = mHexArea.endX;
                        mCurrentByte--;
                        mCurrentByteOffset = 1;
                    }
                }
                
            }
            else // same line, just one byte before
            {
                mCursor.x -= 2;
                mCurrentByte--;
                mCurrentByteOffset = 1;
            }
            // mCursor.x -= 2;
            // mCurrentByte--;
            // mCurrentByteOffset = 1;
            // if (mCursor.x < mHexArea.startX)
            // {
            //     mCursor.x = mHexArea.endX;
            //     mCursor.y--;
            // }
        }
        else // move cursor from 2nd half-byte ---> 1st half-byte
        {
            mCursor.x--;
            mCurrentByteOffset--;
        }
    }
    if (mEditMode == EditorMode::ASCII)
    {

    }
}

void Window::move_right()
{
    int lineIndx = mCurrentByte / BYTES_PER_LINE;
    int lineSize = BYTES_PER_LINE;

    if (lineIndx == mData.mTotalLines - 1)
        lineSize = mData.mBuff.size() % BYTES_PER_LINE;

    if (mEditMode == EditorMode::HEX)
    {
        if (mCurrentByteOffset > 0)
        {
            // if cursor will be located on the next line 
            if ((mCurrentByte + 1) % BYTES_PER_LINE == 0)
            {
                // if next line exist on the screen
                if (mCursor.y < mHexArea.endY)
                {
                    mCursor.x = mHexArea.startX;
                    mCursor.y++;
                    mCurrentByte++;
                    mCurrentByteOffset = 0;
                }
                else // if it exist, but not on the screen
                {
                    mVisibleLines++;
                    mCursor.x = mHexArea.startX;
                    mCurrentByte++;
                    mCurrentByteOffset = 0;
                }
            }
            else // cursor on the same line
            {
                mCursor.x += 2;
                mCurrentByte++;
                mCurrentByteOffset = 0;
            }
        }
        else // if mCurrentByteOffset == 0;
        // HEX Area consist of 2 half byte record, so you just move cursor  1 half-byte ---> 2 half-byte
        {
            mCursor.x++;
            mCurrentByteOffset++;
        }
    }
    if (mEditMode == EditorMode::ASCII)
    {

    }
}

