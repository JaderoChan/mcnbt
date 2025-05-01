#ifndef MCNBT_BE_COMMON_BLOCK_STATE_DATA_HPP
#define MCNBT_BE_COMMON_BLOCK_STATE_DATA_HPP

#include "../../mcnbt.hpp"

namespace nbt
{

namespace be
{

struct CommonBlockStateData
{
    CommonBlockStateData() = default;

    virtual ~CommonBlockStateData() = default;

    Tag getTag(const String& tagName = "states") const
    {
        Tag tag = gCompound(tagName);
        assemble(tag);
        return tag;
    };

protected:
    virtual void assemble(Tag& tag) const {};
};

} // namespace be

} // namespace nbt

#endif // !MCNBT_BE_COMMON_BLOCK_STATE_DATA_HPP
