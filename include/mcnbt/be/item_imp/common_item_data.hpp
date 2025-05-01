#ifndef MCNBT_BE_ITEM_IMP_COMMON_ITEM_DATA_HPP
#define MCNBT_BE_ITEM_IMP_COMMON_ITEM_DATA_HPP

#include "../../mcnbt.hpp"

namespace nbt
{

namespace be
{

struct CommonItemData
{
    CommonItemData() = default;

    CommonItemData(const String& itemId, Byte count) :
        itemId(itemId), count(count) {}

    virtual ~CommonItemData() = default;

    Tag getTag(const String& tagName = "") const
    {
        Tag tag = gCompound(tagName);

        tag << gString(itemId, "Name");
        tag << gByte(count, "Count");
        tag << gShort(damage, "Damage");
        tag << gByte(static_cast<Byte>(wasPcikedUp), "WasPickedUp");

        if (!block.isEnd())
            tag << (block.copy().setName("Block"));

        if (!this->tag.isEnd())
            tag << (this->tag.copy().setName("tag"));

        if (!canDestroy.empty())
        {
            Tag canDestroyTag = gList(TT_STRING, "CanDestroy");
            for (const String& blockId : canDestroy)
                canDestroyTag << gString(blockId);
            tag << canDestroyTag;
        }

        if (!canPlaceOn.empty())
        {
            Tag canPlaceOnTag = gList(TT_STRING, "CanPlaceOn");
            for (const String& blockId : canPlaceOn)
                canPlaceOnTag << gString(blockId);
            tag << canPlaceOnTag;
        }

        assemble(tag);

        return tag;
    }

    String itemId;
    Byte count          = 64;
    Int16 damage        = 0;
    bool wasPcikedUp    = false;
    Tag block;                      ///< The block form of this item is used when placed. (Maybe not exists)
    Tag tag;                        ///< The additional data of the item. (Maybe not exists)
    Vec<String> canDestroy;         ///< (Maybe not exists)
    Vec<String> canPlaceOn;         ///< (Maybe not exists)

protected:
    virtual void assemble(Tag& tag) const {}
};

} // namespace be

} // namespace nbt

#endif // !MCNBT_BE_ITEM_IMP_COMMON_ITEM_DATA_HPP
