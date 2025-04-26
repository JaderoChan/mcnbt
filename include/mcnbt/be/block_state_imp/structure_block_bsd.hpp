#ifndef MCNBT_BE_STRUCTURE_BLOCK_BSD_HPP
#define MCNBT_BE_STRUCTURE_BLOCK_BSD_HPP

#include "common_block_state_data.hpp"

namespace nbt
{

namespace be
{

struct StructureBlockBSD final : CommonBlockStateData
{
    enum Mode : UChar
    {
        MODE_SAVE,
        MODE_LOAD,
        MODE_CORNER
    };

    StructureBlockBSD(Mode mode = MODE_LOAD) : mode(mode) {}

    Mode mode = MODE_LOAD;

protected:
    void assemble(Tag& tag) const override { tag << gString(modeStr_(mode), "structure_block_type"); };

private:
    static String modeStr_(Mode mode)
    {
        switch (mode)
        {
            case MODE_SAVE:     return "save";
            case MODE_LOAD:     return "load";
            case MODE_CORNER:   return "corner";
            default:            return "";
        }
    };
};

} // namespace be

} // namespace nbt

#endif // !MCNBT_BE_STRUCTURE_BLOCK_BSD_HPP
