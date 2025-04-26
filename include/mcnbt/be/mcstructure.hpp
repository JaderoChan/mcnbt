#ifndef MCNBT_BE_MCSTRUCTURE_HPP
#define MCNBT_BE_MCSTRUCTURE_HPP

#include "block_entity.hpp"
#include "block_state.hpp"

namespace nbt
{

namespace be
{

struct MCStructure
{
    MCStructure(Int32 formatVersion = 1, Int32 sizeX = 1, Int32 sizeY = 1, Int32 sizeZ = 1) :
        root(gCompound())
    {
        // Write format version.
        root << gInt(formatVersion, "format_version");

        // Write size.
        Tag size = gList(TT_INT, "size");
        size << gInt(sizeX) << gInt(sizeY) << gInt(sizeZ);
        root << size;

        // Write structure world origin.
        Tag swo = gList(TT_INT, "structure_world_origin");
        swo << gInt(0) << gInt(0) << gInt(0);
        root << swo;

        // Create structure tag.
        Tag structure = gCompound("structure");

        // Create block indices.
        Tag blockIndices = gList(TT_LIST, "block_indices");
        blockIndices << gList(TT_INT) << gList(TT_INT);

        // Create entities tag.
        Tag entities = gList(TT_COMPOUND, "entities");

        // Create palette tag.
        Tag palette = gCompound("palette");
        Tag blockPalette = gList(TT_COMPOUND, "block_palette");

        // Create block position data tag.
        Tag blockPositionData = gCompound("block_position_data");
        palette << gCompound("default");
        palette["default"] << blockPalette << blockPositionData;

        // Merge block data.
        structure << blockIndices << entities << palette;

        // Write structure tag.
        root << structure;
    };

    Tag& formatVersion()            { return root[0]; }

    Tag& size()                     { return root[1]; }

    Tag& structureWorldOrigin()     { return root[2]; }

    Tag& blockIndices1()            { return root[3][0][0]; }

    Tag& blockIndices2()            { return root[3][0][1]; }

    Tag& entities()                 { return root[3][1]; }

    Tag& blockPalette()             { return root[3][2][0][0]; }

    Tag& blockPositionData()        { return root[3][2][0][1]; }

    Tag root;
};

inline Tag createSingleBlockStructure(const std::string& blockId,
                                      const CommonBlockEntityData& bed, const CommonBlockStateData& bsd,
                                      Int32 version = 18105860)
{
    MCStructure mcs;
    mcs.blockIndices1() << gInt(0);
    mcs.blockIndices2() << gInt(-1);

    Tag block = gCompound();
    block << gString(blockId, "name") << bsd.getTag() << gInt(version, "version");
    mcs.blockPalette() << block;

    Tag bpd = gCompound("0");
    bpd << bed.getTag();
    mcs.blockPositionData() << bpd;

    return mcs.root;
}

} // namespace be

} // namespace nbt

#endif // !MCNBT_BE_MCSTRUCTURE_HPP
