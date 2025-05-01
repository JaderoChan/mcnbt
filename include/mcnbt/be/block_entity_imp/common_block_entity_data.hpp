#ifndef MCNBT_BE_COMMON_BLOCK_ENTITY_DATA_HPP
#define MCNBT_BE_COMMON_BLOCK_ENTITY_DATA_HPP

#include "../../mcnbt.hpp"

namespace nbt
{

namespace be
{

struct CommonBlockEntityData
{
    CommonBlockEntityData() = default;

    CommonBlockEntityData(const String& id, const String& customeName = "") : id(id), customName(customeName) {}

    virtual ~CommonBlockEntityData() = default;

    Tag getTag(const String& tagName = "block_entity_data") const
    {
        Tag tag = gCompound(tagName);

        tag << gString(id, "id");
        if (!customName.empty())
            tag << gString(customName, "CustomName");
        tag << gByte(static_cast<Byte>(isMovable), "isMovable");
        tag << gInt(pos[0], "x") << gInt(pos[1], "y") << gInt(pos[2], "z");
        assemble(tag);

        return tag;
    };

    /// The savegame id of the block entity.
    String id;
    /// The custom name of the block entity.
    String customName;
    Int32 pos[3]    = { 0, 0, 0 };
    /// Wether the block entity is movable with a piston.
    bool isMovable  = true;

protected:
    virtual void assemble(Tag& tag) const {}
};

} // namespace be

} // namespace nbt

#endif // !MCNBT_BE_COMMON_BLOCK_ENTITY_DATA_HPP
