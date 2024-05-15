// Project "mcnbt" library by 頔珞JaderoChan
//
// MIT License
//
// Copyright (c) 2024 頔珞JaderoChan
//
// https://github.com/JaderoChan/mcnbt
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

#ifndef BLOCKSTATE_HPP
#define BLOCKSTATE_HPP

#include "nbt.hpp"

#include <string>

struct BlockStateData
{
    BlockStateData() {}
    ~BlockStateData() {}
    Nbt::Tag getTag() const {
        Nbt::Tag tag = Nbt::gCompound("states");
        _write(tag);
        return tag;
    };

protected:
    virtual void _write(Nbt::Tag &tag) const = 0;
};

struct CommandBlockSD final : BlockStateData
{
    enum FacingDirection : char
    {
        Up,
        Down,
        North,
        South,
        West,
        East
    };

    CommandBlockSD(bool isConditional = false, FacingDirection fd = Up) :
        isConditional(isConditional), fd(fd) {}

    bool isConditional;
    FacingDirection fd = Up;

private:
    void _write(Nbt::Tag &tag) const override {
        tag << Nbt::gByte("conditional_bit", static_cast<char>(isConditional));
        tag << Nbt::gInt("facing_direction", static_cast<int>(fd));
    };
};

struct StructureBlockSD final : BlockStateData
{
    enum Mode : char
    {
        Save,
        Load,
        Corner
    };

    StructureBlockSD(Mode mode = Load) : mode(mode) {}

    Mode mode = Load;

private:
    std::string _modestr() const {
        if (mode == Save)
            return std::string("save");
        if (mode == Load)
            return std::string("load");
        if (mode == Corner)
            return std::string("corner");
        return std::string();
    };
    void _write(Nbt::Tag &tag) const override {
        tag << Nbt::gString("structure_block_type", _modestr());
    };
};

#endif // !BLOCKSTATE_HPP
