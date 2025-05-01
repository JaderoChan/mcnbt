#ifndef MCNBT_BE_COMMAND_BLOCK_BED_HPP
#define MCNBT_BE_COMMAND_BLOCK_BED_HPP

#include "common_block_entity_data.hpp"

namespace nbt
{

namespace be
{

struct CommandBlockBED final : CommonBlockEntityData
{
    CommandBlockBED() : CommonBlockEntityData("CommandBlock") {}

    CommandBlockBED(const String& command, Int32 tickDelay = 0,
                    bool isAuto = false, bool isPowered = true,
                    bool conditionMet = false) :
        CommonBlockEntityData("CommandBlock"), command(command), tickDelay(tickDelay),
        isAuto(isAuto), isPowered(isPowered), conditionMet(conditionMet) {}

    String command;
    /// The last output information of the command.
    String lastOutput;
    /// Whether the command block should execute on the first tick once saved or activated.
    bool executeOnFirstTick = true;
    /// Whether the command block should store the last output.
    bool trackOutput        = true;
    /// Whether the command block is automatically repeating.
    bool isAuto             = false;
    /// Whether the command block is powered by redstone.
    bool isPowered          = true;
    bool conditionMet       = false;
    Byte conditionalMode    = 1;
    Int32 successCount      = 0;
    /// The delay between each execution.
    Int32 tickDelay         = 0;
    /// The data version.
    Int32 version           = 38;
    /// The time when a command block was last executed.
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
        tag << gString(lastOutput, "LastOutput");
        tag << gList(TT_END, "LastOutputParams");
        tag << gInt(successCount, "SuccessCount");
        tag << gInt(tickDelay, "TickDelay");
        tag << gByte(static_cast<Byte>(trackOutput), "TrackOutput");
        tag << gInt(version, "Version");
        tag << gByte(static_cast<Byte>(isAuto), "auto");
        tag << gByte(static_cast<Byte>(conditionMet), "conditionMet");
        tag << gByte(conditionalMode, "conditionalMode");
        tag << gByte(static_cast<Byte>(isPowered), "powered");
    };
};

} // namespace be

} // namespace nbt

#endif // !MCNBT_BE_COMMAND_BLOCK_BED_HPP
