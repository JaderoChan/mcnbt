#ifndef MCNBT_BE_BLOCK_ENTITY_DATA_HPP
#define MCNBT_BE_BLOCK_ENTITY_DATA_HPP

#include "../../mcnbt.hpp"

namespace nbt
{

struct BlockEntityData
{
    BlockEntityData() = default;

    BlockEntityData(const String& id, const String& customeName = "") : id(id), customName(customeName) {}

    virtual ~BlockEntityData() = default;

    Tag getTag() const
    {
        Tag tag = gCompound("block_entity_data");
        tag << gString(id, "id");
        tag << gString(customName, "CustomName");
        tag << gByte(static_cast<Byte>(isMovable), "isMovable");
        tag << gInt(pos[0], "x") << gInt(pos[1], "y") << gInt(pos[2], "z");

        assemble(tag);

        return tag;
    };

    /// @brief The savegame id of the block entity.
    String id;
    /// @brief The custom name of the block entity.
    /// @note Maybe not exist (that is empty).
    String customName;
    Int32 pos[3]    = { 0, 0, 0 };
    /// @brief Wether the block entity is movable with a piston.
    bool isMovable  = true;

protected:
    virtual void assemble(Tag& tag) const = 0;
};

}

#endif // !MCNBT_BE_BLOCK_ENTITY_DATA_HPP
