#ifndef BLOCKENTITY_HPP
#define BLOCKENTITY_HPP

#include "nbt.hpp"

#include <string>

#ifndef BLOCKENTITY_MACRO
#define BLOCKENTITY_MACRO

#define BLCOKENTITY_BLOCKID_COMMAND "CommandBlock"
#define BLCOKENTITY_BLOCKID_STRUCTURE "StructureBlock"

#endif // !BLOCKENTITY_MACRO


struct BlockEntityData
{
    BlockEntityData() {}
    BlockEntityData(const std::string &id, const std::string &customeName = std::string()) :
        id(id), customName(customeName) {}
    virtual ~BlockEntityData() {}

    Nbt::Tag getTag() const;

    // The savegame Id of the block entity.
    std::string id;
    // (May not exist) The custom name of the block entity.
    std::string customName;
    int pos[3] = { 0, 0, 0 };
    // 1 or 0 (true/false) - true if the block entity is movable with a piston.
    bool isMovable = true;

protected:
    virtual void _write(Nbt::Tag &tag) const = 0;
};

struct CommandBlockED final : BlockEntityData
{
    CommandBlockED() : BlockEntityData(BLCOKENTITY_BLOCKID_COMMAND) {}
    CommandBlockED(const std::string &command, int tickDelay = 0,
                   bool isAuto = false, bool isPowered = true, bool conditionMet = false) :
        BlockEntityData(BLCOKENTITY_BLOCKID_COMMAND), command(command), tickDelay(0),
        isAuto(isAuto), isPowered(isPowered), conditionMet(conditionMet),
        lastOuTut(std::string()) {}

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
    void _write(Nbt::Tag &tag) const override;
};

struct StructureBlockED final : BlockEntityData
{
    enum Mode : char
    {
        Data,
        Save,
        Load,
        Corner,
        Inventory,
        Export
    };

    enum Mirror : char
    {
        NoMirror,
        X,
        Y,
        XY
    };

    enum Rotation : char
    {
        R0,
        R90,
        R180,
        R270
    };

    enum Animation : char
    {
        NoAnimation,
        ByLayer,
        ByBlock
    };

    enum RedstoneSaveMode : char
    {
        Memory,
        Disk
    };

    StructureBlockED() : BlockEntityData(BLCOKENTITY_BLOCKID_STRUCTURE) {}
    StructureBlockED(const std::string &structureName, Mode mode = Load, bool ignoreEntities = false) :
        BlockEntityData(BLCOKENTITY_BLOCKID_STRUCTURE), structureName(structureName), ignoreEntities(ignoreEntities) {}

    std::string structureName;
    Mode data = Load;
    Animation animationMode = NoAnimation;
    Rotation rotation = R0;
    Mirror mirror = NoMirror;
    RedstoneSaveMode redstoneSaveMode = Memory;
    bool ignoreEntities = false;
    bool removeBlocks = false;
    bool isPowered = true;
    bool showBoundingBox = true;
    long long seed = 0;
    float integrity = 100.;
    float animationSeconds = 0.;
    int offset[3] = { 0, 0, 0 };
    int size[3] = { 1, 1, 1 };

private:
    void _write(Nbt::Tag &tag) const override;
};

#endif // !BLOCKENTITY_HPP
