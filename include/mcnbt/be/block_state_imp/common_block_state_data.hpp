#ifndef MCNBT_BE_COMMON_BLOCK_STATE_DATA_HPP
#define MCNBT_BE_COMMON_BLOCK_STATE_DATA_HPP

#include "../../mcnbt.hpp"

namespace nbt
{

struct CommonBlockStateData
{
    CommonBlockStateData() = default;

    virtual ~CommonBlockStateData() = default;

    Tag getTag() const
    {
        Tag tag = gCompound("states");
        assemble(tag);
        return tag;
    };

protected:
    virtual void assemble(Tag& tag) const {};
};

} // namespace nbt

#endif // !MCNBT_BE_COMMON_BLOCK_STATE_DATA_HPP
