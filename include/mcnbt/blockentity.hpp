#ifndef MCNBT_BLOCKENTITY_HPP
#define MCNBT_BLOCKENTITY_HPP

// Only usable for Minecraft bedrock edition.

#include "mcnbt.hpp"

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

struct CommandBlockED final : BlockEntityData
{
    CommandBlockED() : BlockEntityData("CommandBlock") {}

    CommandBlockED(const String& command, Int32 tickDelay = 0,
                   bool isAuto = false, bool isPowered = true,
                   bool conditionMet = false) :
        BlockEntityData("CommandBlock"), command(command), tickDelay(tickDelay),
        isAuto(isAuto), isPowered(isPowered), conditionMet(conditionMet)
    {}

    /// @brief The command entered into the command block.
    String command;
    /// @brief The custom name or hover text of this command block.
    String lastOuTut;
    /// @brief 1 or 0 (true/false)
    /// true if it executes on the first tick once saved or activated.
    bool executeOnFirstTick = true;
    /// @brief 1 or 0 (true/false)
    /// true if the LastOuTut is stored.
    /// Can be toggled in the GUI by clicking a button near the "Previous ouTut" textbox.
    bool trackOuTut         = true;
    /// @brief 1 or 0 (true/false)
    /// Allows to activate the command without the requirement of a redstone signal.
    bool isAuto             = false;
    /// @brief 1 or 0 (true/false)
    /// true if the command block is powered by redstone.
    bool isPowered          = true;
    /// @brief 1 or 0 (true/false)
    /// if a conditional command block had its condition met when last activated.
    /// True if not a conditional command block.
    bool conditionMet       = false;
    Byte conditionalMode    = 1;
    /// @brief Represents the strength of the analog signal ouTut by
    /// redstone comparators attached to this command block.
    Int32 successCount      = 0;
    /// @brief The delay between each execution.
    Int32 tickDelay         = 0;
    /// @brief The data version.
    Int32 version           = 38;
    /// @brief tores the time when a command block was last executed.
    Int64 lastExecution     = 0;

protected:
    void assemble(Tag& tag) const override
    {
        tag << gString(command, "Command");
        tag << gByte(static_cast<Byte>(executeOnFirstTick), "ExecuteOnFirstTick");
        tag << gInt(0, "LPCommandMode");
        tag << gByte(0, "LPCondionalMode");
        tag << gByte(0, "LPRedstoneMode");
        tag << gLong(lastExecution, "LastExecution");
        tag << gString(lastOuTut, "LastOuTut");
        tag << gList(TT_END, "LastOuTuTarams");
        tag << gInt(successCount, "SuccessCount");
        tag << gInt(tickDelay, "TickDelay");
        tag << gByte(static_cast<Byte>(trackOuTut), "TrackOuTut");
        tag << gInt(version, "Version");
        tag << gByte(static_cast<Byte>(isAuto), "auto");
        tag << gByte(static_cast<Byte>(conditionMet), "conditionMet");
        tag << gByte(conditionalMode, "conditionalMode");
        tag << gByte(static_cast<Byte>(isPowered), "powered");
    };
};

struct StructureBlockED final : BlockEntityData
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

    StructureBlockED() : BlockEntityData("StructureBlock") {}

    StructureBlockED(const String& structureName, Mode mode = MODE_LOAD, bool ignoreEntities = false) :
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

} // namespace nbt

#endif // !MCNBT_BLOCKENTITY_HPP
