#ifndef MCNBT_BE_COMMON_BLOCK_STATE_DATA_HPP
#define MCNBT_BE_COMMON_BLOCK_STATE_DATA_HPP

#include <mcnbt/mcnbt.hpp>

namespace nbt
{

namespace be
{

template <typename BasicTagType = Tag>
struct CommonBlockStateData
{
    CommonBlockStateData() = default;

    virtual ~CommonBlockStateData() = default;

    BasicTagType getTag(const std::string& = "states") const
    {
        BasicTagType tag = BasicTagType::compound();
        assemble(tag);
        return tag;
    }

protected:
    virtual void assemble(BasicTagType&) const {}
};

} // namespace be

} // namespace nbt

#endif // !MCNBT_BE_COMMON_BLOCK_STATE_DATA_HPP
