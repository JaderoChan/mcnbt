#include <iostream>
#include <utility> // std::pair

#define NDEBUG

#define MCNBT_USE_GZIP
#include <mcnbt/mcnbt.hpp>

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

    return { filename, isBigEndian };
}

Tag getTestNbt()
{
    auto root = gCompound("Root");

    auto list = gList(TT_INT, "Fibonacci");
    int a = 0, b = 1;
    for (size_t i = 0; i < 100; ++i)
    {
        int c = a + b;
        list << gInt(c);
        a = b;
        b = c;
    }

    root << list;

    return root;
}

void compressExample(const std::string& filename, bool isBigEndian)
{
    getTestNbt().write(filename, isBigEndian, true);
}

void decompressExample(const std::string& filename, bool isBigEndian)
{
    // #fromFile function automatically detects if the file is compressed.
    auto root = Tag::fromFile(filename, isBigEndian);
    std::cout << root.toSnbt() << std::endl;
}

int main()
{
    auto rtn = inputHint();

    compressExample(rtn.first, rtn.second);

    decompressExample(rtn.first, rtn.second);

    return 0;
}
