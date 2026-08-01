#ifndef MCNBT_BE_COMMAND_BLOCK_BED_HPP
#define MCNBT_BE_COMMAND_BLOCK_BED_HPP

#include "common_block_entity_data.hpp"

namespace nbt
{

namespace be
{

template <typename BasicTagType = Tag>
struct CommandBlockBED final : CommonBlockEntityData<BasicTagType>
{
    CommandBlockBED() : CommonBlockEntityData<BasicTagType>("CommandBlock") {}

    CommandBlockBED(
        const std::string& command, int32_t tickDelay = 0,
        bool isAuto = false, bool isPowered = true,
        bool conditionMet = false)
        : CommonBlockEntityData<BasicTagType>("CommandBlock"),
        command(command), isAuto(isAuto), isPowered(isPowered),
        conditionMet(conditionMet), tickDelay(tickDelay)
    {}

    std::string command;
    /// The last output information of the command.
    std::string lastOutput;
    /// Whether the command block should execute on the first tick once saved or activated.
    bool    executeOnFirstTick = true;
    /// Whether the command block should store the last output.
    bool    trackOutput        = true;
    /// Whether the command block is automatically repeating.
    bool    isAuto             = false;
    /// Whether the command block is powered by redstone.
    bool    isPowered          = true;
    bool    conditionMet       = false;
    int8_t  conditionalMode    = 1;
    int32_t successCount       = 0;
    /// The delay between each execution.
    int32_t tickDelay          = 0;
    /// The data version.
    int32_t version            = 38;
    /// The time when a command block was last executed.
    int64_t lastExecution      = 0;

protected:
    void assemble(BasicTagType& tag) const override
    {
        tag["Command"]            = command;
        tag["ExecuteOnFirstTick"] = executeOnFirstTick;
        tag["LPCommandMode"]      = int32_t(0);
        tag["LPCondionalMode"]    = int8_t(0);
        tag["LPRedstoneMode"]     = int8_t(0);
        tag["LastExecution"]      = lastExecution;
        tag["LastOutput"]         = lastOutput;
        tag["LastOutputParams"]   = BasicTagType::list();
        tag["SuccessCount"]       = successCount;
        tag["TickDelay"]          = tickDelay;
        tag["TrackOutput"]        = trackOutput;
        tag["Version"]            = version;
        tag["auto"]               = isAuto;
        tag["conditionMet"]       = conditionMet;
        tag["conditionalMode"]    = conditionalMode;
        tag["powered"]            = isPowered;
    }
};

} // namespace be

} // namespace nbt

#endif // !MCNBT_BE_COMMAND_BLOCK_BED_HPP
