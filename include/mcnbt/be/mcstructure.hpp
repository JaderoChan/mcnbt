#ifndef MCNBT_BE_MCSTRUCTURE_HPP
#define MCNBT_BE_MCSTRUCTURE_HPP

#include "block_entity.hpp"
#include "block_state.hpp"

namespace nbt
{

namespace be
{

struct MCStructure
{
    MCStructure(int32_t formatVersion = 1, int32_t sizeX = 1, int32_t sizeY = 1, int32_t sizeZ = 1)
        : root(Tag::compound())
    {
        root["format_version"] = formatVersion;

        auto size = Tag::list();
        size << sizeX << sizeY << sizeZ;
        root["size"] = std::move(size);

        auto swo = Tag::list();
        swo << int32_t(0) << int32_t(0) << int32_t(0);
        root["structure_world_origin"] = std::move(swo);

        auto blockIndices = Tag::list();
        blockIndices << Tag::list() << Tag::list();

        auto defaultPalette = Tag::compound();
        defaultPalette["block_palette"]       = Tag::list();
        defaultPalette["block_position_data"] = Tag::compound();

        auto palette = Tag::compound();
        palette["default"] = std::move(defaultPalette);

        auto structure = Tag::compound();
        structure["block_indices"] = std::move(blockIndices);
        structure["entities"]      = Tag::list();
        structure["palette"]       = std::move(palette);

        root["structure"] = std::move(structure);
    }

    Tag& formatVersion()        { return root["format_version"]; }

    Tag& size()                 { return root["size"]; }

    Tag& structureWorldOrigin() { return root["structure_world_origin"]; }

    Tag& blockIndices1()        { return root["structure"]["block_indices"][0]; }

    Tag& blockIndices2()        { return root["structure"]["block_indices"][1]; }

    Tag& entities()             { return root["structure"]["entities"]; }

    Tag& blockPalette()         { return root["structure"]["palette"]["default"]["block_palette"]; }

    Tag& blockPositionData()    { return root["structure"]["palette"]["default"]["block_position_data"]; }

    Tag root;
};

inline Tag createSingleBlockStructure(
    const std::string& blockId,
    const CommonBlockEntityData<>& bed, const CommonBlockStateData<>& bsd,
    int32_t version = 18105860)
{
    MCStructure mcs;
    mcs.blockIndices1() << int32_t(0);
    mcs.blockIndices2() << int32_t(-1);

    auto block = Tag::compound();
    block["name"]    = blockId;
    block["states"]  = bsd.getTag();
    block["version"] = version;
    mcs.blockPalette() << std::move(block);

    auto bpd = Tag::compound();
    bpd["block_entity_data"] = bed.getTag();
    mcs.blockPositionData()["0"] = std::move(bpd);

    return mcs.root;
}

} // namespace be

} // namespace nbt

#endif // !MCNBT_BE_MCSTRUCTURE_HPP
