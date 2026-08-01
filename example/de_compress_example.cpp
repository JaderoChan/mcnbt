#include <iostream>
#include <utility> // std::pair

#include <mcnbt/mcnbt.hpp>

#ifndef MCNBT_HAS_ZLIB
    #error "Can't build this project without zlib/gzip support"
#endif // !MCNBT_HAS_ZLIB

using namespace nbt;

std::pair<std::string, bool> inputHint()
{
    std::string filename;
    bool isBigEndian = false;

    std::cout << "Enter filename: ";
    std::getline(std::cin, filename);

    while (true)
    {
        std::cout << "Is the file big-endian? (y/n): ";
        std::cin.ignore();
        int key = std::cin.get();
        if (key == 'y' || key == 'Y')
        {
            isBigEndian = true;
            break;
        }
        else if (key == 'n' || key == 'N')
        {
            isBigEndian = false;
            break;
        }
        else
        {
            std::cout << "Invalid input. Please enter 'y' or 'n'." << std::endl;
            continue;
        }
    }

    return {filename, isBigEndian};
}

Tag getTestNbt()
{
    auto root = Tag::compound();

    auto list = Tag::list();
    int a = 0, b = 1;
    for (size_t i = 0; i < 100; ++i)
    {
        int c = a + b;
        list << Tag(int32_t(c));
        a = b;
        b = c;
    }

    root["Fibonacci"] = std::move(list);

    return root;
}

void compressExample(const std::string& filename, bool isBigEndian)
{
    getTestNbt().dumpCompressed(filename, isBigEndian, "Root");
}

void decompressExample(const std::string& filename, bool isBigEndian)
{
    // parse() decompresses gzip-compressed streams automatically.
    auto result = Tag::parse(filename, isBigEndian);
    std::cout << result.second.toSnbt(4) << std::endl;
}

int main()
{
    auto rtn = inputHint();
    compressExample(rtn.first, rtn.second);
    decompressExample(rtn.first, rtn.second);

    return 0;
}
