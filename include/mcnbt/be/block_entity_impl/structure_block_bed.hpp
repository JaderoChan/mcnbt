#ifndef MCNBT_BE_STRUCTURE_BLOCK_BED_HPP
#define MCNBT_BE_STRUCTURE_BLOCK_BED_HPP

#include "common_block_entity_data.hpp"

namespace nbt
{

namespace be
{

template <typename BasicTagType = Tag>
struct StructureBlockBED final : CommonBlockEntityData<BasicTagType>
{
    enum Mode : int32_t
    {
        MODE_DATA,
        MODE_SAVE,
        MODE_LOAD,
        MODE_CORNER,
        MODE_INVENTORY,
        MODE_EXPORT
    };

    enum Mirror : uint8_t
    {
        MIRROR_NO   = 0x00,
        MIRROR_X    = 0x01,
        MIRROR_Y    = 0x02,
        MIRROR_XY   = 0x03
    };

    enum Rotation : uint8_t
    {
        ROT_0,
        ROT_90,
        ROT_180,
        ROT_270
    };

    enum Animation : uint8_t
    {
        ANIMATION_NO,
        ANIMATION_BY_LAYER,
        ANIMATION_BY_BLOCK
    };

    enum RedstoneSaveMode : int32_t
    {
        RSM_MEMORY,
        RSM_DISK
    };


    StructureBlockBED() : CommonBlockEntityData<BasicTagType>("StructureBlock") {}

    StructureBlockBED(const std::string& structureName, int32_t mode = MODE_LOAD, bool ignoreEntities = false)
        : CommonBlockEntityData<BasicTagType>("StructureBlock"),
        structureName(structureName),
        mode(mode),
        ignoreEntities(ignoreEntities)
    {}

    std::string structureName;
    int32_t mode             = MODE_LOAD;
    uint8_t animationMode    = ANIMATION_NO;
    uint8_t rotation         = ROT_0;
    uint8_t mirror           = MIRROR_NO;
    int32_t redstoneSaveMode = RSM_MEMORY;
    bool    ignoreEntities   = false;
    bool    removeBlocks     = false;
    bool    isPowered        = true;
    bool    showBoundingBox  = true;
    int64_t seed             = 0;
    float   integrity        = 100.0;
    float   animationSeconds = 0.0;
    int32_t offset[3]        = {0, 0, 0};
    int32_t size[3]          = {1, 1, 1};

protected:
    void assemble(BasicTagType& tag) const override
    {
        tag["animationMode"]    = int8_t(animationMode);
        tag["animationSeconds"] = animationSeconds;
        tag["data"]             = mode;
        tag["dataField"]        = "";
        tag["ignoreEntities"]   = ignoreEntities;
        tag["includePlayers"]   = int8_t(0);
        tag["integrity"]        = integrity;
        tag["isPowered"]        = isPowered;
        tag["mirror"]           = int8_t(mirror);
        tag["redstoneSaveMode"] = redstoneSaveMode;
        tag["removeBlocks"]     = removeBlocks;
        tag["rotation"]         = int8_t(rotation);
        tag["seed"]             = seed;
        tag["showBoundingBox"]  = showBoundingBox;
        tag["structureName"]    = structureName;
        tag["xStructureOffset"] = offset[0];
        tag["yStructureOffset"] = offset[1];
        tag["zStructureOffset"] = offset[2];
        tag["xStructureSize"]   = size[0];
        tag["yStructureSize"]   = size[1];
        tag["zStructureSize"]   = size[2];
    }
};

} // namespace be

} // namespace nbt

#endif // !MCNBT_BE_STRUCTURE_BLOCK_BED_HPP
