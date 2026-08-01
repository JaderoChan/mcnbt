#ifndef MCNBT_BE_COMMON_ENTITY_DATA_HPP
#define MCNBT_BE_COMMON_ENTITY_DATA_HPP

#include <mcnbt/mcnbt.hpp>

namespace nbt
{

namespace be
{

template <typename BasicTagType = Tag>
struct CommonEntityData
{
    CommonEntityData() = default;

    CommonEntityData(const std::string& id) : id(id) {}

    virtual ~CommonEntityData() = default;

    BasicTagType getTag(const std::string& = "") const
    {
        BasicTagType tag = BasicTagType::compound();

        tag["identifier"]        = id;
        tag["IsAngry"]           = isAngry;
        tag["IsAutonomous"]      = isAutonomous;
        tag["IsBaby"]            = isBaby;
        tag["Chested"]           = isChested;
        tag["CustomNameVisible"] = isCustomNameVisible;
        tag["IsEating"]          = isEating;
        tag["IsGliding"]         = isGliding;
        tag["IsGlobal"]          = isGlobal;
        tag["IsIllagerCaptain"]  = isIllagerCaptain;
        tag["Invulnerable"]      = isInvulnerable;
        tag["LootDropped"]       = isLootDropped;
        tag["OnGround"]          = isOnGround;
        tag["IsOrphaned"]        = isOrphaned;
        tag["IsOutOfControl"]    = isOutOfControl;
        tag["Persistent"]        = isPersistent;
        tag["IsRoaring"]         = isRoaring;
        tag["Saddled"]           = isSaddled;
        tag["IsScared"]          = isScared;
        tag["Sheared"]           = isSheared;
        tag["ShowBottom"]        = isShowBottom;
        tag["Sitting"]           = isSitting;
        tag["IsStunned"]         = isStunned;
        tag["IsSwimming"]        = isSwimming;
        tag["IsTamed"]           = isTamed;
        tag["IsTrusting"]        = isTrusting;
        tag["Color"]             = color;
        tag["Color2"]            = color2;
        tag["Fire"]              = fireTicks;
        tag["PortalCooldown"]    = portalCooldown;
        tag["LastDimensionId"]   = lastDimensionId;
        tag["Strength"]          = strength;
        tag["StrengthMax"]       = strengthMax;
        tag["Variant"]           = variant;
        tag["MarkVariant"]       = markVariant;
        tag["SkinID"]            = skinId;
        tag["UniqueID"]          = uniqueId;
        tag["OwnerNew"]          = ownerNew;
        tag["FallDistance"]      = fallDistance;

        if (!tags.empty())
        {
            auto tagsTag = BasicTagType::list();
            for (const auto& var : tags)
                tagsTag << var;
            tag["Tags"] = std::move(tagsTag);
        }

        if (!definitions.empty())
        {
            auto definitionsTag = BasicTagType::list();
            for (const auto& var : definitions)
                definitionsTag << var;
            tag["definitions"] = std::move(definitionsTag);
        }

        auto posTag = BasicTagType::list();
        posTag << pos[0] << pos[1] << pos[2];
        tag["Pos"] = std::move(posTag);

        auto rotTag = BasicTagType::list();
        rotTag << rotation[0] << rotation[1];
        tag["Rotation"] = std::move(rotTag);

        auto motionTag = BasicTagType::list();
        motionTag << motion[0] << motion[1] << motion[2];
        tag["Motion"] = std::move(motionTag);

        if (!linksTag.isEnd())
            tag["LinksTag"] = linksTag;

        assemble(tag);

        return tag;
    }

    std::string id;
    bool    isAngry             = false;
    bool    isAutonomous        = false;
    bool    isBaby              = false;
    bool    isChested           = false;
    bool    isCustomNameVisible = true;
    bool    isEating            = false;
    bool    isGliding           = false;
    bool    isGlobal            = false;
    bool    isIllagerCaptain    = false;
    bool    isInvulnerable      = false;
    bool    isLootDropped       = true;
    bool    isOnGround          = true;
    bool    isOrphaned          = true;
    bool    isOutOfControl      = false;
    bool    isPersistent        = false;
    bool    isRoaring           = false;
    bool    isSaddled           = false;
    bool    isScared            = false;
    bool    isSheared           = false;
    bool    isShowBottom        = true;
    bool    isSitting           = false;
    bool    isStunned           = false;
    bool    isSwimming          = false;
    bool    isTamed             = false;
    bool    isTrusting          = false;
    int8_t  color               = 0;
    int8_t  color2              = 0;
    int16_t fireTicks           = 0;
    int32_t portalCooldown      = 0;
    int32_t lastDimensionId     = 0;
    int32_t strength            = 0;
    int32_t strengthMax         = 0;
    int32_t variant             = 0;
    int32_t markVariant         = 0;
    int32_t skinId              = 0;
    int64_t uniqueId            = 0;
    int64_t ownerNew            = -1;
    float   fallDistance        = 0.0f;
    std::vector<std::string> tags;
    std::vector<std::string> definitions;
    float pos[3]      = {0.0f, 0.0f, 0.0f};
    float rotation[2] = {0.0f, 0.0f};
    float motion[3]   = {0.0f, 0.0f, 0.0f};
    BasicTagType linksTag;

protected:
    virtual void assemble(BasicTagType&) const {}
};

} // namespace be

} // namespace nbt

#endif // !MCNBT_BE_COMMON_ENTITY_DATA_HPP
