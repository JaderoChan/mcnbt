#ifndef MCNBT_BE_BANNER_BED_HPP
#define MCNBT_BE_BANNER_BED_HPP

#include "common_block_entity_data.hpp"
#include <mcnbt/be/enums/color.hpp>

namespace nbt
{

namespace be
{

template <typename BasicTagType = Tag>
struct BannerBED final : public CommonBlockEntityData<BasicTagType>
{
    enum Type : int32_t
    {
        TYPE_NORMAL,
        TYPE_OMINOUS
    };

    struct Pattern
    {
        enum PatternID
        {
            PTRNID_BASE,
            PTRNID_LEFT_STRIPE,
            PTRNID_RIGHT_STRIPE,
            PTRNID_TOP_STRIPE,
            PTRNID_BOTTOM_STRIPE,
            PTRNID_CENTER_STRIPE,       ///< Vertical stripe.
            PTRNID_MIDDLE_STRIPE,       ///< Horizontal stripe.
            PTRNID_DOWN_LEFT_STRIPE,
            PTRNID_DOWN_RIGHT_STRIPE,
            PTRNID_SMALL_STRIPE,        ///< Vertical stripe.
            PTRNID_CROSS,
            PTRNID_SQUARE_CROSS,
            PTRNID_LEFT_OF_DIAGONAL,
            PTRNID_RIGHT_OF_DIAGONAL,
            PTRNID_LEFT_OF_UPSIDE_DOWN_DIAGONAL,
            PTRNID_RIGHT_OF_UPSIDE_DOWN_DIAGONAL,
            PTRNID_LEFT_HALF_VERTICAL,
            PTRNID_RIGHT_HALF_VERTICAL,
            PTRNID_TOP_HALF_HORIZONTAL,
            PTRNID_BOTTOM_HALF_HORIZONTAL,
            PTRNID_TOP_LEFT_CORNER,
            PTRNID_TOP_RIGHT_CORNER,
            PTRNID_BOTTOM_LEFT_CORNER,
            PTRNID_BOTTOM_RIGHT_CORNER,
            PTRNID_TOP_TRIANGLE,
            PTRNID_BOTTOM_TRIANGLE,
            PTRNID_TOP_TRIANGLES_SAWTOOTH,
            PTRNID_BOTTOM_TRIANGLES_SAWTOOTH,
            PTRNID_MIDDLE_CIRCLE,
            PTRNID_MIDDLE_RHOMBUS,
            PTRNID_BORDER,
            PTRNID_CURLY_BORDER,
            PTRNID_BRICK,
            PTRNID_GRADIENT,
            PTRNID_GRADIENT_UPSIDE_DOWN,
            PTRNID_CREEPER,
            PTRNID_SKULL,
            PTRNID_FLOWER,
            PTRNID_MOJANG,
            PTRNID_GLOBE,
            PTRNID_PIGLIN,
            PTRNID_FLOW,
            PTRNID_GUSTER
        };

        Pattern() = default;

        Pattern(const std::string& id, int32_t color) : id(id), color(color) {}

        static std::string patternIdStr(PatternID id)
        {
            switch (id)
            {
                case PTRNID_BASE:                           return "b";
                case PTRNID_LEFT_STRIPE:                    return "ls";
                case PTRNID_RIGHT_STRIPE:                   return "rs";
                case PTRNID_TOP_STRIPE:                     return "ts";
                case PTRNID_BOTTOM_STRIPE:                  return "bs";
                case PTRNID_CENTER_STRIPE:                  return "cs";
                case PTRNID_MIDDLE_STRIPE:                  return "ms";
                case PTRNID_DOWN_LEFT_STRIPE:               return "dls";
                case PTRNID_DOWN_RIGHT_STRIPE:              return "drs";
                case PTRNID_SMALL_STRIPE:                   return "ss";
                case PTRNID_CROSS:                          return "cr";
                case PTRNID_SQUARE_CROSS:                   return "sc";
                case PTRNID_LEFT_OF_DIAGONAL:               return "ld";
                case PTRNID_RIGHT_OF_DIAGONAL:              return "rd";
                case PTRNID_LEFT_OF_UPSIDE_DOWN_DIAGONAL:   return "lud";
                case PTRNID_RIGHT_OF_UPSIDE_DOWN_DIAGONAL:  return "rud";
                case PTRNID_LEFT_HALF_VERTICAL:             return "vh";
                case PTRNID_RIGHT_HALF_VERTICAL:            return "vhr";
                case PTRNID_TOP_HALF_HORIZONTAL:            return "hh";
                case PTRNID_BOTTOM_HALF_HORIZONTAL:         return "hhb";
                case PTRNID_TOP_LEFT_CORNER:                return "tl";
                case PTRNID_TOP_RIGHT_CORNER:               return "tr";
                case PTRNID_BOTTOM_LEFT_CORNER:             return "bl";
                case PTRNID_BOTTOM_RIGHT_CORNER:            return "br";
                case PTRNID_TOP_TRIANGLE:                   return "tt";
                case PTRNID_BOTTOM_TRIANGLE:                return "bt";
                case PTRNID_TOP_TRIANGLES_SAWTOOTH:         return "tts";
                case PTRNID_BOTTOM_TRIANGLES_SAWTOOTH:      return "bts";
                case PTRNID_MIDDLE_CIRCLE:                  return "mc";
                case PTRNID_MIDDLE_RHOMBUS:                 return "mr";
                case PTRNID_BORDER:                         return "bo";
                case PTRNID_CURLY_BORDER:                   return "cbo";
                case PTRNID_BRICK:                          return "bri";
                case PTRNID_GRADIENT:                       return "gra";
                case PTRNID_GRADIENT_UPSIDE_DOWN:           return "gru";
                case PTRNID_CREEPER:                        return "cre";
                case PTRNID_SKULL:                          return "sku";
                case PTRNID_FLOWER:                         return "flo";
                case PTRNID_MOJANG:                         return "moj";
                case PTRNID_GLOBE:                          return "glb";
                case PTRNID_PIGLIN:                         return "pig";
                case PTRNID_FLOW:                           return "flw";
                case PTRNID_GUSTER:                         return "gus";
                default:                                    return "";
            }
        }

        Tag getTag() const
        {
            Tag tag = Tag::compound();
            tag["Color"]   = color;
            tag["Pattern"] = id;
            return tag;
        }

        std::string id    = patternIdStr(PTRNID_BASE);
        int32_t color   = COLOR_WHITE;
    };

    BannerBED() : CommonBlockEntityData<BasicTagType>("Banner") {}

    BannerBED(int32_t baseColor, int32_t type = TYPE_NORMAL, const std::vector<Pattern>& patterns = {})
        : CommonBlockEntityData<BasicTagType>("Banner"),
        baseColor(baseColor),
        type(type),
        patterns(patterns)
    {}

    int32_t              baseColor = COLOR_WHITE;
    int32_t              type      = TYPE_NORMAL;
    /// @sa #BannerBED::Pattern
    std::vector<Pattern> patterns;

protected:
    void assemble(BasicTagType& tag) const override
    {
        tag["Base"] = baseColor;
        tag["Type"] = type;

        if (!patterns.empty())
        {
            auto patternsTag = BasicTagType::list();
            for (const auto& pattern : patterns)
                patternsTag << pattern.getTag();
            tag["Patterns"] = std::move(patternsTag);
        }
    }
};

} // namespace be

} // namespace nbt

#endif // !MCNBT_BE_BANNER_BED_HPP
