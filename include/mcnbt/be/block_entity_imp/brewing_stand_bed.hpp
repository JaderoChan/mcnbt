#ifndef MCNBT_BE_BREWING_STAND_BED_HPP
#define MCNBT_BE_BREWING_STAND_BED_HPP

#include "common_block_entity_data.hpp"

namespace nbt
{

namespace be
{

struct BrewingStandBED : public CommonBlockEntityData
{
    struct ItemStack
    {
        ItemStack() = default;

        ItemStack(const Byte slot, const Tag& itemData) : slot(slot), itemData(itemData) {}

        Tag getTag(const String& tagName = "") const
        {
            Tag tag = itemData.copy();
            tag.setName(tagName);
            tag << gByte(slot, "Slot");
            return tag;
        }

        Byte slot = 0;  ///< The slot the item is in.
        Tag itemData;   ///< The item data. (Compound Tag)
    };

    BrewingStandBED() : CommonBlockEntityData("BrewingStand") {}

    BrewingStandBED(Int16 cookTime, Int16 fuelAmount, Int16 fuelTotal, const Vec<ItemStack>& items) :
        CommonBlockEntityData("BrewingStand"), cookTime(cookTime), fuelAmount(fuelAmount), fuelTotal(fuelTotal),
        items(items) {}

    Int16 cookTime;         ///< The number of ticks until the potions are finished.
    Int16 fuelAmount;       ///< Remaining fuel for the brewing stand.
    Int16 fuelTotal;        ///< The max fuel number for the fuel bar.
    Vec<ItemStack> items;   ///< The items in the brewing stand.

protected:
    void assemble(Tag& tag) const override
    {
        tag << gShort(cookTime, "CookTime");
        tag << gShort(fuelAmount, "FuelAmount");
        tag << gShort(fuelTotal, "FuelTotal");

        if (!items.empty())
        {
            Tag itemsTag = gList(TT_COMPOUND, "Items");
            for (const auto& item : items)
                itemsTag << item.getTag();
            tag << itemsTag;
        }
    }
};

} // namespace be

} // namespace nbt

#endif // !MCNBT_BE_BREWING_STAND_BED_HPP
