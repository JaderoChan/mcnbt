// Project "mcnbt" library by 頔珞JaderoChan
//
// MIT License
//
// Copyright (c) 2024 頔珞JaderoChan
//
// https://github.com/JaderoChan/mcnbt
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

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

    Nbt::Tag getTag() const {
        Nbt::Tag tag = Nbt::gCompound("block_entity_data");
        tag << Nbt::gString("id", id);
        tag << Nbt::gString("CustomName", customName);
        tag << Nbt::gByte("isMovable", static_cast<char>(isMovable));
        tag << Nbt::gInt("x", pos[0]) << Nbt::gInt("y", pos[1]) << Nbt::gInt("z", pos[2]);
        _write(tag);
        return tag;
    };

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
    void _write(Nbt::Tag &tag) const override {
        tag << Nbt::gString("Command", command);
        tag << Nbt::gByte("ExecuteOnFirstTick", static_cast<char>(executeOnFirstTick));
        tag << Nbt::gInt("LPCommandMode", 0);
        tag << Nbt::gByte("LPCondionalMode", 0);
        tag << Nbt::gByte("LPRedstoneMode", 0);
        tag << Nbt::gLong("LastExecution", lastExecution);
        tag << Nbt::gString("LastOuTut", lastOuTut);
        tag << Nbt::gList("LastOuTuTarams", Nbt::End);
        tag << Nbt::gInt("SuccessCount", successCount);
        tag << Nbt::gInt("TickDelay", tickDelay);
        tag << Nbt::gByte("TrackOuTut", static_cast<char>(trackOuTut));
        tag << Nbt::gInt("Version", version);
        tag << Nbt::gByte("auto", static_cast<char>(isAuto));
        tag << Nbt::gByte("conditionMet", static_cast<char>(conditionMet));
        tag << Nbt::gByte("conditionalMode", conditionalMode);
        tag << Nbt::gByte("powered", static_cast<char>(isPowered));
    };
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
    void _write(Nbt::Tag &tag) const override {
        tag << Nbt::gByte("animationMode", static_cast<char>(animationMode));
        tag << Nbt::gFloat("animationSeconds", animationSeconds);
        tag << Nbt::gInt("data", static_cast<int>(data));
        tag << Nbt::gString("dataField", std::string());
        tag << Nbt::gString("id", id);
        tag << Nbt::gByte("ignoreEntities", static_cast<char>(ignoreEntities));
        tag << Nbt::gByte("includePlayers", 0);
        tag << Nbt::gFloat("integrity", integrity);
        tag << Nbt::gByte("isMovable", static_cast<char>(isMovable));
        tag << Nbt::gByte("isPowered", static_cast<char>(isPowered));
        tag << Nbt::gByte("mirror", static_cast<char>(mirror));
        tag << Nbt::gInt("redstoneSaveMode", static_cast<int>(redstoneSaveMode));
        tag << Nbt::gByte("removeBlcoks", static_cast<char>(removeBlocks));
        tag << Nbt::gByte("rotation", static_cast<char>(rotation));
        tag << Nbt::gByte("seed", static_cast<char>(seed));
        tag << Nbt::gByte("showBoundingBox", static_cast<char>(showBoundingBox));
        tag << Nbt::gString("structureName", structureName);
        tag << Nbt::gInt("xStructureOffset", offset[0]);
        tag << Nbt::gInt("yStructureOffset", offset[1]);
        tag << Nbt::gInt("zStructureOffset", offset[2]);
        tag << Nbt::gInt("xStructureSize", size[0]);
        tag << Nbt::gInt("yStructureSize", size[1]);
        tag << Nbt::gInt("zStructureSize", size[2]);
    };
};

#endif // !BLOCKENTITY_HPP
