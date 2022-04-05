#include "Data.hpp"

#include <iostream>
#include <iterator>


Data::Data()
: mIsEditable(false)
, mFirstLine(0)
, mLastLine(0)
, mTotalLines(0)
{
}

void Data::init(char* inputFile)
{
    if (!inputFile && !read_from_stdin())
    {
        std::cerr << "Could not read from standard input!\n";
        std::exit(EXIT_FAILURE);
    }
    else if (inputFile && !read_from_file(inputFile))
    {
        std::cerr << "Could not read from " << inputFile << '\n';
        std::exit(EXIT_FAILURE);
    }
}

bool Data::read_from_file(const std::string& fname)
{
    if (!fs::exists(fname))
    {
        std::cerr << "File " << fname << " does not exist\n";
        return false;
    }

    mName = fs::canonical(fname);
    mFile.open(mName.string(), std::ios::in | std::ios::binary);

    if (!mFile)
    {
        std::cerr << "Failed to open " << fname << "\n";
        return false;
    }

    mBuff.reserve(fs::file_size(mName));
    mFile.unsetf(std::ios_base::skipws);
    mBuff.insert(mBuff.begin(), 
        std::istream_iterator<std::uint8_t>(mFile), {});
    mTotalLines = mBuff.size() / BYTES_PER_LINE;

    if (mBuff.size() % BYTES_PER_LINE)
        mTotalLines++;
    
    mIsEditable = true;
    return true;
}

bool Data::read_from_stdin()
{
    std::cin.unsetf(std::ios_base::skipws);
    std::istream_iterator<std::uint8_t> instream(std::cin);

    mName = "stdin";
    mBuff.insert(mBuff.begin(), instream, {});
    mTotalLines = mBuff.size() / BYTES_PER_LINE;

    if (mBuff.size() % BYTES_PER_LINE)
    {
        mTotalLines++;
    }

    return freopen("/dev/tty", "rw", stdin) != nullptr;
}

std::pair<std::set<int>, int> Data::find(const std::string& findStr, bool hex)
// int Data::find(const std::string& findStr, bool hex)
{
    int size = findStr.size();
    std::set<int> foundIndxs;
    std::vector<uint8_t> findBytes(findStr.size());
    for (int i = 0; i < findStr.size(); ++i)
    {
        findBytes[i] = (uint8_t)findStr[i];;
    }
    int strIndx = 0;
    for (int i = 0; i < mBuff.size(); ++i)
    {
        uint8_t byte = mBuff[i];
        if (byte == findStr[strIndx])
        {
            if (strIndx + 1 < findStr.size())
            {
                strIndx++;
            }
            else
            {
                foundIndxs.insert(i - strIndx);
                strIndx = 0;
            }
        }
    }

    return std::make_pair(foundIndxs, size);
    
}

void Data::save()
{
    if (!mIsEditable)
        return;
    
    std::ofstream out(mName.string(), std::ios::out | std::ios::binary);
    if (!out)
    {
        std::cerr << "Cannot open " << mName << "\n";
        std::exit(EXIT_FAILURE);
    }

    out.write(reinterpret_cast<char*>(mBuff.data()), mBuff.size());
    if (out.fail())
    {
        std::cerr << "Cannot save " << mName << "\n";
        std::exit(EXIT_FAILURE);
    }

    mUnsavedChanges.clear();
}