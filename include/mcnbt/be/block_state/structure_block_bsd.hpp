#ifndef MCNBT_BE_STRUCTURE_BLOCK_BSD_HPP
#define MCNBT_BE_STRUCTURE_BLOCK_BSD_HPP

#include "block_state_data.hpp"

namespace nbt
{

struct StructureBlockBSD final : BlockStateData
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
    void assemble(Tag& tag) const override { tag << gString(modestr_(), "structure_block_type"); };

private:
    String modestr_() const
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

}

#endif // !MCNBT_BE_STRUCTURE_BLOCK_BSD_HPP
