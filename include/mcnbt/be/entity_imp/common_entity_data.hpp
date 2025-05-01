#ifndef MCNBT_BE_ENTITY_IMP_COMMON_ENTITY_DATA_HPP
#define MCNBT_BE_ENTITY_IMP_COMMON_ENTITY_DATA_HPP

#include "../../mcnbt.hpp"

namespace nbt
{

namespace be
{

struct CommonEntityData
{
    CommonEntityData() = default;;

    CommonEntityData(const String& id) : id(id) {}

    virtual ~CommonEntityData() = default;

    Tag getTag(const String& tagName = "") const
    {
        Tag tag = gCompound(tagName);

        tag << gString(id, "identifier");
        tag << gByte(static_cast<Byte>(isAngry), "IsAngry");
        tag << gByte(static_cast<Byte>(isAutonomous), "IsAutonomous");
        tag << gByte(static_cast<Byte>(isBaby), "IsBaby");
        tag << gByte(static_cast<Byte>(isChested), "Chested");
        tag << gByte(static_cast<Byte>(isCustomNameVisible), "CustomNameVisible");
        tag << gByte(static_cast<Byte>(isEating), "IsEating");
        tag << gByte(static_cast<Byte>(isGliding), "IsGliding");
        tag << gByte(static_cast<Byte>(isGlobal), "IsGlobal");
        tag << gByte(static_cast<Byte>(isIllagerCaptain), "IsIllagerCaptain");
        tag << gByte(static_cast<Byte>(isInvulnerable), "Invulnerable");
        tag << gByte(static_cast<Byte>(isLootDropped), "LootDropped");
        tag << gByte(static_cast<Byte>(isOnGround), "OnGround");
        tag << gByte(static_cast<Byte>(isOrphaned), "IsOrphaned");
        tag << gByte(static_cast<Byte>(isOutOfControl), "IsOutOfControl");
        tag << gByte(static_cast<Byte>(isPersistent), "Persistent");
        tag << gByte(static_cast<Byte>(isRoaring), "IsRoaring");
        tag << gByte(static_cast<Byte>(isSaddled), "Saddled");
        tag << gByte(static_cast<Byte>(isScared), "IsScared");
        tag << gByte(static_cast<Byte>(isSheared), "Sheared");
        tag << gByte(static_cast<Byte>(isShowBottom), "ShowBottom");
        tag << gByte(static_cast<Byte>(isSitting), "Sitting");
        tag << gByte(static_cast<Byte>(isStunned), "IsStunned");
        tag << gByte(static_cast<Byte>(isSwimming), "IsSwimming");
        tag << gByte(static_cast<Byte>(isTamed), "IsTamed");
        tag << gByte(static_cast<Byte>(isTrusting), "IsTrusting");
        tag << gByte(color, "Color");
        tag << gByte(color2, "Color2");
        tag << gShort(fireTicks, "Fire");
        tag << gInt(portalCooldown, "PortalCooldown");
        tag << gInt(lastDimensionId, "LastDimensionId");
        tag << gInt(strength, "Strength");
        tag << gInt(strengthMax, "StrengthMax");
        tag << gInt(variant, "Variant");
        tag << gInt(markVariant, "MarkVariant");
        tag << gInt(skinId, "SkinID");
        tag << gLong(uniqueId, "UniqueID");
        tag << gLong(ownerNew, "OwnerNew");
        tag << gFloat(fallDistance, "FallDistance");

        if (!tags.empty())
        {
            Tag tagsTag = gList(TT_STRING, "Tags");
            for (const auto& var : tags)
                tagsTag << gString(var);
            tag << tagsTag;
        }

        if (!definitions.empty())
        {
            Tag definitionsTag = gList(TT_STRING, "definitions");
            for (const auto& var : definitions)
                definitionsTag << gString(var);
            tag << definitionsTag;
        }

        tag << (gList(TT_FLOAT, "Pos") << gFloat(pos[0]) << gFloat(pos[1]) << gFloat(pos[2]));
        tag << (gList(TT_FLOAT, "Rotation") << gFloat(rotation[0]) << gFloat(rotation[1]));
        tag << (gList(TT_FLOAT, "Motion") << gFloat(motion[0]) << gFloat(motion[1]) << gFloat(motion[2]));

        if (!linksTag.isEnd())
            tag << (linksTag.copy().setName("LinksTag"));

        assemble(tag);

        return tag;
    }

    String id;
    bool isAngry                = false;
    bool isAutonomous           = false;
    bool isBaby                 = false;
    bool isChested              = false;
    bool isCustomNameVisible    = true;
    bool isEating               = false;
    bool isGliding              = false;
    bool isGlobal               = false;
    bool isIllagerCaptain       = false;
    bool isInvulnerable         = false;
    bool isLootDropped          = true;
    bool isOnGround             = true;
    bool isOrphaned             = true;
    bool isOutOfControl         = false;
    bool isPersistent           = false;
    bool isRoaring              = false;
    bool isSaddled              = false;
    bool isScared               = false;
    bool isSheared              = false;
    bool isShowBottom           = true;
    bool isSitting              = false;
    bool isStunned              = false;
    bool isSwimming             = false;
    bool isTamed                = false;
    bool isTrusting             = false;
    Byte color                  = 0;
    Byte color2                 = 0;
    Int16 fireTicks             = 0;
    Int32 portalCooldown        = 0;
    Int32 lastDimensionId;
    Int32 strength;
    Int32 strengthMax;
    Int32 variant               = 0;
    Int32 markVariant           = 0;
    Int32 skinId                = 0;
    Int64 uniqueId;
    Int64 ownerNew              = -1;
    Fp32 fallDistance           = 0.0;
    Vec<String> tags;
    Vec<String> definitions;
    Fp32 pos[3]                 = { 0.0, 0.0, 0.0 };
    Fp32 rotation[2]            = { 0.0, 0.0 };
    Fp32 motion[3]              = { 0.0, 0.0 ,0.0 };
    Tag linksTag;

protected:
    virtual void assemble(Tag& tag) const {}
};

} // namespace be

} // namespace nbt

#endif // !MCNBT_BE_ENTITY_IMP_COMMON_ENTITY_DATA_HPP
