#include "Utility.hpp"

#include <algorithm>
#include <iostream>

Point::Point() : x(0), y(0) {}
Point::Point(int x, int y) : x(x), y(y) {}
bool Point::operator==(const Point& p)
{
    return p.x == x && p.y == y;
}

Area::Area() : startX(0), startY(0), endX(0), endY(0)
{
}

Area::Area(int startX, int startY, int endX, int endY) 
: startX(startX), startY(startY), endX(endX), endY(endY)
{
}

Area_1D::Area_1D() : start(0), end(0)
{
}

Area_1D::Area_1D(int start, int end) : start(start), end(end)
{
}

const Area_1D& Area_1D::operator++(int)
{
    start++;
    end++;
    return *this;
}

const Area_1D& Area_1D::operator--(int)
{
    start--;
    end--;
    return *this;
}

bool check_flags(int argc, char** argv, 
    const std::vector<std::string>& flags)
{
    for (auto& flag : flags)
    {
        if (std::find(argv, argv + argc, flag) != argv + argc)
        {
            return true;
        }
    }
    return false;
}

void print_help(char* bin)
{
    std::cerr << "USAGE: " << bin << " [-f file] [-o offset]\n";
}

char* get_arg(int argc, char** argv, const std::string& arg)
{
    auto res = std::find(argv, argv + argc, arg);
    if (res != argv + argc && ++res != argv + argc)
        return *res;
    
    return nullptr;
}

bool is_valid_hex_char(char hexChar)
{
    return ((hexChar >= '0' && hexChar <= '9') 
        || (hexChar >= 'A' && hexChar <= 'F')
        || (hexChar >= 'a' && hexChar <= 'f'));
}

char hexInt_to_hexChar(int hexInt)
{
    if (hexInt < 10)
    {
        return hexInt + '0';
    }
    else
    {
        return hexInt - 10 + 'A';
    }
}

uint8_t hexChar_to_hexInt(char hexChar)
{
    if (hexChar >= '0' && hexChar <= '9')
    {
        return hexChar - '0';
    }
    else
    {
        return hexChar - 'A' + 10;
    }
}

std::string byte_to_hex(uint8_t byte)
{
    std::string hex(2, ' ');

    hex[0] = hexInt_to_hexChar(byte / 16);
    hex[1] = hexInt_to_hexChar(byte % 16);

    return hex;
}

uint8_t hex_to_byte(const std::string& hex)
{
    uint8_t half_byte_1 = hexChar_to_hexInt(hex[0]);
    uint8_t half_byte_2 = hexChar_to_hexInt(hex[1]);
    return (half_byte_1 << 4) + half_byte_2;
}