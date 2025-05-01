#ifndef MCNBT_BE_BEACON_BED_HPP
#define MCNBT_BE_BEACON_BED_HPP

#include "common_block_entity_data.hpp"
#include "../enums/effect.hpp"

namespace nbt
{

namespace be
{

struct BeaconBED final : public CommonBlockEntityData
{
    BeaconBED() : CommonBlockEntityData("Beacon") {}

    BeaconBED(Int32 primaryEffect, Int32 secondaryEffect) :
        CommonBlockEntityData("Beacon"), primaryEffect(primaryEffect), secondaryEffect(secondaryEffect) {}

    Int32 primaryEffect     = EFFECT_NONE;
    Int32 secondaryEffect   = EFFECT_NONE;

protected:
    void assemble(Tag& tag) const override
    {
        tag << gInt(primaryEffect, "primary");
        tag << gInt(secondaryEffect, "secondary");
    }
};

} // namespace nbt

} // namespace be

#endif // !MCNBT_BE_BEACON_BED_HPP
