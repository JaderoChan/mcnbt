#ifndef MCSTRUCTURE_HPP
#define MCSTRUCTURE_HPP

#include "blockentity.hpp"
#include "blockstate.hpp"

struct MCStructure
{
    MCStructure(int formatVersion = 1, int sizeX = 1, int sizeY = 1, int sizeZ = 1) : root(Nbt::gCompound()) {
        using namespace Nbt;
        root << gInt("format_version", formatVersion);

        Tag size = gList("size", Int);
        size << gpInt(sizeX) << gpInt(sizeY) << gpInt(sizeZ);
        root << size;

        Tag swo = gList("structure_world_origin", Int);
        swo << gpInt(0) << gpInt(0) << gpInt(0);
        root << swo;

        Tag structure = gCompound("structure");
        Tag blockIndices = gList("block_indices", List);
        blockIndices << gpList(Int) << gpList(Int);
        Tag entities = gList("entities", Compound);
        Tag palette = gCompound("palette");
        Tag blockPalette = gList("block_palette", Compound);
        Tag blockPositionData = gCompound("block_position_data");
        palette << gCompound("default");
        palette["default"] << blockPalette << blockPositionData;

        structure << blockIndices << entities << palette;
        root << structure;
    };

    Nbt::Tag &formatVersion() { return root[0]; }
    Nbt::Tag &size() { return root[1]; }
    Nbt::Tag &structureWorldOrigin() { return root[2]; }
    Nbt::Tag &blockIndices1() { return root[3][0][0]; }
    Nbt::Tag &blockIndices2() { return root[3][0][1]; }
    Nbt::Tag &entities() { return root[3][1]; }
    Nbt::Tag &blockPalette() { return root[3][2][0][0]; }
    Nbt::Tag &blockPositionData() { return root[3][2][0][1]; }

    Nbt::Tag root;
};

static Nbt::Tag getSingleBlockStructure(const std::string &blockId,
                                        const BlockEntityData &bed, const BlockStateData &bsd,
                                        int version = 18105860) {
    MCStructure mcs;
    mcs.blockIndices1() << Nbt::gpInt(0);
    mcs.blockIndices2() << Nbt::gpInt(-1);
    Nbt::Tag block = Nbt::gpCompound();
    block << Nbt::gString("name", blockId) << bsd.getTag() << Nbt::gInt("version", version);
    mcs.blockPalette() << block;
    Nbt::Tag bpd = Nbt::gCompound("0");
    bpd << bed.getTag();
    mcs.blockPositionData() << bpd;
    return mcs.root;
}

#endif // !MCSTRUCTURE_HPP
