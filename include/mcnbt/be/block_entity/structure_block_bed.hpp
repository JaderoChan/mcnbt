#ifndef MCNBT_BE_STRUCTURE_BLOCK_BED_HPP
#define MCNBT_BE_STRUCTURE_BLOCK_BED_HPP

#include "block_entity_data.hpp"

namespace nbt
{

struct StructureBlockBED final : BlockEntityData
{
    enum Mode : UChar
    {
        MODE_DATA,
        MODE_SAVE,
        MODE_LOAD,
        MODE_CORNER,
        MODE_INVENTORY,
        MODE_EXPORT
    };

    enum Mirror : UChar
    {
        MIRROR_NO   = 0x00,
        MIRROR_X    = 0x01,
        MIRROR_Y    = 0x02,
        MIRROR_XY   = 0x03
    };

    enum Rotation : UChar
    {
        ROT_0,
        ROT_90,
        ROT_180,
        ROT_270
    };

    enum Animation : UChar
    {
        ANIMATION_NO,
        ANIMATION_BY_LAYER,
        ANIMATION_BY_BLOCK
    };

    enum RedstoneSaveMode : UChar
    {
        RSM_MEMORY,
        RSM_DISK
    };

    StructureBlockBED() : BlockEntityData("StructureBlock") {}

    StructureBlockBED(const String& structureName, Mode mode = MODE_LOAD, bool ignoreEntities = false) :
        BlockEntityData("StructureBlock"), structureName(structureName), ignoreEntities(ignoreEntities)
    {}

    String structureName;
    Mode data                           = MODE_LOAD;
    Animation animationMode             = ANIMATION_NO;
    Rotation rotation                   = ROT_0;
    Mirror mirror                       = MIRROR_NO;
    RedstoneSaveMode redstoneSaveMode   = RSM_MEMORY;
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
        tag << gByte(static_cast<Byte>(animationMode), "animationMode");
        tag << gFloat(animationSeconds, "animationSeconds");
        tag << gInt(static_cast<Int32>(data), "data");
        tag << gString(String(), "dataField");
        tag << gString(id, "id");
        tag << gByte(static_cast<Byte>(ignoreEntities), "ignoreEntities");
        tag << gByte(0, "includePlayers");
        tag << gFloat(integrity, "integrity");
        tag << gByte(static_cast<Byte>(isMovable), "isMovable");
        tag << gByte(static_cast<Byte>(isPowered), "isPowered");
        tag << gByte(static_cast<Byte>(mirror), "mirror");
        tag << gInt(static_cast<Int32>(redstoneSaveMode), "redstoneSaveMode");
        tag << gByte(static_cast<Byte>(removeBlocks), "removeBlcoks");
        tag << gByte(static_cast<Byte>(rotation), "rotation");
        tag << gByte(static_cast<Byte>(seed), "seed");
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

}

#endif // !MCNBT_BE_STRUCTURE_BLOCK_BED_HPP
