#ifndef MCNBT_BE_BEEHIVE_AND_BEE_NEST_BED_HPP
#define MCNBT_BE_BEEHIVE_AND_BEE_NEST_BED_HPP

#include "common_block_entity_data.hpp"

namespace nbt
{

namespace be
{

template <typename BasicTagType = Tag>
struct BeehiveAndBeeNestBED final : public CommonBlockEntityData<BasicTagType>
{
    struct Occupant
    {
        Occupant() = default;

        Occupant(const std::string& actorId, int32_t ticksLeftToStay, const Tag& entityData)
            : actorId(actorId),
            ticksLeftToStay(ticksLeftToStay),
            entityData(entityData)
        {}

        Tag getTag() const
        {
            Tag tag = Tag::compound();
            tag["ActorIdentifier"] = actorId;
            tag["TicksLeftToStay"] = ticksLeftToStay;
            tag["SaveData"]        = entityData;
            return tag;
        }

        std::string actorId;         ///< The ID of the entity in the hive. Always "minecraft:bee" in vanilla game.
        int32_t     ticksLeftToStay = 0;  ///< The time in ticks until the entity leave the hive.
        Tag         entityData;         ///< The NBT data of the entity in the hive. (Compound tag)
    };

    BeehiveAndBeeNestBED() : CommonBlockEntityData<BasicTagType>("Beehive") {}

    BeehiveAndBeeNestBED(bool shouldSpawnBees, const std::vector<Occupant>& occupants)
        : CommonBlockEntityData<BasicTagType>("Beehive"),
        shouldSpawnBees(shouldSpawnBees),
        occupants(occupants)
    {}

    bool                shouldSpawnBees = false;
    std::vector<Occupant> occupants;

protected:
    void assemble(BasicTagType& tag) const override
    {
        tag["ShouldSpawnBees"] = shouldSpawnBees;
        if (!occupants.empty())
        {
            auto occupantsTag = BasicTagType::list();
            for (const auto& occupant : occupants)
                occupantsTag << occupant.getTag();
            tag["Occupants"] = std::move(occupantsTag);
        }
    }
};

} // namespace be

} // namespace nbt

#endif // !MCNBT_BE_BEEHIVE_AND_BEE_NEST_BED_HPP
