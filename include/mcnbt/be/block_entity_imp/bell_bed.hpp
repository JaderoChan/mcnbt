#ifndef MCNBT_BE_BELL_BED_HPP
#define MCNBT_BE_BELL_BED_HPP

#include "common_block_entity_data.hpp"

namespace nbt
{

namespace be
{

struct BellBED : public CommonBlockEntityData
{
    BellBED() : CommonBlockEntityData("Bell") {}

    BellBED(Int32 direction, Int32 ticks, bool isRinging)
        : CommonBlockEntityData("Bell"), direction(direction), ticks(ticks), isRinging(isRinging) {}

    Int32 direction = 0;
    Int32 ticks     = 45;   ///< The time in ticks of the bell's ringing duration.
    bool isRinging  = false;

protected:
    void assemble(Tag& tag) const override
    {
        tag << gInt(direction, "Direction");
        tag << gInt(ticks, "Ticks");
        tag << gByte(static_cast<Byte>(isRinging), "Ringing");
    }
};

} // namespace be

} // namespace nbt

#endif // !MCNBT_BE_BELL_BED_HPP
