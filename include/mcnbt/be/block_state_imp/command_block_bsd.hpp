#ifndef MCNBT_BE_COMMAND_BLOCK_BSD_HPP
#define MCNBT_BE_COMMAND_BLOCK_BSD_HPP

#include "common_block_state_data.hpp"

namespace nbt
{

struct CommandBlockBSD final : CommonBlockStateData
{
    /// @brief Block facing direction.
    enum FacingDirection : UChar
    {
        FD_DOWN,
        FD_UP,
        FD_NORTH,
        FD_SOUTH,
        FD_WEST,
        FD_EAST
    };

    CommandBlockBSD(bool isConditional = false, FacingDirection fd = FD_UP) : isConditional(isConditional), fd(fd) {}

    bool isConditional;
    FacingDirection fd = FD_UP;

protected:
    void assemble(Tag& tag) const override
    {
        tag << gByte(static_cast<Byte>(isConditional), "conditional_bit");
        tag << gInt(static_cast<Int32>(fd), "facing_direction");
    };
};

} // namespace nbt

#endif // !MCNBT_BE_COMMAND_BLOCK_BSD_HPP
