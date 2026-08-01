#ifndef MCNBT_BE_BELL_BED_HPP
#define MCNBT_BE_BELL_BED_HPP

#include "common_block_entity_data.hpp"

namespace nbt
{

namespace be
{

template <typename BasicTagType = Tag>
struct BellBED : public CommonBlockEntityData<BasicTagType>
{
    BellBED() : CommonBlockEntityData<BasicTagType>("Bell") {}

    BellBED(int32_t direction, int32_t ticks, bool isRinging)
        : CommonBlockEntityData<BasicTagType>("Bell"),
        direction(direction),
        ticks(ticks),
        isRinging(isRinging)
    {}

    int32_t direction = 0;
    int32_t ticks     = 45;   ///< The time in ticks of the bell's ringing duration.
    bool    isRinging = false;

protected:
    void assemble(BasicTagType& tag) const override
    {
        tag["Direction"] = direction;
        tag["Ticks"]     = ticks;
        tag["Ringing"]   = isRinging;
    }
};

} // namespace be

} // namespace nbt

#endif // !MCNBT_BE_BELL_BED_HPP
