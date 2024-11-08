#ifndef MCNBT_BLOCKSTATE_HPP
#define MCNBT_BLOCKSTATE_HPP

// Only usable for Minecraft bedrock edition.

#include "mcnbt.hpp"

#include <string>

namespace nbt
{

struct BlockStateData
{
    BlockStateData() {}

    virtual ~BlockStateData() {}

    Tag getTag() const
    {
        Tag tag = gCompound("states");

        write_(tag);

        return tag;
    };

protected:
    virtual void write_(Tag& tag) const = 0;
};

struct CommandBlockSD final : BlockStateData
{
    // Block facing direction.
    enum FacingDirection : uchar
    {
        DOWN,
        UP,
        NORTH,
        SOUTH,
        WEST,
        EAST
    };

    CommandBlockSD(bool isConditional = false, FacingDirection fd = UP) :
        isConditional(isConditional), fd(fd) {}

    bool isConditional;
    FacingDirection fd = UP;

private:
    void write_(Tag& tag) const override
    {
        tag << gByte(static_cast<byte>(isConditional), "conditional_bit");
        tag << gInt(static_cast<int32>(fd), "facing_direction");
    };
};

struct StructureBlockSD final : BlockStateData
{
    enum Mode : uchar
    {
        SAVE,
        LOAD,
        CORNER
    };

    StructureBlockSD(Mode mode = LOAD) : mode(mode) {}

    Mode mode = LOAD;

private:
    std::string modestr_() const
    {
        switch (mode) {
            case SAVE:
                return "save";
            case LOAD:
                return "load";
            case CORNER:
                return "corner";
            default:
                return "";
        }
    };

    void write_(Tag& tag) const override
    {
        tag << gString(modestr_(), "structure_block_type");
    };
};

}

#endif // !MCNBT_BLOCKSTATE_HPP
