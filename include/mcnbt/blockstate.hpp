#ifndef MCNBT_BLOCKSTATE_HPP
#define MCNBT_BLOCKSTATE_HPP

// Only usable for Minecraft bedrock edition.

#include "mcnbt.hpp"

namespace nbt
{

struct BlockStateData
{
    BlockStateData() {}

    virtual ~BlockStateData() {}

    Tag getTag() const
    {
        Tag tag = gCompound("states");

        assemble(tag);

        return tag;
    };

protected:
    virtual void assemble(Tag& tag) const = 0;
};

struct CommandBlockSD final : BlockStateData
{
    /// @brief Block facing direction.
    enum FacingDirection : uchar
    {
        FD_DOWN,
        FD_UP,
        FD_NORTH,
        FD_SOUTH,
        FD_WEST,
        FD_EAST
    };

    CommandBlockSD(bool isConditional = false, FacingDirection fd = FD_UP) : isConditional(isConditional), fd(fd) {}

    bool isConditional;
    FacingDirection fd = FD_UP;

protected:
    void assemble(Tag& tag) const override
    {
        tag << gByte(static_cast<byte>(isConditional), "conditional_bit");
        tag << gInt(static_cast<int32>(fd), "facing_direction");
    };
};

struct StructureBlockSD final : BlockStateData
{
    enum Mode : uchar
    {
        MODE_SAVE,
        MODE_LOAD,
        MODE_CORNER
    };

    StructureBlockSD(Mode mode = MODE_LOAD) : mode(mode) {}

    Mode mode = MODE_LOAD;

protected:
    void assemble(Tag& tag) const override { tag << gString(modestr_(), "structure_block_type"); };

private:
    String modestr_() const
    {
        switch (mode) {
            case MODE_SAVE:
                return "save";
            case MODE_LOAD:
                return "load";
            case MODE_CORNER:
                return "corner";
            default:
                return "";
        }
    };
};

} // namespace nbt

#endif // !MCNBT_BLOCKSTATE_HPP
