#ifndef DATA_HPP
#define DATA_HPP


#include <string>
#include <filesystem>
#include <set>
#include <vector>
#include <fstream>

namespace fs = std::filesystem;
typedef std::set<std::uint32_t> IntCache;

constexpr std::uint32_t BYTES_PER_LINE = 16;

class Data
{
    public: 
                Data();
        
        void    init(char* input_file = nullptr);

        bool    read_from_file(const std::string& fname);
        bool    read_from_stdin();
        void    save();

        std::pair<std::set<int>, int> find(const std::string& findStr, bool hex);
    
    public:
        bool                        mIsEditable;
        fs::path                    mName;
        IntCache                    mDirtyCache;
        std::ifstream               mFile;
        int                         mFirstLine;
        int                         mLastLine;
        int                         mTotalLines;
        std::vector<std::uint8_t>   mBuff;
        std::set<int>     mUnsavedChanges;
    };


#endif