#ifndef MCNBT_BE_COMMON_BLOCK_ENTITY_DATA_HPP
#define MCNBT_BE_COMMON_BLOCK_ENTITY_DATA_HPP

#include <mcnbt/mcnbt.hpp>

namespace nbt
{

namespace be
{

template <typename BasicTagType = Tag>
struct CommonBlockEntityData
{
    CommonBlockEntityData() = default;

    CommonBlockEntityData(const std::string& id, const std::string& customName = "") : id(id), customName(customName) {}

    virtual ~CommonBlockEntityData() = default;

    BasicTagType getTag(const std::string& = "block_entity_data") const
    {
        BasicTagType tag = BasicTagType::compound();

        tag["id"] = id;
        if (!customName.empty())
            tag["CustomName"] = customName;
        tag["isMovable"] = isMovable;
        tag["x"] = pos[0];
        tag["y"] = pos[1];
        tag["z"] = pos[2];
        assemble(tag);

        return tag;
    };

    /// The savegame id of the block entity.
    std::string id;
    /// The custom name of the block entity.
    std::string customName;
    int32_t pos[3]    = {0, 0, 0};
    /// Wether the block entity is movable with a piston.
    bool    isMovable = true;

protected:
    virtual void assemble(BasicTagType&) const {}
};

} // namespace be

} // namespace nbt

#endif // !MCNBT_BE_COMMON_BLOCK_ENTITY_DATA_HPP
