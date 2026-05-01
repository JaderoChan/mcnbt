#ifndef MCNBT_BE_COMMAND_BLOCK_BSD_HPP
#define MCNBT_BE_COMMAND_BLOCK_BSD_HPP

#include "common_block_state_data.hpp"

namespace nbt
{

namespace be
{

struct CommandBlockBSD final : CommonBlockStateData
{
    enum FacingDirection : Int32
    {
        FD_DOWN     = 0,
        FD_UP       = 1,
        FD_NORTH    = 2,
        FD_SOUTH    = 3,
        FD_WEST     = 4,
        FD_EAST     = 5
    };

    CommandBlockBSD() = default;

    CommandBlockBSD(bool isConditional, Int32 fd) : isConditional(isConditional), fd(fd) {}

    bool isConditional  = false;
    Int32 fd            = FD_UP;

protected:
    void assemble(Tag& tag) const override
    {
        tag << gByte(static_cast<Byte>(isConditional), "conditional_bit");
        tag << gInt(fd, "facing_direction");
    };
};

} // namespace be

} // namespace nbt

#endif // !MCNBT_BE_COMMAND_BLOCK_BSD_HPP
