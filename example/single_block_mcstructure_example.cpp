#include <iostream>

#include <mcnbt/be/mcstructure.hpp>

namespace be = nbt::be;

int main()
{
    // Create a single block structure of command block. (Bedrock Edition)
    be::CommandBlockBED bed("say hello world!", 20, true, true, false);
    be::CommandBlockBSD bsd(false, be::CommandBlockBSD::FD_UP);
    auto structure = be::createSingleBlockStructure("minecraft:command_block", bed, bsd);

    // Save the structure to a file. (Bedrock Edition, Little Endian)
    std::cout << structure.toSnbt() << std::endl;
    structure.write("single_block_mcstructure_example.mcstructure", false);

    return 0;
}
