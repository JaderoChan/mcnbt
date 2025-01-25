// The "mcnbt" library, in c++.
//
// Webs: https://github.com/JaderoChan/mcnbt
// You can contact me by email: c_dl_cn@outlook.com

// MIT License
//
// Copyright (c) 2024 頔珞JaderoChan
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

// Note:
// Move the tag (not copy) is default when add tag to list or compound. (use #copy() function if need)

#ifndef MCNBT_HPP
#define MCNBT_HPP

// Whether to use GZip to un/compress MCNBT.
// #define MCNBT_USE_GZIP

// Whether to disable throw logic exception check (for performance).
// #define MCNBT_DISABLE_LOGIC_EXCEPTION

#include <cstdint>          // int16_t, int32_t, int64_t
#include <cstddef>          // size_t
#include <cstring>          // strlen(), memcpy()
#include <string>           // string, to_string()
#include <vector>           // vector
#include <unordered_map>    // unordered_map
#include <iostream>         // istream, ostream
#include <fstream>          // ifstream, ofstream
#include <sstream>          // stringstream
#include <stdexcept>        // runtime_error, logic_error, out_of_range
#include <cassert>          // assert()

#ifdef MCNBT_USE_GZIP
#include "gzip.hpp"
#endif // MCNBT_USE_GZIP

// McNbt namespace.
namespace nbt
{

// Just for the intellisense better show "tip about namespace". :)

}

// Type alias.
namespace nbt
{

using uchar     = unsigned char;
using byte      = char;
using int16     = int16_t;
using int32     = int32_t;
using int64     = int64_t;
using fp32      = float;
using fp64      = double;

using String    = std::string;

using SStream   = std::stringstream;

using IStream   = std::istream;
using OStream   = std::ostream;
using IOStream  = std::iostream;

using IFStream  = std::ifstream;
using OFStream  = std::ofstream;
using FStream   = std::fstream;

template <typename T>
using Vec       = std::vector<T>;

template <typename K, typename V>
using Map       = std::unordered_map<K, V>;

} // namespace nbt

// Enum, constants and aux functions.
namespace nbt
{

// Enum of NBT tag type.
enum TagType : uchar
{
    TT_END          = 0,
    TT_BYTE         = 1,
    TT_SHORT        = 2,
    TT_INT          = 3,
    TT_LONG         = 4,
    TT_FLOAT        = 5,
    TT_DOUBLE       = 6,
    TT_BYTE_ARRAY   = 7,
    TT_STRING       = 8,
    TT_LIST         = 9,
    TT_COMPOUND     = 10,
    TT_INT_ARRAY    = 11,
    TT_LONG_ARRAY   = 12
};

/*
* Constants about the indent of snbt.
*/

constexpr size_t _SNBT_INDENT_WIDTH     = 2;
constexpr char _SNBT_INDENT_CHAR        = 0x20;
static const String _SNBT_INDENT_STR    = String(_SNBT_INDENT_WIDTH, _SNBT_INDENT_CHAR);

/*
* Aux functions about the tag type.
*/

inline bool isEnd(TagType type)
{
    return type == TT_END;
}

inline bool isByte(TagType type)
{
    return type == TT_BYTE;
}

inline bool isShort(TagType type)
{
    return type == TT_SHORT;
}

inline bool isInt(TagType type)
{
    return type == TT_INT;
}

inline bool isLong(TagType type)
{
    return type == TT_LONG;
}

inline bool isFloat(TagType type)
{
    return type == TT_FLOAT;
}

inline bool isDouble(TagType type)
{
    return type == TT_DOUBLE;
}

inline bool isString(TagType type)
{
    return type == TT_STRING;
}

inline bool isByteArray(TagType type)
{
    return type == TT_BYTE_ARRAY;
}

inline bool isIntArray(TagType type)
{
    return type == TT_INT_ARRAY;
}

inline bool isLongArray(TagType type)
{
    return type == TT_LONG_ARRAY;
}

inline bool isList(TagType type)
{
    return type == TT_LIST;
}

inline bool isCompound(TagType type)
{
    return type == TT_COMPOUND;
}

inline bool isInteger(TagType type)
{
    return isByte(type) || isShort(type) || isInt(type) || isLong(type);
}

inline bool isFloatPoint(TagType type)
{
    return isFloat(type) || isDouble(type);
}

inline bool isNum(TagType type)
{
    return isInteger(type) || isFloatPoint(type);
}

inline bool isArray(TagType type)
{
    return isByteArray(type) || isIntArray(type) || isLongArray(type);
}

inline bool isContainer(TagType type)
{
    return isList(type) || isCompound(type);
}

/// @brief Get the string text of the tag type.
inline String getTagTypeString(TagType type)
{
    switch (type) {
        case TT_END:            return "End";
        case TT_BYTE:           return "Byte";
        case TT_SHORT:          return "Short";
        case TT_INT:            return "Int";
        case TT_LONG:           return "Long";
        case TT_FLOAT:          return "Float";
        case TT_DOUBLE:         return "Double";
        case TT_BYTE_ARRAY:     return "Byte Array";
        case TT_STRING:         return "String";
        case TT_LIST:           return "List";
        case TT_COMPOUND:       return "Compound";
        case TT_INT_ARRAY:      return "Int Array";
        case TT_LONG_ARRAY:     return "Long Array";
        default:                return "";
    }
}

} // namespace nbt

// Functions of read and write binary data.
namespace nbt
{

/// @brief Reverse a C style string.
/// @param size The size of need reversed range, and if it is 0 reverse all bytes until \0 char.
/// @note The #srcStr can be equal to the #dstStr.
inline void _reverse(char* srcStr, char* dstStr, size_t size = 0)
{
    if (size == 0)
        size = std::strlen(srcStr);

    size_t i = 0;
    while (i < size / 2)
    {
        char ch = srcStr[i];
        dstStr[i] = srcStr[size - 1 - i];
        dstStr[size - 1 - i] = ch;

        i++;
    }
}

/// @brief Check whether the memory order of system of compiler environment is big endian.
inline bool _isBigEndian()
{
    static bool isInited = false;
    static bool rslt = false;

    // If already checked, return the result.
    if (isInited)
        return rslt;

    int num = 1;
    rslt = reinterpret_cast<char*>(&num)[0] == 0 ? true : false;
    isInited = true;

    return rslt;
}

/// @brief Obtain bytes from input stream, and convert it to number.
/// @param is               The input stream.
/// @param isBigEndian      The endianness of the bytes.
/// @param resumeCursor     Whether to resume the cursor position of input stream after read.
/// @return A number.
template <typename T>
T _bytes2num(IStream& is, bool isBigEndian = false, bool resumeCursor = false)
{
    size_t size = sizeof(T);
    T result = T();

    // Store the begin position of read data used to resume cursor.
    auto begpos = is.tellg();

    // Read the bytes from input stream.
    static byte buffer[sizeof(T)] = {};
    is.read(buffer, size);

    size = static_cast<size_t>(is.gcount());

    // Reverse the bytes if the specified endianness is different from system's endianness.
    if (isBigEndian != _isBigEndian())
        _reverse(buffer, buffer, size);

    // Convert the bytes to number. (reinterpreting memory bytes)
    std::memcpy(&result, buffer, size);

    // Resume the cursor position of input stream if needed.
    if (resumeCursor)
        is.seekg(begpos);

    return result;
}

/// @brief Convert the number to bytes, and write it to output stream.
/// @param num              The number to convert.
/// @param os               The output stream.
/// @param isBigEndian      The endianness of the bytes need to write.
template <typename T>
void _num2bytes(T num, OStream& os, bool isBigEndian = false)
{
    size_t size = sizeof(T);

    static byte buffer[sizeof(T)] = {};

    // Convert the number to bytes. (reinterpreting memory bytes)
    std::memcpy(buffer, &num, size);

    // Reverse the bytes if the specified endianness is different from system's endianness.
    if (isBigEndian != _isBigEndian())
        _reverse(buffer, buffer, size);

    os.write(buffer, size);
}

} // namespace nbt

// Main.
namespace nbt
{

class Tag
{
public:
    Tag() = default;

    /// @note Release all alloced memory and set the parent to nullptr.
    ~Tag()
    {
        release_();
        parent_ = nullptr;
    }

    /// @note Deep copy but not copy the other's parent.
    Tag(const Tag& other) :
        tagType_(other.tagType_), itemType_(other.itemType_)
    {
        if (other.isNum())                                      tagData_.num = other.tagData_.num;
        else if (other.isString() && other.tagData_.str)        tagData_.str = new String(*other.tagData_.str);
        else if (other.isArray() && other.tagData_.bad)         tagData_.bad = new Vec<byte>(*other.tagData_.bad);
        else if (other.isIntArray() && other.tagData_.iad)      tagData_.iad = new Vec<int32>(*other.tagData_.iad);
        else if (other.isLongArray() && other.tagData_.lad)     tagData_.lad = new Vec<int64>(*other.tagData_.lad);
        else if (other.isList() && other.tagData_.ld)
        {
            tagData_.ld = new Vec<Tag>();
            tagData_.ld->reserve(other.tagData_.ld->size());

            for (const auto& var : *other.tagData_.ld)
            {
                tagData_.ld->emplace_back(var);
                tagData_.ld->back().parent_ = this;
            }
        }
        else if (other.isCompound() && other.tagData_.cd)
        {
            tagData_.cd = new CompoundData();
            tagData_.cd->reserve(other.tagData_.cd->size());

            tagData_.cd->idxs = other.tagData_.cd->idxs;
            for (const auto& var : other.tagData_.cd->data)
            {
                tagData_.cd->data.emplace_back(var);
                tagData_.cd->data.back().parent_ = this;
            }
        }

        if (other.tagName_ && !other.tagName_->empty())
            tagName_ = new String(*other.tagName_);
    }

    /// @note Copy the other's parent.
    Tag(Tag&& other) noexcept :
        tagType_(other.tagType_), itemType_(other.itemType_), tagData_(other.tagData_), tagName_(other.tagName_)
    {
        if (isList() && tagData_.ld)
        {
            for (auto& var : *tagData_.ld)
                var.parent_ = this;
        }
        else if (isCompound() && tagData_.cd)
        {
            for (auto& var : tagData_.cd->data)
                var.parent_ = this;
        }

        other.tagData_.str  = nullptr;
        other.tagName_      = nullptr;
    }

    /// @note Deep copy but not copy the other's parent.
    Tag& operator=(const Tag& other)
    {
        assert(!(isListItem() && (type() != other.type())));
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
        if (isListItem() && (type() != other.type()))
            throw std::logic_error("Can't assign a tag of incorrect tag type to list element.");
#endif

        if (this == &other)
            return *this;

        release_();

        tagType_ = other.tagType_;
        itemType_ = other.itemType_;

        if (other.isNum())                                      tagData_.num = other.tagData_.num;
        else if (other.isString() && other.tagData_.str)        tagData_.str = new String(*other.tagData_.str);
        else if (other.isArray() && other.tagData_.bad)         tagData_.bad = new Vec<byte>(*other.tagData_.bad);
        else if (other.isIntArray() && other.tagData_.iad)      tagData_.iad = new Vec<int32>(*other.tagData_.iad);
        else if (other.isLongArray() && other.tagData_.lad)     tagData_.lad = new Vec<int64>(*other.tagData_.lad);
        else if (other.isList() && other.tagData_.ld)
        {
            tagData_.ld = new Vec<Tag>();
            tagData_.ld->reserve(other.tagData_.ld->size());

            for (const auto& var : *other.tagData_.ld)
            {
                tagData_.ld->emplace_back(var);
                tagData_.ld->back().parent_ = this;
            }
        }
        else if (other.isCompound() && other.tagData_.cd)
        {
            tagData_.cd = new CompoundData();
            tagData_.cd->reserve(other.tagData_.cd->size());

            tagData_.cd->idxs = other.tagData_.cd->idxs;
            for (const auto& var : other.tagData_.cd->data)
            {
                tagData_.cd->data.emplace_back(var);
                tagData_.cd->data.back().parent_ = this;
            }
        }

        if (!isListItem() && !other.name().empty())
            tagName_ = new String(other.name());

        return *this;
    }

    /// @note Copy the other's parent.
    Tag& operator=(Tag&& other)
    {
        assert(!isContained(other));
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
        if (isContained(other))
            throw std::logic_error("Can't assign parent to self.");
#endif

        assert(!(isListItem() && (type() != other.type())));
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
        if (isListItem() && (type() != other.type()))
            throw std::logic_error("Can't assign a tag of incorrect tag type to list element");
#endif

        if (this == &other)
            return *this;

        release_();

        tagType_    = other.tagType_;
        itemType_   = other.itemType_;
        tagData_    = other.tagData_;
        tagName_    = other.tagName_;

        if (isList() && tagData_.ld)
        {
            for (auto& var : *tagData_.ld)
                var.parent_ = this;
        }
        else if (isCompound() && tagData_.cd)
        {
            for (auto& var : tagData_.cd->data)
                var.parent_ = this;
        }

        other.tagData_.str  = nullptr;
        other.tagName_      = nullptr;

        if (isListItem() && tagName_)
        {
            delete tagName_;
            tagName_ = nullptr;
        }

        return *this;
    }

    explicit Tag(TagType type) : tagType_(type) {}

    /// @brief Get the tag from binary input stream.
    /// @param is               The input stream.
    /// @param isBigEndian      Whether the read data from input stream with big endian.
    /// @param headerSize       The size of need discard data from input stream begin.
    // (usually is 0, but bedrock edition map file is 8, some useless dat)
    static Tag fromBinStream(IFStream& is, bool isBigEndian, size_t headerSize = 0)
    {
#ifdef MCNBT_USE_GZIP
        SStream buf;
        buf << is.rdbuf();
        String content = buf.str();
        buf.clear();

        SStream ss;
        if (gzip::isCompressed(content))
            content = gzip::decompress(content);

        ss << content;
        content.clear();

        if (headerSize != 0)
            ss.seekg(headerSize, ss.cur);

        return fromBinStream_(ss, isBigEndian, false);
#else
        if (headerSize != 0)
            is.seekg(headerSize, is.cur);

        return fromBinStream_(is, isBigEndian, false);
#endif // MCNBT_USE_GZIP
    }

    /// @brief Get the tag from a nbt file.
    static Tag fromFile(const String& filename, bool isBigEndian, size_t headerSize = 0)
    {
        IFStream ifs(filename, std::ios::binary);

        if (!ifs.is_open())
            throw std::runtime_error("Failed to open file: " + filename);

        Tag rslt = fromBinStream(ifs, isBigEndian, headerSize);

        ifs.close();

        return rslt;
    }

    /// @todo
    /// @brief Get the tag from a text input stream.
    /// @note The root tag must be a compound tag.
    /// @note The tag name (key of key-value) must valid, it can't contains {, }, [,] and so on key characters.
    static Tag fromSnbt(IStream& is);

    /// @brief Get the tag from a string.
    /// @note The root tag must be a compound tag.
    /// @note The tag name (key of key-value) must valid, it can't contains {, }, [,] and so on key characters.
    static Tag fromSnbt(const String& snbt)
    {
        SStream ss;
        ss << snbt;

        return fromSnbt(ss);
    }

   /// @brief Functions of check tag type.

    bool isEnd() const          { return nbt::isEnd(tagType_); }

    bool isByte() const         { return nbt::isByte(tagType_); }

    bool isShort() const        { return nbt::isShort(tagType_); }

    bool isInt() const          { return nbt::isInt(tagType_); }

    bool isLong() const         { return nbt::isLong(tagType_); }

    bool isFloat() const        { return nbt::isFloat(tagType_); }

    bool isDouble() const       { return nbt::isDouble(tagType_); }

    bool isString() const       { return nbt::isString(tagType_); }

    bool isByteArray() const    { return nbt::isByteArray(tagType_); }

    bool isIntArray() const     { return nbt::isIntArray(tagType_); }

    bool isLongArray() const    { return nbt::isLongArray(tagType_); }

    bool isList() const         { return nbt::isList(tagType_); }

    bool isCompound() const     { return nbt::isCompound(tagType_); }

    bool isInteger() const      { return nbt::isInteger(tagType_); }

    bool isFloatPoint() const   { return nbt::isFloatPoint(tagType_); }

    bool isNum() const          { return nbt::isNum(tagType_); }

    bool isArray() const        { return nbt::isArray(tagType_); }

    bool isContainer() const    { return nbt::isContainer(tagType_); }

    /// @brief Functions of common to all tag.

    /// @brief Make a copy.
    // Usually used for add tag to list or compound. (because default is move when add tag to list or compound)
    Tag copy() const            { return Tag(*this); }

    /// @brief Assign a tag to self, be equal to *this = tag;
    Tag& assign(const Tag& tag) { *this = tag; return *this; }

    /// @brief Get the tag type.
    TagType type() const        { return tagType_; }

    /// @brief Get the name of tag.
    String name() const         { return tagName_ ? *tagName_ : ""; }

    /// @brief Get the name length of tag.
    int16 nameLength() const    { return static_cast<int16>(name().size()); }

    /// @brief Set the name of tag.
    /// @note If the new name already exist in parent, cover it.
    /// @attention Only be called via #non-ListItem.
    Tag& setName(const String& name)
    {
        assert(!isListItem());
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
        if (isListItem())
            throw std::logic_error("Can't set name for list element.");
#endif

        // If the new name is empty and the old name is empty (maybe not exists), do nothing.
        if (name.empty() && this->name().empty())
            return *this;

        // If the new name is equal to the old name, do nothing. 
        String oldname = this->name();
        if (name == oldname)
            return *this;

        // Why need some step above? see below.

        if (!parent_)
        {
            if (tagName_)       *tagName_ = name;
            else                tagName_ = new String(name);

            return *this;
        }
        else
        {
            Tag* p = parent_;
            if (p->hasTag(name))
                p->remove(name);

            Tag& t = (*p)[oldname];

            if (t.tagName_)     *t.tagName_ = name;
            else                t.tagName_ = new String(name);

            size_t idx = p->tagData_.cd->idxs[oldname];

            p->tagData_.cd->idxs.erase(oldname);
            p->tagData_.cd->idxs.insert({ name, idx });

            return t;
        }
    }

    /// @brief Check if is a list element.
    bool isListItem() const     { return parent_ && parent_->isList(); }

    /// @brief Check if the parent is exists.
    bool hasParent() const      { return parent_ != nullptr; }

    /// @return The parent pointer.
    const Tag* parent() const   { return parent_; }

    /// @brief Check if is be contained in specified tag.
    /// @param container The tag that be checked whether self is contained in it.
    /// @note Recursive check all the parent (parent's parent) until a parent is nullptr.
    bool isContained(const Tag& container) const
    {
        const Tag* p = parent_;

        while (p)
        {
            if (p == &container)
                return true;

            p = p->parent_;
        }

        return false;
    }

    /// @brief Functions about the list tag.

    /// @brief Get the list item type.
    /// @attention Only be called via #TT_LIST.
    bool listItemType() const
    {
        assert(isList());
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
        if (!isList())
            throw std::logic_error("Can't get the list item type for non-list tag.");
#endif

        return itemType_;
    }

    /// @brief Check if the list item type is be seted.
    /// @attention Only be called via #TT_LIST.
    bool hasSetListItemType() const
    {
        return listItemType() != TT_END;
    }

    /// @brief Set the tag type of list item.
    /// @note If the list item type already be seted, cover the original list item type and remove all original items.
    /// @attention Only be called via #TT_LIST.
    Tag& setListItemType(TagType type)
    {
        assert(isList());
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
        if (!isList())
            throw std::logic_error("Can't set the list item type for non-list tag.");
#endif

        if (itemType_ != TT_END)
        {
            if (tagData_.ld)
            {
                delete tagData_.ld;
                tagData_.ld = nullptr;
            }
        }

        itemType_ = type;

        return *this;
    }

    /// @attention Only be called via #TT_LIST.
    Tag& assign(size_t size, const Tag& tag)
    {
        assert(isList());
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
        if (!isList())
            throw std::logic_error("Can't assign multiple tags to non-list tag.");

        if (itemType_ == TT_END)
            throw std::logic_error("Can't read or write a uninitialized list.");

        if (tag.type() != itemType_)
        {
            String errmsg = "Can't assign the tag of " + getTagTypeString(tag.type());
            errmsg += " to the list of " + getTagTypeString(itemType_);

            throw std::logic_error(errmsg);
        }
#endif

        if (size == 0 && !tagData_.ld)
            return *this;

        if (!tagData_.ld)
            tagData_.ld = new Vec<Tag>();

        tagData_.ld->assign(size, tag);

        return *this;
    }

    /// @brief Functions about the compound tag.

    /// @brief Check if the compound contains member of specified name.
    /// @attention Only be called via #TT_COMPOUND.
    bool hasTag(const String& name) const
    {
        assert(isCompound());
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
        if (!isCompound())
            throw std::logic_error("Can't check tag existence for non-compound tag.");
#endif

        if (!tagData_.cd)
            return false;

        return tagData_.cd->idxs.find(name) != tagData_.cd->idxs.end();
    }

    /// @brief Functions about the tag of containers.

    /// @brief Get the length of string or size of array or tag counts of list and compound.
    /// @attention Only be called via
    // #TT_STRING, #TT_BYTE_ARRAY, #TT_INT_ARRAY, #TT_LONG_ARRAY, #TT_LIST, #TT_COMPOUND.
    size_t size() const
    {
        assert(isString() || isArray() || isContainer());
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
        if (!isString() && !isArray() && !isContainer())
            throw std::logic_error("Can't get size for non-string, non-array, non-container tag.");
#endif

        if (isString())     return !tagData_.str ? 0 : tagData_.str->size();
        if (isByteArray())  return !tagData_.bad ? 0 : tagData_.bad->size();
        if (isIntArray())   return !tagData_.iad ? 0 : tagData_.iad->size();
        if (isLongArray())  return !tagData_.lad ? 0 : tagData_.lad->size();
        if (isList())       return !tagData_.ld ? 0 : tagData_.ld->size();
        if (isCompound())   return !tagData_.cd ? 0 : tagData_.cd->size();

        return 0;
    }

    /// @brief Check if the string or array or list or compound is empty.
    /// @attention Only be called via
    // #TT_STRING, #TT_BYTE_ARRAY, #TT_INT_ARRAY, #TT_LONG_ARRAY, #TT_LIST, #TT_COMPOUND.
    bool empty() const { return size() == 0; }

    /// @brief Reserve the space of string or array or list or compound.
    /// @attention Only be called via
    // #TT_STRING, #TT_BYTE_ARRAY, #TT_INT_ARRAY, #TT_LONG_ARRAY, #TT_LIST, #TT_COMPOUND.
    void reserve(size_t size)
    {
        assert(isString() || isArray() || isContainer());
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
        if (!isString() && !isArray() && !isContainer())
            throw std::logic_error("Can't reserve space for non-string, non-array, non-container tag.");
#endif

        if (isString())
        {
            if (!tagData_.str)
                tagData_.str = new String();

            tagData_.str->reserve(size);
        }
        else if (isByteArray())
        {
            if (!tagData_.bad)
                tagData_.bad = new Vec<byte>();

            tagData_.bad->reserve(size);
        }
        else if (isIntArray())
        {
            if (!tagData_.iad)
                tagData_.iad = new Vec<int32>();

            tagData_.iad->reserve(size);
        }
        else if (isLongArray())
        {
            if (!tagData_.lad)
                tagData_.lad = new Vec<int64>();

            tagData_.lad->reserve(size);
        }
        else if (isList())
        {
            if (!tagData_.ld)
                tagData_.ld = new Vec<Tag>();

            tagData_.ld->reserve(size);
        }
        else if (isCompound())
        {
            if (!tagData_.cd)
                tagData_.cd = new CompoundData();

            tagData_.cd->reserve(size);
        }
    }

    /// @brief Functions for set value. Only be called via corresponding tag.

    /// @attention Only be called via #TT_BYTE.
    Tag& setByte(byte value)
    {
        assert(isByte());
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
        if (!isByte())
            throw std::logic_error("Can't set byte value for non-byte tag.");
#endif

        tagData_.num.i8 = value;

        return *this;
    }

    /// @attention Only be called via #TT_SHORT.
    Tag& setShort(int16 value)
    {
        assert(isShort());
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
        if (!isShort())
            throw std::logic_error("Can't set short value for non-short tag.");
#endif

        tagData_.num.i16 = value;

        return *this;
    }

    /// @attention Only be called via #TT_INT.
    Tag& setInt(int32 value)
    {
        assert(isInt());
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
        if (!isInt())
            throw std::logic_error("Can't set int value for non-int tag.");
#endif

        tagData_.num.i32 = value;

        return *this;
    }

    /// @attention Only be called via #TT_LONG.
    Tag& setLong(int64 value)
    {
        assert(isLong());
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
        if (!isLong())
            throw std::logic_error("Can't set long value for non-long tag.");
#endif

        tagData_.num.i64 = value;

        return *this;
    }

    /// @attention Only be called via #TT_FLOAT.
    Tag& setFloat(fp32 value)
    {
        assert(isFloat());
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
        if (!isFloat())
            throw std::logic_error("Can't set float value for non-float tag.");
#endif

        tagData_.num.f32 = value;

        return *this;
    }

    /// @attention Only be called via #TT_DOUBLE.
    Tag& setDouble(fp64 value)
    {
        assert(isDouble());
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
        if (!isDouble())
            throw std::logic_error("Can't set double value for non-double tag.");
#endif

        tagData_.num.f64 = value;

        return *this;
    }

    /// @brief Fast way of set the integer value.
    /// @attention Only be called via #TT_BYTE, #TT_SHORT, #TT_INT, #TT_LONG.
    Tag& setInteger(int64 value)
    {
        assert(isInteger());
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
        if (!isInteger())
            throw std::logic_error("Can't set interger number for non-integer tag.");
#endif

        if (isByte())           setByte(static_cast<byte>(value));
        else if (isShort())     setShort(static_cast<int16>(value));
        else if (isInt())       setInt(static_cast<int32>(value));
        else if (isLong())      setLong(value);

        return *this;
    }

    /// @brief Fast way of set the float point value.
    /// @attention Only be called via #TT_FLOAT, #TT_DOUBLE.
    Tag& setFloatPoint(fp64 value)
    {
        assert(isFloatPoint());
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
        if (!isFloatPoint())
            throw std::logic_error("Can't set float point number value for non-float point tag.");
#endif

        if (isFloat())          setFloat(static_cast<fp32>(value));
        else if (isDouble())    setDouble(value);

        return *this;
    }

    /// @attention Only be called via #TT_STRING.
    Tag& setString(const String& value)
    {
        assert(isString());
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
        if (!isString())
            throw std::logic_error("Can't set string value for non-string tag.");
#endif

        if (value.empty() && !tagData_.str)
            return *this;

        if (tagData_.str)   *tagData_.str = value;
        else                tagData_.str = new String(value);

        return *this;
    }

    /// @attention Only be called via #TT_BYTE_ARRAY.
    Tag& setByteArray(const Vec<byte>& value)
    {
        assert(isByteArray());
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
        if (!isByteArray())
            throw std::logic_error("Can't set byte array value for non-byte array tag.");
#endif

        if (value.empty() && !tagData_.bad)
            return *this;

        if (tagData_.bad)   *tagData_.bad = value;
        else                tagData_.bad = new Vec<byte>(value);

        return *this;
    }

    /// @attention Only be called via #TT_INT_ARRAY.
    Tag& setIntArray(const Vec<int32>& value)
    {
        assert(isIntArray());
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
        if (!isIntArray())
            throw std::logic_error("Can't set int array value for non-int array tag.");
#endif

        if (value.empty() && !tagData_.iad)
            return *this;

        if (tagData_.iad)   *tagData_.iad = value;
        else                tagData_.iad = new Vec<int32>(value);

        return *this;
    }

    /// @attention Only be called via #TT_LONG_ARRAY.
    Tag& setLongArray(const Vec<int64>& value)
    {
        assert(isLongArray());
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
        if (!isLongArray())
            throw std::logic_error("Can't set long array value for non-long array tag.");
#endif

        if (value.empty() && !tagData_.lad)
            return *this;

        if (tagData_.lad)   *tagData_.lad = value;
        else                tagData_.lad = new Vec<int64>(value);

        return *this;
    }

    /// @attention Only be called via #TT_BYTE_ARRAY.
    Tag& setArray(const Vec<byte>& value) { return setByteArray(value); }

    /// @overload
    /// @attention Only be called via #TT_INT_ARRAY.
    Tag& setArray(const Vec<int32>& value) { return setIntArray(value); }

    /// @overload
    /// @attention Only be called via #TT_LONG_ARRAY.
    Tag& setArray(const Vec<int64>& value) { return setLongArray(value); }

    /// @brief Add a value to the byte array.
    /// @attention Only be called via #TT_BYTE_ARRAY.
    Tag& addByte(byte value)
    {
        assert(isByteArray());
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
        if (!isByteArray())
            throw std::logic_error("Can't add byte value to non-byte array tag.");
#endif

        if (!tagData_.bad)
            tagData_.bad = new Vec<byte>();

        tagData_.bad->emplace_back(value);

        return *this;
    }

    /// @brief Add a value to the int array.
    /// @attention Only be called via #TT_INT_ARRAY.
    Tag& addInt(int32 value)
    {
        assert(isIntArray());
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
        if (!isIntArray())
            throw std::logic_error("Can't add int value to non-int array tag.");
#endif

        if (!tagData_.iad)
            tagData_.iad = new Vec<int32>();

        tagData_.iad->emplace_back(value);

        return *this;
    }

    /// @brief Add a value to the long array.
    /// @attention Only be called via #TT_LONG_ARRAY.
    Tag& addLong(int64 value)
    {
        assert(isLongArray());
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
        if (!isLongArray())
            throw std::logic_error("Can't add long value to non-long array tag.");
#endif

        if (!tagData_.lad)
            tagData_.lad = new Vec<int64>();

        tagData_.lad->emplace_back(value);

        return *this;
    }

    /// @brief Add a tag to the list or compound.
    /// @note Original tag will be moved to the new tag whether left-value or right-value reference,
    // and the original tag will invalid after this operation, but you can call #copy() function to avoid this.
    /// @attention Only be called via #TT_LIST, #TT_COMPOUND.
    Tag& addTag(Tag&& tag)
    {
        assert(isContainer());
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
        if (!isContainer())
            throw std::logic_error("Can't add tag to non-container tag.");
#endif

        assert(this != &tag);
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
        if (this == &tag)
            throw std::logic_error("Can't add self to self.");
#endif

        assert(!isContained(tag));
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
        if (isContained(tag))
            throw std::logic_error("Can't add parent to self.");
#endif

        // List
        if (isList())
        {
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
            if (itemType_ == TT_END)
                throw std::logic_error("Can't read or write a uninitialized list.");

            if (tag.tagType_ != itemType_)
            {
                String errmsg = "Can't add the tag of " + getTagTypeString(tag.type());
                errmsg += " to the list of " + getTagTypeString(itemType_);

                throw std::logic_error(errmsg);
            }
#endif

            if (!tagData_.ld)
                tagData_.ld = new Vec<Tag>();

            bool needShuffle = (tagData_.ld->capacity() - tagData_.ld->size()) == 0;
            tagData_.ld->emplace_back(std::move(tag));

            if (needShuffle)
            {
                for (auto& var : *tagData_.ld)
                    var.parent_ = this;
            }
            else
            {
                tagData_.ld->back().parent_ = this;
            }

            if (tagData_.ld->back().tagName_)
            {
                delete tagData_.ld->back().tagName_;
                tagData_.ld->back().tagName_ = nullptr;
            }
        }
        // Compound
        else
        {
            if (!tagData_.cd)
                tagData_.cd = new CompoundData();

            if (hasTag(tag.name()))
            {
                tagData_.cd->data[tagData_.cd->idxs[tag.name()]] = std::move(tag);
            }
            else
            {
                bool needShuffle = (tagData_.cd->data.capacity() - tagData_.cd->size()) == 0;
                tagData_.cd->data.emplace_back(std::move(tag));
                tagData_.cd->idxs.insert({ tagData_.cd->data.back().name(), tagData_.cd->data.size() - 1 });

                if (needShuffle)
                {
                    for (auto& var : tagData_.cd->data)
                        var.parent_ = this;
                }
                else
                {
                    tagData_.cd->data.back().parent_ = this;
                }
            }
        }

        return *this;
    }

    /// @overload
    Tag& addTag(Tag& tag) { return addTag(std::move(tag)); }

    /// @brief Functions for get value. Only be called via corresponding tag.

    /// @attention Only be called via #TT_BYTE.
    byte getByte() const
    {
        assert(isByte());
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
        if (!isByte())
            throw std::logic_error("Can't get byte value for non-byte tag.");
#endif

        return tagData_.num.i8;
    }

    /// @attention Only be called via #TT_SHORT.
    int16 getShort() const
    {
        assert(isShort());
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
        if (!isShort())
            throw std::logic_error("Can't get short value for non-short tag.");
#endif

        return tagData_.num.i16;
    }

    /// @attention Only be called via #TT_INT.
    int32 getInt() const
    {
        assert(isInt());
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
        if (!isInt())
            throw std::logic_error("Can't get int value for non-int tag.");
#endif

        return tagData_.num.i32;
    }

    /// @attention Only be called via #TT_LONG.
    int64 getLong() const
    {
        assert(isLong());
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
        if (!isLong())
            throw std::logic_error("Can't get long value for non-long tag.");
#endif

        return tagData_.num.i64;
    }

    /// @attention Only be called via #TT_FLOAT.
    fp32 getFloat() const
    {
        assert(isFloat());
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
        if (!isFloat())
            throw std::logic_error("Can't get float value for non-float tag.");
#endif

        return tagData_.num.f32;
    }

    /// @attention Only be called via #TT_DOUBLE.
    fp64 getDouble() const
    {
        assert(isDouble());
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
        if (!isDouble())
            throw std::logic_error("Can't get double value for non-double tag.");
#endif

        return tagData_.num.f64;
    }

    /// @brief Fast way of get the integer value.
    /// @attention Only be called via #TT_BYTE, #TT_SHORT, #TT_INT, #TT_LONG.
    int64 getInteger() const
    {
        assert(isInteger());
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
        if (!isInteger())
            throw std::logic_error("Can't get interger number for non-integer tag.");
#endif

        if (isByte())           return tagData_.num.i8;
        if (isShort())          return tagData_.num.i16;
        if (isInt())            return tagData_.num.i32;
        if (isLong())           return tagData_.num.i64;

        return 0;
    }

    /// @brief Fast way of get the float point value.
    /// @attention Only be called via #TT_FLOAT, #TT_DOUBLE.
    fp64 getFloatPoint() const
    {
        assert(isFloatPoint());
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
        if (!isFloatPoint())
            throw std::logic_error("Can't get float point number value for non-float point tag.");
#endif

        if (isFloat())          return tagData_.num.f32;
        else if (isDouble())    return tagData_.num.f64;

        return 0;
    }

    /// @attention Only be called via #TT_STRING.
    String getString() const
    {
        assert(isString());
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
        if (!isString())
            throw std::logic_error("Can't get string value for non-string tag.");
#endif

        return tagData_.str ? *tagData_.str : "";
    }

    /// @attention Only be called via #TT_BYTE_ARRAY.
    Vec<byte> getByteArray() const
    {
        assert(isByteArray());
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
        if (!isByteArray())
            throw std::logic_error("Can't get byte array value for non-byte array tag.");
#endif

        return tagData_.bad ? *tagData_.bad : Vec<byte>();
    }

//     const Vec<byte>& getByteArray() const
//     {
//         assert(isByteArray());
// #if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
//         if (!isByteArray())
//             throw std::logic_error("Can't get byte array value for non-byte array tag.");
// #endif
// 
//         if (!tagData_.bad)
//         {
//             static Vec<byte> placeHolder;
//             return placeHolder;
//         }
// 
//         return *tagData_.bad;
//     }
// 
//     Vec<byte>& getByteArray()
//     {
//         assert(isByteArray());
// #if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
//         if (!isByteArray())
//             throw std::logic_error("Can't get byte array value for non-byte array tag.");
// #endif
// 
//         if (!tagData_.bad)
//             tagData_.bad = new Vec<byte>();
// 
//         return *tagData_.bad;
//     }

    /// @attention Only be called via #TT_INT_ARRAY.
    Vec<int32> getIntArray() const
    {
        assert(isIntArray());
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
        if (!isIntArray())
            throw std::logic_error("Can't get int array value for non-int array tag.");
#endif

        return tgaData_.iad ? *tgaData_.iad : Vec<int32>();
    }

    /// @attention Only be called via #TT_LONG_ARRAY.
    Vec<int64> getLongArray() const
    {
        assert(isLongArray());
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
        if (!isLongArray())
            throw std::logic_error("Can't get long array value for non-long array tag.");
#endif

        return tgaData_.iad ? *tgaData_.iad : Vec<int64>();
    }

    /// @overload
    /// @brief Get a value from the byte array by index.
    /// @attention Only be called via #TT_BYTE_ARRAY.
    byte getByte(size_t idx) const
    {
        assert(isByteArray());
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
        if (!isByteArray())
            throw std::logic_error("Can't get byte value from non-byte array tag.");
#endif

        if (!tagData_.bad || idx >= tagData_.bad->size())
            throw std::out_of_range("The specified index is out of range.");

        return (*tagData_.bad)[idx];
    }

    /// @attention Only be called via #TT_BYTE_ARRAY.
    byte getFrontByte() const
    {
        assert(isByteArray());
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
        if (!isByteArray())
            throw std::logic_error("Can't get front byte value from non-byte array tag.");
#endif

        if (!tagData_.bad || tagData_.bad->empty())
            throw std::out_of_range("The front member is not exists.");

        return tagData_.bad->front();
    }

    /// @attention Only be called via #TT_BYTE_ARRAY.
    byte getBackByte() const
    {
        assert(isByteArray());
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
        if (!isByteArray())
            throw std::logic_error("Can't get back byte value from non-byte array tag.");
#endif

        if (!tagData_.bad || tagData_.bad->empty())
            throw std::out_of_range("The back member is not exits.");

        return tagData_.bad->back();
    }

    /// @overload
    /// @brief Get a value from the int array by index.
    /// @attention Only be called via #TT_INT_ARRAY.
    int32 getInt(size_t idx) const
    {
        assert(isIntArray());
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
        if (!isIntArray())
            throw std::logic_error("Can't get int value from non-int array tag.");
#endif

        if (!tagData_.iad || idx >= tagData_.iad->size())
            throw std::out_of_range("The specified index is out of range.");

        return (*tagData_.iad)[idx];
    }

    /// @attention Only be called via #TT_INT_ARRAY.
    int32 getFrontInt() const
    {
        assert(isIntArray());
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
        if (!isIntArray())
            throw std::logic_error("Can't get front int value from non-int array tag.");
#endif

        if (!tagData_.iad || tagData_.iad->empty())
            throw std::out_of_range("The front member is not exists.");

        return tagData_.iad->front();
    }

    /// @attention Only be called via #TT_INT_ARRAY.
    int32 getBackInt() const
    {
        assert(isIntArray());
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
        if (!isIntArray())
            throw std::logic_error("Can't get back int value from non-int array tag.");
#endif

        if (!tagData_.iad || tagData_.iad->empty())
            throw std::out_of_range("The back member is not exits.");

        return tagData_.iad->back();
    }

    /// @overload
    /// @brief Get a value from long array by index.
    /// @attention Only be called via #TT_LONG_ARRAY.
    int64 getLong(size_t idx) const
    {
        assert(isLongArray());
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
        if (!isLongArray())
            throw std::logic_error("Can't get long value from non-long array tag.");
#endif

        if (!tagData_.lad || idx >= tagData_.lad->size())
            throw std::out_of_range("The specified index is out of range.");

        return (*tagData_.lad)[idx];
    }

    /// @attention Only be called via #TT_LONG_ARRAY.
    int64 getFrontLong() const
    {
        assert(isLongArray());
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
        if (!isLongArray())
            throw std::logic_error("Can't get front long value from non-long array tag.");
#endif

        if (!tagData_.lad || tagData_.lad->empty())
            throw std::out_of_range("The front member is not exists.");

        return tagData_.lad->front();
    }

    /// @attention Only be called via #TT_LONG_ARRAY.
    int64 getBackLong() const
    {
        assert(isLongArray());
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
        if (!isLongArray())
            throw std::logic_error("Can't get back long value from non-long array tag.");
#endif

        if (!tagData_.lad || tagData_.lad->empty())
            throw std::out_of_range("The back member is not exits.");

        return tagData_.lad->back();
    }

    /// @brief Get the tag by index.
    /// @attention Only be called via #TT_LIST, #TT_COMPOUND.
    Tag& getTag(size_t idx)
    {
        assert(isContainer());
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
        if (!isContainer())
            throw std::logic_error("Can't get tag from non-container tag.");
#endif

        // List
        if (isList())
        {
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
            if (itemType_ == TT_END)
                throw std::logic_error("Can't read or write a uninitialized list.");
#endif

            if (!tagData_.ld || idx >= tagData_.ld->size())
                throw std::out_of_range("The specified index is out of range.");

            return (*tagData_.ld)[idx];
        }
        // Compound
        else
        {
            if (!tagData_.cd || idx >= tagData_.cd->size())
                throw std::out_of_range("The specified index is out of range.");

            return tagData_.cd->data[idx];
        }
    }

    /// @overload
    /// @brief Get the tag by name.
    /// @attention Only be called via #TT_COMPOUND.
    Tag& getTag(const String& name)
    {
        assert(isCompound());
        assert(hasTag(name));
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
        if (!isCompound())
            throw std::logic_error("Can't get tag from non-compound tag.");

        if (!hasTag(name))
            throw std::logic_error("The specified name is not exists.");

        if (!tagData_.cd)
            throw std::logic_error("The member of specified name is not exists.");
#endif

        return tagData_.cd->data[tagData_.cd->idxs[name]];
    }

    /// @attention Only be called via #TT_LIST, #TT_COMPOUND.
    Tag& getFrontTag()
    {
        assert(isContainer());
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
        if (!isContainer())
            throw std::logic_error("Can't get front tag from non-container tag.");
#endif

        // List
        if (isList())
        {
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
            if (itemType_ == TT_END)
                throw std::logic_error("Can't read or write a uninitialized list.");
#endif

            if (!tagData_.ld || tagData_.ld->empty())
                throw std::out_of_range("The front member is not exists.");

            return tagData_.ld->front();
        }
        // Compound
        else
        {
            if (!tagData_.cd || tagData_.cd->empty())
                throw std::out_of_range("The front member is not exists.");

            return tagData_.cd->data.front();
        }
    }

    /// @attention Only be called via #TT_LIST, #TT_COMPOUND.
    Tag& getBackTag()
    {
        assert(isContainer());
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
        if (!isContainer())
            throw std::logic_error("Can't get back tag from non-container tag.");
#endif

        // List
        if (isList())
        {
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
            if (itemType_ == TT_END)
                throw std::logic_error("Can't read or write a uninitialized list.");
#endif

            if (!tagData_.ld || tagData_.ld->empty())
                throw std::out_of_range("The back member is not exists.");

            return tagData_.ld->back();
        }
        // Compound
        else
        {
            if (!tagData_.cd || tagData_.cd->empty())
                throw std::out_of_range("The back member is not exists.");

            return tagData_.cd->data.back();
        }
    }

    /// @brief Remove the element by index.
    /// @attention Only be called via
    // #TT_STRING, #TT_BYTE_ARRAY, #TT_INT_ARRAY, #TT_LONG_ARRAY, #TT_LIST, #TT_COMPOUND.
    Tag& remove(size_t idx)
    {
        assert(isString() || isArray() || isContainer());
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
        if (!isString() && !isArray() && !isContainer())
            throw std::logic_error("Can't remove element from non-string, non-array, non-container tag.");
#endif

        if (isString())
        {
            if (!tagData_.str || idx >= tagData_.str->size())
                throw std::out_of_range("The specified index is out of range.");

            tagData_.str->erase(tagData_.str->begin() + idx);
        }
        else if (isByteArray())
        {
            if (!tagData_.bad || idx >= tagData_.bad->size())
                throw std::out_of_range("The specified index is out of range.");

            tagData_.bad->erase(tagData_.bad->begin() + idx);
        }
        else if (isIntArray())
        {
            if (!tagData_.iad || idx >= tagData_.iad->size())
                throw std::out_of_range("The specified index is out of range.");

            tagData_.iad->erase(tagData_.iad->begin() + idx);
        }
        else if (isLongArray())
        {
            if (!tagData_.lad || idx >= tagData_.lad->size())
                throw std::out_of_range("The specified index is out of range.");

            tagData_.lad->erase(tagData_.lad->begin() + idx);
        }
        else if (isList())
        {
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
            if (itemType_ == TT_END)
                throw std::logic_error("Can't read or write a uninitialized list.");
#endif

            if (!tagData_.ld || idx >= tagData_.ld->size())
                throw std::out_of_range("The specified index is out of range.");

            tagData_.ld->erase(tagData_.ld->begin() + idx);
        }
        else if (isCompound())
        {
            if (!tagData_.cd || idx >= tagData_.cd->size())
                throw std::out_of_range("The specified index is out of range.");

            tagData_.cd->idxs.erase(tagData_.cd->data[idx].name());
            tagData_.cd->data.erase(tagData_.cd->data.begin() + idx);

            for (auto& var : tagData_.cd->idxs)
                if (var.second > idx)
                    var.second--;
        }

        return *this;
    }

    /// @overload
    /// @brief Remove the tag by name.
    /// @attention Only be called via #TT_COMPOUND.
    Tag& remove(const String& name)
    {
        assert(isCompound());
        assert(hasTag(name));
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
        if (!isCompound())
            throw std::logic_error("Can't remove tag from non-compound tag.");

        if (!hasTag(name))
            throw std::logic_error("The specified name is not exists.");

        if (!tagData_.cd)
            throw std::logic_error("The member of specified name is not exists.");
#endif

        size_t idx = tagData_.cd->idxs[name];

        tagData_.cd->data.erase(tagData_.cd->data.begin() + idx);
        tagData_.cd->idxs.erase(name);

        for (auto& var : tagData_.cd->idxs)
            if (var.second > idx)
                var.second--;

        return *this;
    }

    /// @brief Remove the first element.
    /// @attention Only be called via
    // #TT_STRING, #TT_BYTE_ARRAY, #TT_INT_ARRAY, #TT_LONG_ARRAY, #TT_LIST, #TT_COMPOUND.
    Tag& removeFront()
    {
        assert(isString() || isArray() || isContainer());
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
        if (!isString() && !isArray() && !isContainer())
            throw std::logic_error("Can't remove front element from non-string, non-array, non-container tag.");
#endif

        if (isString())
        {
            if (!tagData_.str || tagData_.str->empty())
                throw std::out_of_range("The front member is not exists.");

            tagData_.str->erase(tagData_.str->begin());
        }
        else if (isByteArray())
        {
            if (!tagData_.bad || tagData_.bad->empty())
                throw std::out_of_range("The front member is not exists.");

            tagData_.bad->erase(tagData_.bad->begin());
        }
        else if (isIntArray())
        {
            if (!tagData_.iad || tagData_.iad->empty())
                throw std::out_of_range("The front member is not exists.");

            tagData_.iad->erase(tagData_.iad->begin());
        }
        else if (isLongArray())
        {
            if (!tagData_.lad || tagData_.lad->empty())
                throw std::out_of_range("The front member is not exists.");

            tagData_.lad->erase(tagData_.lad->begin());
        }
        else if (isList())
        {
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
            if (itemType_ == TT_END)
                throw std::logic_error("Can't read or write a uninitialized list.");
#endif

            if (!tagData_.ld || tagData_.ld->empty())
                throw std::out_of_range("The front member is not exists.");

            tagData_.ld->erase(tagData_.ld->begin());
        }
        else if (isCompound())
        {
            if (!tagData_.cd || tagData_.cd->empty())
                throw std::out_of_range("The front member is not exists.");

            tagData_.cd->idxs.erase(tagData_.cd->data.front().name());
            tagData_.cd->data.erase(tagData_.cd->data.begin());

            for (auto& var : tagData_.cd->idxs)
                var.second--;
        }

        return *this;
    }

    /// @brief Remove the last element.
    /// @attention Only be called via
    // #TT_STRING, #TT_BYTE_ARRAY, #TT_INT_ARRAY, #TT_LONG_ARRAY, #TT_LIST, #TT_COMPOUND.
    Tag& removeBack()
    {
        assert(isString() || isArray() || isContainer());
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
        if (!isString() && !isArray() && !isContainer())
            throw std::logic_error("Can't remove back element from non-string, non-array, non-container tag.");
#endif

        if (isString())
        {
            if (!tagData_.str || tagData_.str->empty())
                throw std::out_of_range("The back member is not exists.");

            tagData_.str->pop_back();
        }
        else if (isByteArray())
        {
            if (!tagData_.bad || tagData_.bad->empty())
                throw std::out_of_range("The back member is not exists.");

            tagData_.bad->pop_back();
        }
        else if (isIntArray())
        {
            if (!tagData_.iad || tagData_.iad->empty())
                throw std::out_of_range("The back member is not exists.");

            tagData_.iad->pop_back();
        }
        else if (isLongArray())
        {
            if (!tagData_.lad || tagData_.lad->empty())
                throw std::out_of_range("The back member is not exists.");

            tagData_.lad->pop_back();
        }
        else if (isList())
        {
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
            if (itemType_ == TT_END)
                throw std::logic_error("Can't read or write a uninitialized list.");
#endif

            if (!tagData_.ld || tagData_.ld->empty())
                throw std::out_of_range("The back member is not exists.");

            tagData_.ld->pop_back();
        }
        else if (isCompound())
        {
            if (!tagData_.cd || tagData_.cd->empty())
                throw std::out_of_range("The back member is not exists.");

            tagData_.cd->idxs.erase(tagData_.cd->data.back().name());
            tagData_.cd->data.pop_back();
        }

        return *this;
    }

    /// @brief Remove the all elements.
    /// @attention Only be called via
    // #TT_STRING, #TT_BYTE_ARRAY, #TT_INT_ARRAY, #TT_LONG_ARRAY, #TT_LIST, #TT_COMPOUND.
    Tag& removeAll()
    {
        assert(isString() || isArray() || isContainer());
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
        if (!isString() && !isArray() && !isContainer())
            throw std::logic_error("Can't remove all elements from non-string, non-array, non-container tag.");
#endif

        if (isString())
        {
            if (tagData_.str)
                tagData_.str->clear();
        }
        else if (isByteArray())
        {
            if (tagData_.bad)
                tagData_.bad->clear();
        }
        else if (isIntArray())
        {
            if (tagData_.iad)
                tagData_.iad->clear();
        }
        else if (isLongArray())
        {
            if (tagData_.lad)
                tagData_.lad->clear();
        }
        else if (isList())
        {
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
            if (itemType_ == TT_END)
                throw std::logic_error("Can't read or write a uninitialized list.");
#endif

            if (tagData_.ld)
                tagData_.ld->clear();
        }
        else if (isCompound())
        {
            if (tagData_.cd)
                tagData_.cd->clear();
        }

        return *this;
    }

#ifdef MCNBT_USE_GZIP
    /// @brief Write the tag to output stream.
    void write(OStream& os, bool isBigEndian, bool isCompressed = false) const
    {
        if (isCompressed)
        {
            SStream ss;
            write_(ss, isBigEndian, isListItem());
            os << gzip::compress(ss.str());
        }
        else
        {
            write_(os, isBigEndian, isListItem());
        }
    }

    /// @overload
    void write(const String& filename, bool isBigEndian, bool isCompressed = false) const
    {
        OFStream ofs(filename, std::ios_base::binary);

        if (ofs.is_open())      write(ofs, isBigEndian, isCompressed);
        else                    throw std::runtime_error("Failed to open file: " + filename);

        ofs.close();
    }
#else
    /// @brief Write the tag to output stream.
    void write(OStream& os, bool isBigEndian) const { write_(os, isBigEndian, isListItem()); }

    /// @overload
    void write(const String& filename, bool isBigEndian) const
    {
        OFStream ofs(filename, std::ios_base::binary);

        if (ofs.is_open())      write(ofs, isBigEndian);
        else                    throw std::runtime_error("Failed to open file: " + filename);

        ofs.close();
    }
#endif // MCNBT_USE_GZIP

    /// @brief Get the SNBT (The string representation of NBT).
    /// @param isWrappedIndented If true, the output string will be wrapped and indented.
    String toSnbt(bool isWrappedIndented = true) const { return toSnbt_(isWrappedIndented, isListItem()); }

    /// @brief Operators overloading.

    /// @brief Fast way of get the tag by index.
    Tag& operator[](size_t idx)             { return getTag(idx); }

    /// @overload
    /// @brief Fast way of get the tag by name.
    Tag& operator[](const String& name)     { return getTag(name); }

    /// @brief Fast way of add the tag.
    Tag& operator<<(Tag&& tag)              { return addTag(std::move(tag)); }

    /// @overload
    Tag& operator<<(Tag& tag)               { return addTag(tag); }

private:
    // Nums.
    // Contains interger and float point number.
    union Num
    {
        Num() : i64(0) {}

        byte    i8;
        int16   i16;
        int32   i32;
        int64   i64;
        fp32    f32;
        fp64    f64;
    };

    // A simple wrapper of std::vector<tag> and std::map<string, size_t>.
    struct CompoundData
    {
        Vec<Tag> data;
        Map<String, size_t> idxs;

        bool empty() const          { return data.empty(); }

        size_t size() const         { return data.size(); }

        void reserve(size_t size)   { data.reserve(size); idxs.reserve(size); }

        void clear()                { data.clear(); idxs.clear(); }
    };

    // Value of tag.
    // Individual tag is like key-value pair.
    // The key is the name of tag (can be empty. e.g. All list element not has name).
    // The value is stored in the following union.
    union Data
    {
        Data() : num(Num()) {}

        // Number data
        Num             num;
        // String data
        String*         str;
        // Byte Array data
        Vec<byte>*      bad;
        // Int Array data
        Vec<int32>*     iad;
        // Long Array data
        Vec<int64>*     lad;
        // List data
        Vec<Tag>*       ld;
        // Compound data
        CompoundData*   cd;
    };

    /// @brief Get the tag from a binary input stream.
    /// @param isListItem       Whether the parent is a List tag.
    /// @param parentType       If the parameter #isListItem is false, ignore this.
    // Else this must be set to same as the element tag type of parent List.
    static Tag fromBinStream_(IStream& is, bool isBigEndian, bool isListItem, TagType parentType = TT_END)
    {
        Tag tag;

        // Get the tag type.
        // If the parent is a List, that is this tag is a list element, get the tag type from parent.
        // Else get the tag type from stream.
        if (isListItem)         tag.tagType_ = parentType;
        else                    tag.tagType_ = static_cast<TagType>(is.get());

        if (tag.tagType_ == TT_END)
            return tag;

        // Get the tag name (key).
        // If the tag not is a list element obtain the name from stream.
        if (!isListItem)
        {
            int16 nameLen = _bytes2num<int16>(is, isBigEndian);
            if (nameLen != 0)
            {
                byte* bytes = new byte[nameLen];

                is.read(bytes, nameLen);
                tag.tagName_ = new String();
                tag.tagName_->assign(bytes, static_cast<size_t>(is.gcount()));

                delete[] bytes;
            }
        }

        // Get the tag dat (value).
        switch (tag.tagType_)
        {
            case TT_BYTE:
                tag.tagData_.num.i8 = _bytes2num<byte>(is, isBigEndian);
                break;
            case TT_SHORT:
                tag.tagData_.num.i16 = _bytes2num<int16>(is, isBigEndian);
                break;
            case TT_INT:
                tag.tagData_.num.i32 = _bytes2num<int32>(is, isBigEndian);
                break;
            case TT_LONG:
                tag.tagData_.num.i64 = _bytes2num<int64>(is, isBigEndian);
                break;
            case TT_FLOAT:
                tag.tagData_.num.f32 = _bytes2num<fp32>(is, isBigEndian);
                break;
            case TT_DOUBLE:
                tag.tagData_.num.f64 = _bytes2num<fp64>(is, isBigEndian);
                break;
            case TT_STRING:
            {
                int16 strlen = _bytes2num<int16>(is, isBigEndian);

                if (strlen != 0)
                {
                    byte* bytes = new byte[strlen];

                    is.read(bytes, strlen);
                    tag.tagData_.str = new String();
                    tag.tagData_.str->assign(bytes, static_cast<size_t>(is.gcount()));

                    delete[] bytes;
                }
                break;
            }
            case TT_BYTE_ARRAY:
            {
                int32 dsize = _bytes2num<int32>(is, isBigEndian);

                if (dsize != 0)
                {
                    tag.tagData_.bad = new Vec<byte>();
                    tag.tagData_.bad->reserve(dsize);

                    for (int32 i = 0; i < dsize; ++i)
                        tag.addByte(_bytes2num<byte>(is, isBigEndian));
                }
                break;
            }
            case TT_INT_ARRAY:
            {
                int32 dsize = _bytes2num<int32>(is, isBigEndian);

                if (dsize != 0)
                {
                    tag.tagData_.iad = new Vec<int32>();
                    tag.tagData_.iad->reserve(dsize);

                    for (int32 i = 0; i < dsize; ++i)
                        tag.addInt(_bytes2num<int32>(is, isBigEndian));
                }
                break;
            }
            case TT_LONG_ARRAY:
            {
                int32 dsize = _bytes2num<int32>(is, isBigEndian);

                if (dsize != 0)
                {
                    tag.tagData_.lad = new Vec<int64>();
                    tag.tagData_.lad->reserve(dsize);

                    for (int32 i = 0; i < dsize; ++i)
                        tag.addLong(_bytes2num<int64>(is, isBigEndian));
                }
                break;
            }
            case TT_LIST:
            {
                tag.itemType_ = static_cast<TagType>(is.get());
                int32 dsize = _bytes2num<int32>(is, isBigEndian);

                if (dsize != 0)
                {
                    tag.tagData_.ld = new Vec<Tag>();
                    tag.tagData_.ld->reserve(dsize);

                    for (int32 i = 0; i < dsize; ++i)
                        tag.addTag(fromBinStream_(is, isBigEndian, true, tag.));
                }
                break;
            }
            case TT_COMPOUND:
            {
                while (!is.eof()) {
                    if (is.peek() == TT_END)
                    {
                        // Give up End tag and move stream point to next byte.
                        is.get();
                        break;
                    }

                    tag.addTag(fromBinStream_(is, isBigEndian, false));
                }
                break;
            }
            default:
                throw std::runtime_error("Invalid tag type.");
        }

        return tag;
    }

    // TODO
    static Tag fromSnbt_(IStream& snbtSs, TagType parentType);

    void write_(OStream& os, bool isBigEndian, bool isListItem) const
    {
        if (!isListItem)
        {
            os.put(static_cast<byte>(tagType_));

            if (!tagName_ || tagName_->empty())
            {
                _num2bytes<int16>(static_cast<int16>(0), os, isBigEndian);
            }
            else
            {
                _num2bytes<int16>(static_cast<int16>(tagName_->size()), os, isBigEndian);
                os.write(tagName_->c_str(), tagName_->size());
            }
        }

        switch (tagType_)
        {
            case TT_END:
                os.put(TT_END);
                break;
            case TT_BYTE:
                os.put(tagData_.num.i8);
                break;
            case TT_SHORT:
                _num2bytes<int16>(tagData_.num.i16, os, isBigEndian);
                break;
            case TT_INT:
                _num2bytes<int32>(tagData_.num.i32, os, isBigEndian);
                break;
            case TT_LONG:
                _num2bytes<int64>(tagData_.num.i64, os, isBigEndian);
                break;
            case TT_FLOAT:
                _num2bytes<fp32>(tagData_.num.f32, os, isBigEndian);
                break;
            case TT_DOUBLE:
                _num2bytes<fp64>(tagData_.num.f64, os, isBigEndian);
                break;
            case TT_STRING:
            {
                if (!tagData_.str || tagData_.str->empty())
                {
                    _num2bytes<int16>(static_cast<int16>(0), os, isBigEndian);
                    break;
                }

                _num2bytes<int16>(static_cast<int16>(tagData_.str->size()), os, isBigEndian);
                os.write(tagData_.str->c_str(), tagData_.str->size());

                break;
            }
            case TT_BYTE_ARRAY:
            {
                if (!tagData_.bad || tagData_.bad->empty())
                {
                    _num2bytes<int32>(static_cast<int32>(0), os, isBigEndian);
                    break;
                }

                _num2bytes<int32>(static_cast<int32>(tagData_.bad->size()), os, isBigEndian);

                for (const auto& var : *tagData_.bad)
                    os.put(var);

                break;
            }
            case TT_INT_ARRAY:
            {
                if (!tagData_.iad || tagData_.iad->empty())
                {
                    _num2bytes<int32>(static_cast<int32>(0), os, isBigEndian);
                    break;
                }

                _num2bytes<int32>(static_cast<int32>(tagData_.iad->size()), os, isBigEndian);

                for (const auto& var : *tagData_.iad)
                    _num2bytes<int32>(var, os, isBigEndian);

                break;
            }
            case TT_LONG_ARRAY:
            {
                if (!tagData_.lad || tagData_.lad->empty())
                {
                    _num2bytes<int32>(static_cast<int32>(0), os, isBigEndian);
                    break;
                }

                _num2bytes<int32>(static_cast<int32>(tagData_.lad->size()), os, isBigEndian);

                for (const auto& var : *tagData_.lad)
                    _num2bytes<int64>(var, os, isBigEndian);

                break;
            }
            case TT_LIST:
            {
                if (!tagData_.ld || tagData_.ld->empty())
                {
                    os.put(static_cast<byte>(TT_END));
                    _num2bytes<int32>(static_cast<int32>(0), os, isBigEndian);
                    break;
                }

                os.put(static_cast<byte>(itemType_));
                _num2bytes<int32>(static_cast<int32>(tagData_.ld->size()), os, isBigEndian);

                for (const auto& var : *tagData_.ld)
                    var.write_(os, isBigEndian, true);

                break;
            }
            case TT_COMPOUND:
            {
                if (!tagData_.cd || tagData_.cd->data.empty())
                {
                    os.put(TT_END);
                    break;
                }

                for (const auto& var : tagData_.cd->data)
                    var.write_(os, isBigEndian, false);

                os.put(TT_END);

                break;
            }
            default:
                throw std::runtime_error("Invalid tag type.");
        }
    }

    String toSnbt_(bool isWrappedIndented, bool isListItem) const
    {
        static size_t indentCount = 0;

        String inheritedIndentStr(indentCount * _SNBT_INDENT_WIDTH, ' ');
        String key = isWrappedIndented ? inheritedIndentStr : "";

        if (!isListItem && !name().empty())
            key += name() + (isWrappedIndented ? ": " : ":");

        switch (type())
        {
            case TT_END:        return "";
            case TT_BYTE:       return key + std::to_string(static_cast<int>(tagData_.num.i8)) + 'b';
            case TT_SHORT:      return key + std::to_string(tagData_.num.i16) + 's';
            case TT_INT:        return key + std::to_string(tagData_.num.i32);
            case TT_LONG:       return key + std::to_string(tagData_.num.i64) + 'l';
            case TT_FLOAT:      return key + std::to_string(tagData_.num.f32) + 'f';
            case TT_DOUBLE:     return key + std::to_string(tagData_.num.f64) + 'd';
            case TT_STRING:     return key + '"' + (tagData_.str ? *tagData_.str : "") + '"';
            case TT_BYTE_ARRAY:
            {
                if (!tagData_.bad || tagData_.bad->empty())
                    return key + "[B;]";

                // If has indent add the newline character and indent string.
                String result = key + '[';
                result += isWrappedIndented ? ('\n' + inheritedIndentStr + _SNBT_INDENT_STR) : "";
                result += "B;";

                for (const auto& var : *tagData_.bad)
                {
                    result += isWrappedIndented ? ('\n' + inheritedIndentStr + _SNBT_INDENT_STR) : "";
                    result += std::to_string(static_cast<int>(var)) + "b,";
                }

                if (result.back() == ',')
                    result.pop_back();

                result += isWrappedIndented ? ('\n' + inheritedIndentStr) : "";
                result += ']';

                return result;
            }
            case TT_INT_ARRAY:
            {
                if (!tagData_.iad || tagData_.iad->empty())
                    return key + "[I;]";

                // If has indent add the newline character and indent string.
                String result = key + '[';
                result += isWrappedIndented ? ('\n' + inheritedIndentStr + _SNBT_INDENT_STR) : "";
                result += "I;";

                for (const auto& var : *tagData_.iad)
                {
                    result += isWrappedIndented ? ('\n' + inheritedIndentStr + _SNBT_INDENT_STR) : "";
                    result += std::to_string(var) + ",";
                }

                if (result.back() == ',')
                    result.pop_back();

                result += isWrappedIndented ? ('\n' + inheritedIndentStr) : "";
                result += ']';

                return result;
            }
            case TT_LONG_ARRAY:
            {
                if (!tagData_.lad || tagData_.lad->empty())
                    return key + "[L;]";

                // If has indent add the newline character and indent string.
                String result = key + '[';
                result += isWrappedIndented ? ('\n' + inheritedIndentStr + _SNBT_INDENT_STR) : "";
                result += "L;";

                for (const auto& var : *tagData_.lad)
                {
                    result += isWrappedIndented ? ('\n' + inheritedIndentStr + _SNBT_INDENT_STR) : "";
                    result += std::to_string(var) + "l,";
                }

                if (result.back() == ',')
                    result.pop_back();                

                result += isWrappedIndented ? ('\n' + inheritedIndentStr) : "";
                result += ']';

                return result;
            }
            case TT_LIST:
            {
                if (!tagData_.ld || tagData_.ld->empty())
                    return key + "[]";

                String result = key + '[';

                indentCount++;
                for (const auto& var : *tagData_.ld)
                {
                    result += isWrappedIndented ? "\n" : "";
                    result += var.toSnbt_(isWrappedIndented, true) + ",";
                }
                indentCount--;

                if (result.back() == ',')
                    result.pop_back();

                result += isWrappedIndented ? ('\n' + inheritedIndentStr) : "";
                result += ']';

                return result;
            }
            case TT_COMPOUND:
            {
                if (!tagData_.cd || tagData_.cd->data.empty())
                    return key + "{}";

                String result = key + "{";

                indentCount++;
                for (const auto& var : tagData_.cd->data)
                {
                    result += isWrappedIndented ? "\n" : "";
                    result += var.toSnbt_(isWrappedIndented, false) + ",";
                }
                indentCount--;

                if (result.back() == ',')
                    result.pop_back();

                result += isWrappedIndented ? ('\n' + inheritedIndentStr) : "";
                result += '}';

                return result;
            }
            default:
                throw std::runtime_error("Invalid tag type.");
        }
    }

    // Release the all alloced memory.
    // (e.g. tag name and tag value.)
    void release_()
    {
        if (isString() && tagData_.str)             delete tagData_.str;
        else if (isByteArray() && tagData_.bad)     delete tagData_.bad;
        else if (isIntArray() && tagData_.iad)      delete tagData_.iad;
        else if (isLongArray() && tagData_.lad)     delete tagData_.lad;
        else if (isList() && tagData_.ld)           delete tagData_.ld;
        else if (isCompound() && tagData_.cd)       delete tagData_.cd;
        tagData_.str = nullptr;

        if (tagName_)
        {
            delete tagName_;
            tagName_ = nullptr;
        }
    }

    TagType tagType_    = TT_END;
    TagType itemType_   = TT_END;    ///< Tag type of the list items. Just usefull for list tag.
    Data tagData_;
    String* tagName_    = nullptr;
    Tag* parent_        = nullptr;
};

} // namespace nbt

// Faster way for construct a tag object.
namespace nbt
{

inline Tag gByte(byte value, const String& name = "")
{
    Tag tag(TT_BYTE);

    if (!name.empty())
        tag.setName(name);

    return tag.setByte(value);
}

inline Tag gShort(int16 value, const String& name = "")
{
    Tag tag(TT_SHORT);

    if (!name.empty())
        tag.setName(name);

    return tag.setShort(value);
}

inline Tag gInt(int32 value, const String& name = "")
{
    Tag tag(TT_INT);

    if (!name.empty())
        tag.setName(name);

    return tag.setInt(value);
}

inline Tag gLong(int64 value, const String& name = "")
{
    Tag tag(TT_LONG);

    if (!name.empty())
        tag.setName(name);

    return tag.setLong(value);
}

inline Tag gFloat(fp32 value, const String& name = "")
{
    Tag tag(TT_FLOAT);

    if (!name.empty())
        tag.setName(name);

    return tag.setFloat(value);
}

inline Tag gDouble(fp64 value, const String& name = "")
{
    Tag tag(TT_DOUBLE);

    if (!name.empty())
        tag.setName(name);

    return tag.setDouble(value);
}

inline Tag gString(const String& value, const String& name = "")
{
    Tag tag(TT_STRING);

    if (!name.empty())
        tag.setName(name);

    return tag.setString(value);
}

inline Tag gByteArray(const Vec<byte>& value, const String& name = "")
{
    Tag tag(TT_BYTE_ARRAY);

    if (!name.empty())
        tag.setName(name);

    return tag.setByteArray(value);
}

inline Tag gIntArray(const Vec<int32>& value, const String& name = "")
{
    Tag tag(TT_INT_ARRAY);

    if (!name.empty())
        tag.setName(name);

    return tag.setIntArray(value);
}

inline Tag gLongArray(const Vec<int64>& value, const String& name = "")
{
    Tag tag(TT_LONG_ARRAY);

    if (!name.empty())
        tag.setName(name);

    return tag.setLongArray(value);
}

inline Tag gList(TagType dtype, const String& name = "")
{
    Tag tag(TT_LIST);
    tag.initListElementType(dtype);

    if (!name.empty())
        tag.setName(name);

    return tag;
}

inline Tag gCompound(const String& name = "")
{
    Tag tag(TT_COMPOUND);

    if (!name.empty())
        tag.setName(name);

    return tag;
}

} // namespace nbt

#endif // !MCNBT_HPP
