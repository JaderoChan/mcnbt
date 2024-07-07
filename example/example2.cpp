#include <fstream>
#include <iostream>
#include <exception>

#include "nbt.hpp"

int main(){
    using namespace Nbt;

    // Get the tag of single command block.
    CommandBlockED bed("/say hello, world", 10, true, false, false);
    CommandBlockSD bsd;
    Tag root = getSingleBlockStructure("minecraft:command_block", bed, bsd);
    // Write the tag.
    std::ofstream os("D:/test.dat", std::ios_base::binary);
    if (os.is_open()) {
        root.write(os);
    }
    // Print the snbt.
    std::cout << root.toSnbt() << '\n' << std::endl;
    // Operator the tag.
    Tag size;
    // You should check specify member if exists before get it.
    if(root.hasMember("size"))
        size = root["size"];
    std::cout << size.toSnbt() << '\n' <<  std::endl;
    // You should use exception when operate the tag.
    try {
        root["size"] << size;
        // root["size"] << 1;
    } catch (std::exception &e) {
        std::cout << e.what() << '\n' <<  std::endl;
    }
    size.setName("size2");
    root << size;
    std::cout << root.toSnbt() << '\n' << std::endl;

    return 0;
}
