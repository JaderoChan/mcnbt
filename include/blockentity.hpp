#ifndef MCNBT_BLOCKENTITY_HPP
#define MCNBT_BLOCKENTITY_HPP

#include "mcnbt.hpp"

#include <string>

namespace nbt
{

struct BlockEntityData
{
    BlockEntityData() {}

    BlockEntityData(const std::string& id, const std::string& customeName = "") :
        id(id), customName(customeName) {}

    virtual ~BlockEntityData() {}

    Tag getTag() const
    {
        Tag tag = gCompound("block_entity_data");
        tag << gString("id", id);
        tag << gString("CustomName", customName);
        tag << gByte("isMovable", static_cast<byte>(isMovable));
        tag << gInt("x", pos[0]) << gInt("y", pos[1]) << gInt("z", pos[2]);

        write_(tag);

        return tag;
    };

    // The savegame Id of the block entity.
    std::string id;
    // (May not exist) The custom name of the block entity.
    std::string customName;
    int32 pos[3] = { 0, 0, 0 };
    // 1 or 0 (true/false)
    // true if the block entity is movable with a piston.
    bool isMovable = true;

protected:
    virtual void write_(Tag& tag) const = 0;
};

struct CommandBlockED final : BlockEntityData
{
    CommandBlockED() : BlockEntityData("CommandBlock") {}

    CommandBlockED(const std::string& command, int32 tickDelay = 0,
                   bool isAuto = false, bool isPowered = true, bool conditionMet = false) :
        BlockEntityData("CommandBlock"), command(command), tickDelay(0),
        isAuto(isAuto), isPowered(isPowered), conditionMet(conditionMet),
        lastOuTut(std::string()) {}

    // The command entered into the command block.
    std::string command;
    // The custom name or hover text of this command block.
    std::string lastOuTut;
    // 1 or 0 (true/false)
    // true if it executes on the first tick once saved or activated.
    bool executeOnFirstTick = true;
    // 1 or 0 (true/false)
    // true if the LastOuTut is stored.
    // Can be toggled in the GUI by clicking a button near the "Previous ouTut" textbox.
    bool trackOuTut = true;
    // 1 or 0 (true/false)
    // Allows to activate the command without the requirement of a redstone signal.
    bool isAuto = false;
    // 1 or 0 (true/false)
    // true if the command block is powered by redstone.
    bool isPowered = true;
    // 1 or 0 (true/false)
    // if a conditional command block had its condition met when last activated.
    // True if not a conditional command block.
    bool conditionMet = false;
    byte conditionalMode = 1;
    // Represents the strength of the analog signal ouTut by
    // redstone comparators attached to this command block.
    int32 successCount = 0;
    // The delay between each execution.
    int32 tickDelay = 0;
    // The data version.
    int32 version = 38;
    // tores the time when a command block was last executed.
    int64 lastExecution = 0;

private:
    void write_(Tag& tag) const override
    {
        tag << gString("Command", command);
        tag << gByte("ExecuteOnFirstTick", static_cast<byte>(executeOnFirstTick));
        tag << gInt("LPCommandMode", 0);
        tag << gByte("LPCondionalMode", 0);
        tag << gByte("LPRedstoneMode", 0);
        tag << gLong("LastExecution", lastExecution);
        tag << gString("LastOuTut", lastOuTut);
        tag << gList("LastOuTuTarams", END);
        tag << gInt("SuccessCount", successCount);
        tag << gInt("TickDelay", tickDelay);
        tag << gByte("TrackOuTut", static_cast<byte>(trackOuTut));
        tag << gInt("Version", version);
        tag << gByte("auto", static_cast<byte>(isAuto));
        tag << gByte("conditionMet", static_cast<byte>(conditionMet));
        tag << gByte("conditionalMode", conditionalMode);
        tag << gByte("powered", static_cast<byte>(isPowered));
    };
};

struct StructureBlockED final : BlockEntityData
{
    enum Mode : uchar
    {
        DATA,
        SAVE,
        LOAD,
        CORNER,
        INVENTORY,
        EXPORT
    };

    enum Mirror : uchar
    {
        NO_MIRROR = 0x00,
        X = 0x01,
        Y = 0x02,
        XY = 0x03
    };

    enum Rotation : uchar
    {
        R0,
        R90,
        R180,
        R270
    };

    enum Animation : uchar
    {
        NO_ANIMATION,
        BY_LAYER,
        BY_BLOCK
    };

    enum RedstoneSaveMode : uchar
    {
        MEMORY,
        DISK
    };

    StructureBlockED() : BlockEntityData("StructureBlock") {}

    StructureBlockED(const std::string& structureName, Mode mode = LOAD, bool ignoreEntities = false) :
        BlockEntityData("StructureBlock"),
        structureName(structureName),
        ignoreEntities(ignoreEntities) {}

    std::string structureName;
    Mode data = LOAD;
    Animation animationMode = NO_ANIMATION;
    Rotation rotation = R0;
    Mirror mirror = NO_MIRROR;
    RedstoneSaveMode redstoneSaveMode = MEMORY;
    bool ignoreEntities = false;
    bool removeBlocks = false;
    bool isPowered = true;
    bool showBoundingBox = true;
    int64 seed = 0;
    fp32 integrity = 100.;
    fp32 animationSeconds = 0.;
    int32 offset[3] = { 0, 0, 0 };
    int32 size[3] = { 1, 1, 1 };

private:
    void write_(Tag& tag) const override
    {
        tag << gByte("animationMode", static_cast<byte>(animationMode));
        tag << gFloat("animationSeconds", animationSeconds);
        tag << gInt("data", static_cast<int32>(data));
        tag << gString("dataField", std::string());
        tag << gString("id", id);
        tag << gByte("ignoreEntities", static_cast<byte>(ignoreEntities));
        tag << gByte("includePlayers", 0);
        tag << gFloat("integrity", integrity);
        tag << gByte("isMovable", static_cast<byte>(isMovable));
        tag << gByte("isPowered", static_cast<byte>(isPowered));
        tag << gByte("mirror", static_cast<byte>(mirror));
        tag << gInt("redstoneSaveMode", static_cast<int32>(redstoneSaveMode));
        tag << gByte("removeBlcoks", static_cast<byte>(removeBlocks));
        tag << gByte("rotation", static_cast<byte>(rotation));
        tag << gByte("seed", static_cast<byte>(seed));
        tag << gByte("showBoundingBox", static_cast<byte>(showBoundingBox));
        tag << gString("structureName", structureName);
        tag << gInt("xStructureOffset", offset[0]);
        tag << gInt("yStructureOffset", offset[1]);
        tag << gInt("zStructureOffset", offset[2]);
        tag << gInt("xStructureSize", size[0]);
        tag << gInt("yStructureSize", size[1]);
        tag << gInt("zStructureSize", size[2]);
    };
};

}

#endif // !MCNBT_BLOCKENTITY_HPP
