#ifndef BLOCKSTATE_HPP
#define BLOCKSTATE_HPP

#include "nbt.hpp"
#include <string>

struct BlockStateData
{
    BlockStateData() {}
    ~BlockStateData() {}
    Nbt::Tag getTag() const;

protected:
    virtual void _write(Nbt::Tag &tag) const = 0;
};

struct CommandBlockSD final : BlockStateData
{
    enum FacingDirection : char
    {
        Up,
        Down,
        North,
        South,
        West,
        East
    };

    CommandBlockSD(bool isConditional = false, FacingDirection fd = Up) :
        isConditional(isConditional), fd(fd) {}

    bool isConditional;
    FacingDirection fd = Up;

private:
    void _write(Nbt::Tag &tag) const override;
};

struct StructureBlockSD final : BlockStateData
{
    enum Mode : char
    {
        Save,
        Load,
        Corner
    };

    StructureBlockSD(Mode mode = Load) : mode(mode) {}

    Mode mode = Load;

private:
    std::string _modestr() const {
        if (mode == Save)
            return std::string("save");
        if (mode == Load)
            return std::string("load");
        if (mode == Corner)
            return std::string("corner");
        return std::string();
    };
    void _write(Nbt::Tag &tag) const override;
};

#endif // !BLOCKSTATE_HPP
