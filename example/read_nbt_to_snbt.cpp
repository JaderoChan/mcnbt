#include <iostream>

#include <mcnbt/mcnbt.hpp>

int main()
{
    std::string filepath;
    std::cout << "Enter the NBT file path:" << std::endl;
    std::cin >> filepath;

    try
    {
        auto kv = nbt::Tag::parse(filepath, false);
        auto rootName = kv.first;
        auto root = std::move(kv.second);
        std::cout << root.toSnbt(2) << std::endl;
    }
    catch (std::exception& e)
    {
        try
        {
            auto kv = nbt::Tag::parse(filepath, true);
            auto rootName = kv.first;
            auto root = std::move(kv.second);
            std::cout << root.toSnbt(2) << std::endl;
        }
        catch (std::exception& e)
        {
            std::cout << "Failed to parse the NBT file: " << e.what() << std::endl;
            return 1;
        }
    }

    return 0;
}
