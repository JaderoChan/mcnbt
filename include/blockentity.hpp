#ifndef BLOCKENTITY_HPP
#define BLOCKENTITY_HPP

#include "nbt.hpp"

#include <string>

namespace nbt
{

struct BlockEntityData
{
    BlockEntityData() {}

    BlockEntityData(const std::string& id, const std::string& customeName = "") :
        id(id), customName(customeName)
    {}

    virtual ~BlockEntityData() {}

    Tag getTag() const
    {
        Tag tag = gCompound("block_entity_data");
        tag << gString("id", id);
        tag << gString("CustomName", customName);
        tag << gByte("isMovable", static_cast<char>(isMovable));
        tag << gInt("x", pos[0]) << gInt("y", pos[1]) << gInt("z", pos[2]);

        write_(tag);
        
        return tag;
    };

    // The savegame Id of the block entity.
    std::string id;
    // (May not exist) The custom name of the block entity.
    std::string customName;
    int pos[3] = {0, 0, 0};
    // 1 or 0 (true/false) - true if the block entity is movable with a piston.
    bool isMovable = true;

protected:
    virtual void write_(Tag& tag) const = 0;
};

struct CommandBlockED final : BlockEntityData
{
    CommandBlockED() : BlockEntityData("CommandBlock") {}

    CommandBlockED(const std::string& command, int tickDelay = 0,
                   bool isAuto = false, bool isPowered = true, bool conditionMet = false) :
        BlockEntityData("CommandBlock"), command(command), tickDelay(0),
        isAuto(isAuto), isPowered(isPowered), conditionMet(conditionMet),
        lastOuTut(std::string())
    {}

// The command entered into the command block.
    std::string command;
    // The custom name or hover text of this command block.
    std::string lastOuTut;
    // 1 or 0 (true/false) - true if it executes on the first tick once saved or activated.
    bool executeOnFirstTick = true;
    // 1 or 0 (true/false) - true if the LastOuTut is stored. Can be toggled in the GUI by clicking a button near the "Previous OuTut" textbox.
    bool trackOuTut = true;
    // 1 or 0 (true/false) - Allows to activate the command without the requirement of a redstone signal.
    bool isAuto = false;
    // 1 or 0 (true/false) - true if the command block is powered by redstone.
    bool isPowered = true;
    // 1 or 0 (true/false) - if a conditional command block had its condition met when last activated. True if not a conditional command block.
    bool conditionMet = false;
    char conditionalMode = 1;
    // Represents the strength of the analog signal ouTut by redstone comparators attached to this command block.
    int successCount = 0;
    // The delay between each execution.
    int tickDelay = 0;
    // The data version.
    int version = 38;
    // tores the time when a command block was last executed.
    long long lastExecution = 0;

private:
    void write_(Tag& tag) const override
    {
        tag << gString("Command", command);
        tag << gByte("ExecuteOnFirstTick", static_cast<char>(executeOnFirstTick));
        tag << gInt("LPCommandMode", 0);
        tag << gByte("LPCondionalMode", 0);
        tag << gByte("LPRedstoneMode", 0);
        tag << gLong("LastExecution", lastExecution);
        tag << gString("LastOuTut", lastOuTut);
        tag << gList("LastOuTuTarams", END);
        tag << gInt("SuccessCount", successCount);
        tag << gInt("TickDelay", tickDelay);
        tag << gByte("TrackOuTut", static_cast<char>(trackOuTut));
        tag << gInt("Version", version);
        tag << gByte("auto", static_cast<char>(isAuto));
        tag << gByte("conditionMet", static_cast<char>(conditionMet));
        tag << gByte("conditionalMode", conditionalMode);
        tag << gByte("powered", static_cast<char>(isPowered));
    };
};

struct StructureBlockED final : BlockEntityData
{
    enum Mode : unsigned char
    {
        DATA,
        SAVE,
        LOAD,
        CORNER,
        INVENTORY,
        EXPORT
    };

    enum Mirror : unsigned char
    {
        NO_MIRROR,
        X,
        Y,
        XY
    };

    enum Rotation : unsigned char
    {
        R0,
        R90,
        R180,
        R270
    };

    enum Animation : unsigned char
    {
        NO_ANIMATION,
        BY_LAYER,
        BY_BLOCK
    };

    enum RedstoneSaveMode : unsigned char
    {
        MEMORY,
        DISK
    };

    StructureBlockED() : BlockEntityData("StructureBlock") {}

    StructureBlockED(const std::string& structureName, Mode mode = LOAD, bool ignoreEntities = false) :
        BlockEntityData("StructureBlock"), structureName(structureName), ignoreEntities(ignoreEntities)
    {}

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
    long long seed = 0;
    float integrity = 100.;
    float animationSeconds = 0.;
    int offset[3] = {0, 0, 0};
    int size[3] = {1, 1, 1};

private:
    void write_(Tag& tag) const override
    {
        tag << gByte("animationMode", static_cast<char>(animationMode));
        tag << gFloat("animationSeconds", animationSeconds);
        tag << gInt("data", static_cast<int>(data));
        tag << gString("dataField", std::string());
        tag << gString("id", id);
        tag << gByte("ignoreEntities", static_cast<char>(ignoreEntities));
        tag << gByte("includePlayers", 0);
        tag << gFloat("integrity", integrity);
        tag << gByte("isMovable", static_cast<char>(isMovable));
        tag << gByte("isPowered", static_cast<char>(isPowered));
        tag << gByte("mirror", static_cast<char>(mirror));
        tag << gInt("redstoneSaveMode", static_cast<int>(redstoneSaveMode));
        tag << gByte("removeBlcoks", static_cast<char>(removeBlocks));
        tag << gByte("rotation", static_cast<char>(rotation));
        tag << gByte("seed", static_cast<char>(seed));
        tag << gByte("showBoundingBox", static_cast<char>(showBoundingBox));
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

#endif // !BLOCKENTITY_HPP
