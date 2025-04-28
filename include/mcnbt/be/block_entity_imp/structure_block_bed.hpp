#ifndef MCNBT_BE_STRUCTURE_BLOCK_BED_HPP
#define MCNBT_BE_STRUCTURE_BLOCK_BED_HPP

#include "common_block_entity_data.hpp"

namespace nbt
{

namespace be
{

struct StructureBlockBED final : CommonBlockEntityData
{
    enum Mode : Int32
    {
        MODE_DATA,
        MODE_SAVE,
        MODE_LOAD,
        MODE_CORNER,
        MODE_INVENTORY,
        MODE_EXPORT
    };

    enum Mirror : Byte
    {
        MIRROR_NO   = 0x00,
        MIRROR_X    = 0x01,
        MIRROR_Y    = 0x02,
        MIRROR_XY   = 0x03
    };

    enum Rotation : Byte
    {
        ROT_0,
        ROT_90,
        ROT_180,
        ROT_270
    };

    enum Animation : Byte
    {
        ANIMATION_NO,
        ANIMATION_BY_LAYER,
        ANIMATION_BY_BLOCK
    };

    enum RedstoneSaveMode : Int32
    {
        RSM_MEMORY,
        RSM_DISK
    };

    StructureBlockBED() : CommonBlockEntityData("StructureBlock") {}

    StructureBlockBED(const String& structureName, Int32 mode = MODE_LOAD, bool ignoreEntities = false) :
        CommonBlockEntityData("StructureBlock"), structureName(structureName), mode(mode),
        ignoreEntities(ignoreEntities) {}

    String structureName;
    Int32 mode                          = MODE_LOAD;
    Byte animationMode                  = ANIMATION_NO;
    Byte rotation                       = ROT_0;
    Byte mirror                         = MIRROR_NO;
    Int32 redstoneSaveMode              = RSM_MEMORY;
    bool ignoreEntities                 = false;
    bool removeBlocks                   = false;
    bool isPowered                      = true;
    bool showBoundingBox                = true;
    Int64 seed                          = 0;
    Fp32 integrity                      = 100.0;
    Fp32 animationSeconds               = 0.0;
    Int32 offset[3]                     = { 0, 0, 0 };
    Int32 size[3]                       = { 1, 1, 1 };

protected:
    void assemble(Tag& tag) const override
    {
        tag << gByte(animationMode, "animationMode");
        tag << gFloat(animationSeconds, "animationSeconds");
        tag << gInt(mode, "data");
        tag << gString("", "dataField");
        tag << gByte(static_cast<Byte>(ignoreEntities), "ignoreEntities");
        tag << gByte(0, "includePlayers");
        tag << gFloat(integrity, "integrity");
        tag << gByte(static_cast<Byte>(isPowered), "isPowered");
        tag << gByte(mirror, "mirror");
        tag << gInt(redstoneSaveMode, "redstoneSaveMode");
        tag << gByte(static_cast<Byte>(removeBlocks), "removeBlcoks");
        tag << gByte(rotation, "rotation");
        tag << gByte(seed, "seed");
        tag << gByte(static_cast<Byte>(showBoundingBox), "showBoundingBox");
        tag << gString(structureName, "structureName");
        tag << gInt(offset[0], "xStructureOffset");
        tag << gInt(offset[1], "yStructureOffset");
        tag << gInt(offset[2], "zStructureOffset");
        tag << gInt(size[0], "xStructureSize");
        tag << gInt(size[1], "yStructureSize");
        tag << gInt(size[2], "zStructureSize");
    };
};

} // namespace be

} // namespace nbt

#endif // !MCNBT_BE_STRUCTURE_BLOCK_BED_HPP
