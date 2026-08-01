#ifndef MCNBT_BE_STRUCTURE_BLOCK_BSD_HPP
#define MCNBT_BE_STRUCTURE_BLOCK_BSD_HPP

#include "common_block_state_data.hpp"

namespace nbt
{

namespace be
{

template <typename BasicTagType = Tag>
struct StructureBlockBSD final : CommonBlockStateData<BasicTagType>
{
    enum Mode
    {
        MODE_SAVE,
        MODE_LOAD,
        MODE_CORNER
    };

    StructureBlockBSD() = default;

    StructureBlockBSD(const std::string& mode) : mode(mode) {}

    static std::string modeStr(Mode mode)
    {
        switch (mode)
        {
            case MODE_SAVE:     return "save";
            case MODE_LOAD:     return "load";
            case MODE_CORNER:   return "corner";
            default:            return "";
        }
    }

    std::string mode = modeStr(MODE_LOAD);

protected:
    void assemble(BasicTagType& tag) const override { tag["structure_block_type"] = mode; }
};

} // namespace be

} // namespace nbt

#endif // !MCNBT_BE_STRUCTURE_BLOCK_BSD_HPP
