#include <iostream>

#include <mcnbt/mcnbt.hpp>

int main()
{
    std::string filepath;
    std::cout << "Enter the NBT file path:" << std::endl;
    std::cin >> filepath;

    using ResultType = decltype(nbt::Tag::parse("", false));
    ResultType result;

    try
    {
        result = nbt::Tag::parse(filepath, false);
        std::cout << "[Little Endian]" << std::endl;
    }
    catch (std::exception& e)
    {
        try
        {
            result = nbt::Tag::parse(filepath, true);
            std::cout << "[Big Endian]" << std::endl;
        }
        catch (std::exception& e)
        {
            std::cout << "Failed to parse the NBT file: " << e.what() << std::endl;
            return 1;
        }
    }

    auto rootName = result.first;
    auto root = std::move(result.second);
    std::cout << root.toSnbt(2) << std::endl;

    return 0;
}
