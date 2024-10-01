#ifndef BLOCKSTATE_HPP
#define BLOCKSTATE_HPP

#include "nbt.hpp"

#include <string>

namespace nbt
{

struct BlockStateData
{
    BlockStateData() {}

    virtual ~BlockStateData() {}

    Tag getTag() const {
        Tag tag = gCompound("states");
        write_(tag);
        return tag;
    };

protected:
    virtual void write_(Tag &tag) const = 0;
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
    void write_(Tag &tag) const override {
        tag << gByte("conditional_bit", static_cast<char>(isConditional));
        tag << gInt("facing_direction", static_cast<int>(fd));
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
    std::string modestr_() const {
        if (mode == Save)
            return std::string("save");
        if (mode == Load)
            return std::string("load");
        if (mode == Corner)
            return std::string("corner");
        return std::string();
    };

    void write_(Tag &tag) const override {
        tag << gString("structure_block_type", modestr_());
    };
};

}

#endif // !BLOCKSTATE_HPP
