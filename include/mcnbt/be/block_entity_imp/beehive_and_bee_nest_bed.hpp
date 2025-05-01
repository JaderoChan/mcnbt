#ifndef MCNBT_BE_BEEHIVE_AND_BEE_NEST_BED_HPP
#define MCNBT_BE_BEEHIVE_AND_BEE_NEST_BED_HPP

#include "common_block_entity_data.hpp"

namespace nbt
{

namespace be
{

struct BeehiveAndBeeNestBED final : public CommonBlockEntityData
{
    struct Occupant
    {
        Occupant() = default;

        Occupant(const String& actorId, Int32 ticksLeftToStay, const Tag& entityData) :
            actorId(actorId), ticksLeftToStay(ticksLeftToStay), entityData(entityData) {}

        Tag getTag(const String& tagName = "") const
        {
            Tag tag = gCompound(tagName);

            tag << gString(actorId, "ActorIdentifier");
            tag << gInt(ticksLeftToStay, "TicksLeftToStay");
            tag << entityData.copy().setName("SaveData");

            return tag;
        }
        String actorId;         ///< The ID of the entity in the hive. Always "minecraft:bee" in vanilla game.
        Int32 ticksLeftToStay;  ///< The time in ticks until the entity leave the hive.
        Tag entityData;         ///< The NBT data of the entity in the hive. (Commpound tag)
    };

    BeehiveAndBeeNestBED() : CommonBlockEntityData("Beehive") {}

    BeehiveAndBeeNestBED(bool shouldSpawnBees, const Vec<Occupant>& occupants) :
        CommonBlockEntityData("Beehive"), shouldSpawnBees(shouldSpawnBees), occupants(occupants) {}

    bool shouldSpawnBees = false;
    Vec<Occupant> occupants;

protected:
    void assemble(Tag& tag) const override
    {
        tag << gByte(static_cast<Byte>(shouldSpawnBees), "ShouldSpawnBees");
        if (!occupants.empty())
        {
            auto occupantsTag = gList(TT_COMPOUND, "Occupants");
            for (const auto& occupant : occupants)
                occupantsTag << occupant.getTag();
            tag << occupantsTag;
        }
    }
};

} // namespace be

} // namespace nbt

#endif // !MCNBT_BE_BEEHIVE_AND_BEE_NEST_BED_HPP
