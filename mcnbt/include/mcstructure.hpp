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

struct MCStructure
{
    MCStructure(int formatVersion = 1, int sizeX = 1, int sizeY = 1, int sizeZ = 1) : root(Nbt::gCompound()) {
        using namespace Nbt;
        root << gInt("format_version", formatVersion);

        Tag size = gList("size", Int);
        size << gpInt(sizeX) << gpInt(sizeY) << gpInt(sizeZ);
        root << size.move();

        Tag swo = gList("structure_world_origin", Int);
        swo << gpInt(0) << gpInt(0) << gpInt(0);
        root << swo.move();

        Tag structure = gCompound("structure");
        Tag blockIndices = gList("block_indices", List);
        blockIndices << gpList(Int) << gpList(Int);
        Tag entities = gList("entities", Compound);
        Tag palette = gCompound("palette");
        Tag blockPalette = gList("block_palette", Compound);
        Tag blockPositionData = gCompound("block_position_data");
        palette << gCompound("default");
        palette["default"] << blockPalette.move() << blockPositionData.move();

        structure << blockIndices.move() << entities.move() << palette.move();
        root << structure.move();
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
    mcs.blockPalette() << block.move();
    Nbt::Tag bpd = Nbt::gCompound("0");
    bpd << bed.getTag();
    mcs.blockPositionData() << bpd.move();
    return mcs.root;
}

#endif // !MCSTRUCTURE_HPP
