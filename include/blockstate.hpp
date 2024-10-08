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
    enum FacingDirection : unsigned char
    {
        UP,
        DOWN,
        NORTH,
        SOUTH,
        WEST,
        EAST
    };

    CommandBlockSD(bool isConditional = false, FacingDirection fd = UP) :
        isConditional(isConditional), fd(fd)
    {}

    bool isConditional;
    FacingDirection fd = UP;

private:
    void write_(Tag& tag) const override
    {
        tag << gByte("conditional_bit", static_cast<char>(isConditional));
        tag << gInt("facing_direction", static_cast<int>(fd));
    };
};

struct StructureBlockSD final : BlockStateData
{
    enum Mode : unsigned char
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
        tag << gString("structure_block_type", modestr_());
    };
};

}

#endif // !BLOCKSTATE_HPP
