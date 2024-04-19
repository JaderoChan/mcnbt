#include "blockstate.hpp"

Nbt::Tag BlockStateData::getTag() const {
    Nbt::Tag tag = Nbt::gCompound("states");
    _write(tag);
    return tag;
}

void CommandBlockSD::_write(Nbt::Tag &tag) const {
    tag << Nbt::gByte("conditional_bit", static_cast<char>(isConditional));
    tag << Nbt::gInt("facing_direction", static_cast<int>(fd));
}

void StructureBlockSD::_write(Nbt::Tag &tag) const {
    tag << Nbt::gString("structure_block_type", _modestr());
}
