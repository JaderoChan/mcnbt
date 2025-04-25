#ifndef MCNBT_BE_COMMAND_BLOCK_BED_HPP
#define MCNBT_BE_COMMAND_BLOCK_BED_HPP

#include "common_block_entity_data.hpp"

namespace nbt
{

struct CommandBlockBED final : CommonBlockEntityData
{
    CommandBlockBED() : CommonBlockEntityData("CommandBlock") {}

    CommandBlockBED(const String& command, Int32 tickDelay = 0,
                   bool isAuto = false, bool isPowered = true,
                   bool conditionMet = false) :
        CommonBlockEntityData("CommandBlock"), command(command), tickDelay(tickDelay),
        isAuto(isAuto), isPowered(isPowered), conditionMet(conditionMet) {}

    /// The command entered into the command block.
    String command;
    /// The custom name or hover text of this command block.
    String lastOuTut;
    /// 1 or 0 (true/false)
    /// true if it executes on the first tick once saved or activated.
    bool executeOnFirstTick = true;
    /// 1 or 0 (true/false)
    /// true if the LastOuTut is stored.
    /// Can be toggled in the GUI by clicking a button near the "Previous ouTut" textbox.
    bool trackOuTut         = true;
    /// 1 or 0 (true/false)
    /// Allows to activate the command without the requirement of a redstone signal.
    bool isAuto             = false;
    /// 1 or 0 (true/false)
    /// true if the command block is powered by redstone.
    bool isPowered          = true;
    /// 1 or 0 (true/false)
    /// if a conditional command block had its condition met when last activated.
    /// True if not a conditional command block.
    bool conditionMet       = false;
    Byte conditionalMode    = 1;
    /// Represents the strength of the analog signal ouTut by
    /// redstone comparators attached to this command block.
    Int32 successCount      = 0;
    /// The delay between each execution.
    Int32 tickDelay         = 0;
    /// The data version.
    Int32 version           = 38;
    /// tores the time when a command block was last executed.
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

} // namespace nbt

#endif // !MCNBT_BE_COMMAND_BLOCK_BED_HPP
