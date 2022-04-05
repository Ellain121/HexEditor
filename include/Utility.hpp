#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <vector>
#include <string>

typedef int Event;

constexpr int CTRL_Q = 'q' & 0x1F;
constexpr int CTRL_S = 's' & 0x1F;
constexpr int CTRL_X = 'x' & 0x1F;
constexpr int CTRL_A = 'a' & 0x1F;
constexpr int CTRL_Z = 'z' & 0x1F;
constexpr int CTRL_C = 'c' & 0x1F;
constexpr int CTRL_F = 'f' & 0x1F;
constexpr int KEY_NORMAL_ENTER = 10;

struct Point
{
    Point();
    Point(int x, int y);

    int x;
    int y;

    bool operator==(const Point& p);
};

struct Area
{
    Area();
    Area(int startX, int startY, int endX, int endY);

    int     startX;
    int     endX;
    int     startY;
    int     endY;
};

struct Area_1D
{
    Area_1D();
    Area_1D(int start, int end);

    int     start;
    int     end;

    const Area_1D& operator++(int);
    const Area_1D& operator--(int);
};

bool check_flags(int argc, char** argv, 
    const std::vector<std::string>& flags);
void print_help(char* bin);
char* get_arg(int argc, char** argv, const std::string& arg);


bool is_valid_hex_char(char hexChar);
char hexInt_to_hexChar(int hexInt);
uint8_t hexChar_to_hexInt(char hexChar);
std::string byte_to_hex(uint8_t byte);
uint8_t hex_to_byte(const std::string& hex);

#endif