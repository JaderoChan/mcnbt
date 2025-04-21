#ifndef MCNBT_BE_BLOCK_STATE_DATA_HPP
#define MCNBT_BE_BLOCK_STATE_DATA_HPP

#include "../../mcnbt.hpp"

namespace nbt
{

struct BlockStateData
{
    BlockStateData() = default;

    virtual ~BlockStateData() = default;

    Tag getTag() const
    {
        Tag tag = gCompound("states");
        assemble(tag);
        return tag;
    };

protected:
    virtual void assemble(Tag& tag) const = 0;
};

}

#endif // !MCNBT_BE_BLOCK_STATE_DATA_HPP
