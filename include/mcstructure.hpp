#ifndef MCSTRUCTURE_HPP
#define MCSTRUCTURE_HPP

#include "blockentity.hpp"
#include "blockstate.hpp"

namespace nbt
{

struct MCStructure
{
    MCStructure(int formatVersion = 1, int sizeX = 1, int sizeY = 1, int sizeZ = 1) :
        root(gCompound())
    {
        root << gInt("format_version", formatVersion);

        Tag size = gList("size", INT);
        size << gpInt(sizeX) << gpInt(sizeY) << gpInt(sizeZ);
        root << size;

        Tag swo = gList("structure_world_origin", INT);
        swo << gpInt(0) << gpInt(0) << gpInt(0);
        root << swo;

        Tag structure = gCompound("structure");
        Tag blockIndices = gList("block_indices", LIST);
        blockIndices << gpList(INT) << gpList(INT);
        Tag entities = gList("entities", COMPOUND);
        Tag palette = gCompound("palette");
        Tag blockPalette = gList("block_palette", COMPOUND);
        Tag blockPositionData = gCompound("block_position_data");
        palette << gCompound("default");
        palette["default"] << blockPalette << blockPositionData;

        structure << blockIndices << entities << palette;
        root << structure;
    };

    Tag &formatVersion()
    {
        return root[0];
    }

    Tag &size()
    {
        return root[1];
    }

    Tag &structureWorldOrigin()
    {
        return root[2];
    }

    Tag &blockIndices1()
    {
        return root[3][0][0];
    }

    Tag &blockIndices2()
    {
        return root[3][0][1];
    }

    Tag &entities()
    {
        return root[3][1];
    }

    Tag &blockPalette()
    {
        return root[3][2][0][0];
    }

    Tag &blockPositionData()
    {
        return root[3][2][0][1];
    }

    Tag root;
};

inline Tag getSingleBlockStructure(const std::string &blockId,
                                   const BlockEntityData &bed, const BlockStateData &bsd,
                                   int version = 18105860)
{
    MCStructure mcs;
    mcs.blockIndices1() << gpInt(0);
    mcs.blockIndices2() << gpInt(-1);
    Tag block = gpCompound();
    block << gString("name", blockId) << bsd.getTag() << gInt("version", version);
    mcs.blockPalette() << block;
    Tag bpd = gCompound("0");
    bpd << bed.getTag();
    mcs.blockPositionData() << bpd;
    return mcs.root;
}

}

#endif // !MCSTRUCTURE_HPP
