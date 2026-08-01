#ifndef MCNBT_BE_BEACON_BED_HPP
#define MCNBT_BE_BEACON_BED_HPP

#include "common_block_entity_data.hpp"
#include <mcnbt/be/enums/effect.hpp>

namespace nbt
{

namespace be
{

template <typename BasicTagType = Tag>
struct BeaconBED final : public CommonBlockEntityData<BasicTagType>
{
    BeaconBED() : CommonBlockEntityData<BasicTagType>("Beacon") {}

    BeaconBED(int32_t primaryEffect, int32_t secondaryEffect)
        : CommonBlockEntityData<BasicTagType>("Beacon"),
        primaryEffect(primaryEffect),
        secondaryEffect(secondaryEffect)
    {}

    int32_t primaryEffect   = EFFECT_NONE;
    int32_t secondaryEffect = EFFECT_NONE;

protected:
    void assemble(BasicTagType& tag) const override
    {
        tag["primary"]   = primaryEffect;
        tag["secondary"] = secondaryEffect;
    }
};

} // namespace be

} // namespace nbt

#endif // !MCNBT_BE_BEACON_BED_HPP
