#include "blockentity.hpp"

Nbt::Tag BlockEntityData::getTag() const {
    Nbt::Tag tag = Nbt::gCompound("block_entity_data");
    tag << Nbt::gString("id", id);
    tag << Nbt::gString("CustomName", customName);
    tag << Nbt::gByte("isMovable", static_cast<char>(isMovable));
    tag << Nbt::gInt("x", pos[0]) << Nbt::gInt("y", pos[1]) << Nbt::gInt("z", pos[2]);
    _write(tag);
    return tag;
}

void CommandBlockED::_write(Nbt::Tag &tag) const {
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
}

void StructureBlockED::_write(Nbt::Tag &tag) const {
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
}
