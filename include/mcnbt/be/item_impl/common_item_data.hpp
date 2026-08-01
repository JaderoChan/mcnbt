#ifndef MCNBT_BE_COMMON_ITEM_DATA_HPP
#define MCNBT_BE_COMMON_ITEM_DATA_HPP

#include <mcnbt/mcnbt.hpp>

namespace nbt
{

namespace be
{

template <typename BasicTagType = Tag>
struct CommonItemData
{
    CommonItemData() = default;

    CommonItemData(const std::string& itemId, int8_t count) : itemId(itemId), count(count) {}

    virtual ~CommonItemData() = default;

    BasicTagType getTag(const std::string& = "") const
    {
        BasicTagType result = BasicTagType::compound();

        result["Name"]         = itemId;
        result["Count"]        = count;
        result["Damage"]       = damage;
        result["WasPickedUp"]  = wasPickedUp;

        if (!block.isEnd())
            result["Block"] = block;

        if (!this->tag.isEnd())
            result["tag"] = this->tag;

        if (!canDestroy.empty())
        {
            auto canDestroyTag = BasicTagType::list();
            for (const std::string& blockId : canDestroy)
                canDestroyTag << blockId;
            result["CanDestroy"] = std::move(canDestroyTag);
        }

        if (!canPlaceOn.empty())
        {
            auto canPlaceOnTag = BasicTagType::list();
            for (const std::string& blockId : canPlaceOn)
                canPlaceOnTag << blockId;
            result["CanPlaceOn"] = std::move(canPlaceOnTag);
        }

        assemble(result);

        return result;
    }

    std::string  itemId;
    int8_t       count       = 64;
    int16_t      damage      = 0;
    bool         wasPickedUp = false;
    BasicTagType block;                        ///< The block form of this item is used when placed. (Maybe not exists)
    BasicTagType tag;                          ///< The additional data of the item. (Maybe not exists)
    std::vector<std::string> canDestroy;       ///< (Maybe not exists)
    std::vector<std::string> canPlaceOn;       ///< (Maybe not exists)

protected:
    virtual void assemble(BasicTagType&) const {}
};

} // namespace be

} // namespace nbt

#endif // !MCNBT_BE_COMMON_ITEM_DATA_HPP
