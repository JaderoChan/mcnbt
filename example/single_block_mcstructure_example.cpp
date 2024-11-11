#include <iostream>

#include <mcnbt/mcstructure.hpp>

int main()
{
    // Create a single block structure of command block. (Bedrock Edition)
    nbt::CommandBlockED bed("say hello world!", 20, true, true, false);
    nbt::CommandBlockSD bsd(false, nbt::CommandBlockSD::FD_UP);
    auto structure = nbt::createSingleBlockStructure("minecraft:command_block", bed, bsd);

    // Save the structure to a file. (Bedrock Edition, Little Endian)
    structure.write("single_block_mcstructure_example.mcstructure", false);

    return 0;
}