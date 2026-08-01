#ifndef MCNBT_BE_BREWING_STAND_BED_HPP
#define MCNBT_BE_BREWING_STAND_BED_HPP

#include "common_block_entity_data.hpp"

namespace nbt
{

namespace be
{

template <typename BasicTagType = Tag>
struct BrewingStandBED : public CommonBlockEntityData<BasicTagType>
{
    struct ItemStack
    {
        ItemStack() = default;

        ItemStack(int8_t slot, const Tag& itemData) : slot(slot), itemData(itemData) {}

        Tag getTag() const
        {
            Tag tag = itemData;
            tag["Slot"] = slot;
            return tag;
        }

        int8_t slot = 0;  ///< The slot the item is in.
        Tag    itemData;  ///< The item data. (Compound Tag)
    };

    BrewingStandBED() : CommonBlockEntityData<BasicTagType>("BrewingStand") {}

    BrewingStandBED(int16_t cookTime, int16_t fuelAmount, int16_t fuelTotal, const std::vector<ItemStack>& items)
        : CommonBlockEntityData<BasicTagType>("BrewingStand"),
        cookTime(cookTime),
        fuelAmount(fuelAmount),
        fuelTotal(fuelTotal),
        items(items)
    {}

    int16_t               cookTime   = 0;  ///< The number of ticks until the potions are finished.
    int16_t               fuelAmount = 0;  ///< Remaining fuel for the brewing stand.
    int16_t               fuelTotal  = 0;  ///< The max fuel number for the fuel bar.
    std::vector<ItemStack> items;          ///< The items in the brewing stand.

protected:
    void assemble(BasicTagType& tag) const override
    {
        tag["CookTime"]   = cookTime;
        tag["FuelAmount"] = fuelAmount;
        tag["FuelTotal"]  = fuelTotal;

        if (!items.empty())
        {
            auto itemsTag = BasicTagType::list();
            for (const auto& item : items)
                itemsTag << item.getTag();
            tag["Items"] = std::move(itemsTag);
        }
    }
};

} // namespace be

} // namespace nbt

#endif // !MCNBT_BE_BREWING_STAND_BED_HPP
