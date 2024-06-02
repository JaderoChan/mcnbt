// The "mcnbt" library written in c++.
//
// Webs: https://github.com/JaderoChan/mcnbt
//
// MIT License
//
// Copyright (c) 2024 頔珞JaderoChan
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef MCSTRUCTURE_HPP
#define MCSTRUCTURE_HPP

#include "blockentity.hpp"
#include "blockstate.hpp"

namespace Nbt
{

struct MCStructure
{
    MCStructure(int formatVersion = 1, int sizeX = 1, int sizeY = 1, int sizeZ = 1) : root(gCompound()) {
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

    Tag &formatVersion() { return root[0]; }
    Tag &size() { return root[1]; }
    Tag &structureWorldOrigin() { return root[2]; }
    Tag &blockIndices1() { return root[3][0][0]; }
    Tag &blockIndices2() { return root[3][0][1]; }
    Tag &entities() { return root[3][1]; }
    Tag &blockPalette() { return root[3][2][0][0]; }
    Tag &blockPositionData() { return root[3][2][0][1]; }

    Tag root;
};

static Tag getSingleBlockStructure(const std::string &blockId,
                                        const BlockEntityData &bed, const BlockStateData &bsd,
                                        int version = 18105860) {
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
