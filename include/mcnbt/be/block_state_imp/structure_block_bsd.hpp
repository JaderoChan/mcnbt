#ifndef MCNBT_BE_STRUCTURE_BLOCK_BSD_HPP
#define MCNBT_BE_STRUCTURE_BLOCK_BSD_HPP

#include "common_block_state_data.hpp"

namespace nbt
{

namespace be
{

struct StructureBlockBSD final : CommonBlockStateData
{
    enum Mode
    {
        MODE_SAVE,
        MODE_LOAD,
        MODE_CORNER
    };

    StructureBlockBSD() = default;

    StructureBlockBSD(const String& mode) : mode(mode) {}

    static String modeStr(Mode mode)
    {
        switch (mode)
        {
            case MODE_SAVE:     return "save";
            case MODE_LOAD:     return "load";
            case MODE_CORNER:   return "corner";
            default:            return "";
        }
    };

    String mode = modeStr(MODE_LOAD);

protected:
    void assemble(Tag& tag) const override { tag << gString(mode, "structure_block_type"); };
};

} // namespace be

} // namespace nbt

#endif // !MCNBT_BE_STRUCTURE_BLOCK_BSD_HPP
