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
