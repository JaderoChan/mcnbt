#ifndef MCNBT_BE_COMMAND_BLOCK_BSD_HPP
#define MCNBT_BE_COMMAND_BLOCK_BSD_HPP

#include "common_block_state_data.hpp"

namespace nbt
{

namespace be
{

template <typename BasicTagType = Tag>
struct CommandBlockBSD final : CommonBlockStateData<BasicTagType>
{
    enum FacingDirection : int32_t
    {
        FD_DOWN     = 0,
        FD_UP       = 1,
        FD_NORTH    = 2,
        FD_SOUTH    = 3,
        FD_WEST     = 4,
        FD_EAST     = 5
    };

    CommandBlockBSD() = default;

    CommandBlockBSD(bool isConditional, int32_t fd) : isConditional(isConditional), fd(fd) {}

    bool    isConditional = false;
    int32_t fd            = FD_UP;

protected:
    void assemble(BasicTagType& tag) const override
    {
        tag["conditional_bit"]  = isConditional;
        tag["facing_direction"] = fd;
    }
};

} // namespace be

} // namespace nbt

#endif // !MCNBT_BE_COMMAND_BLOCK_BSD_HPP
