// The "mcnbt" library written in c++.
//
// Webs: https://github.com/JaderoChan/mcnbt
// You can contact me at: c_dl_cn@outlook.com
//
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

#include <cstdint>  // int16_t, int32_t, int64_t
#include <cstddef>  // size_t
#include <cstring>  // strlen(), memcpy()
#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cassert>

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

using uchar = unsigned char;
using byte = char;
using int16 = int16_t;
using int32 = int32_t;
using int64 = int64_t;
using fp32 = float;
using fp64 = double;

using String = std::string;

using SStream = std::stringstream;

using IStream = std::istream;
using OStream = std::ostream;
using IOStream = std::iostream;

using IFStream = std::ifstream;
using OFStream = std::ofstream;
using FStream = std::fstream;

template <typename T>
using Vec = std::vector<T>;

template <typename K, typename V>
using Map = std::unordered_map<K, V>;

} // namespace nbt

// Enum, constants and aux functions.
namespace nbt
{

// Enum of NBT tag type.
enum TagType : uchar
{
    TT_END = 0,
    TT_BYTE = 1,
    TT_SHORT = 2,
    TT_INT = 3,
    TT_LONG = 4,
    TT_FLOAT = 5,
    TT_DOUBLE = 6,
    TT_BYTE_ARRAY = 7,
    TT_STRING = 8,
    TT_LIST = 9,
    TT_COMPOUND = 10,
    TT_INT_ARRAY = 11,
    TT_LONG_ARRAY = 12
};

/*
* Constants about the indent of snbt.
*/

constexpr size_t _SNBT_INDENT_WIDTH = 2;
constexpr char _SNBT_INDENT_CHAR = 0x20;
static const String _SNBT_INDENT_STR = String(_SNBT_INDENT_WIDTH, _SNBT_INDENT_CHAR);

/*
* Aux functions about tag type.
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

inline String getTagTypeString(TagType type)
{
    switch (type) {
        case TT_END:
            return "End";
        case TT_BYTE:
            return "Byte";
        case TT_SHORT:
            return "Short";
        case TT_INT:
            return "Int";
        case TT_LONG:
            return "Long";
        case TT_FLOAT:
            return "Float";
        case TT_DOUBLE:
            return "Double";
        case TT_BYTE_ARRAY:
            return "Byte Array";
        case TT_STRING:
            return "String";
        case TT_LIST:
            return "List";
        case TT_COMPOUND:
            return "Compound";
        case TT_INT_ARRAY:
            return "Int Array";
        case TT_LONG_ARRAY:
            return "Long Array";
        default:
            return "";
    }
}

} // namespace nbt

// Core of read and write binary data.
namespace nbt
{

// @brief Reverse a C style string.
// @param size The size of need reversed range, and reverse all if the size is 0.
inline void _reverse(char* str, size_t size = 0)
{
    // If size is 0, reverse all.
    if (size == 0)
        size = std::strlen(str);

    // For each character in the range, swap it with the corresponding character at the end of the range.
    size_t i = 0;
    while (i < size / 2) {
        char ch = str[i];
        str[i] = str[size - 1 - i];
        str[size - 1 - i] = ch;

        i++;
    }
}

// @brief Check whether the memory order of system of compiler environment is big endian.
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

// @brief Obtain bytes from input stream, and convert it to number.
// @param resumeCursor Whether to resume the cursor position of input stream after read.
// @return A number.
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
        _reverse(buffer, size);

    // Convert the bytes to number. (reinterpreting memory bytes)
    std::memcpy(&result, buffer, size);

    // Resume the cursor position of input stream if needed.
    if (resumeCursor)
        is.seekg(begpos);

    return result;
}

// @brief Convert the number to bytes, and write it to output stream.
template <typename T>
void _num2bytes(T num, OStream& os, bool isBigEndian = false)
{
    size_t size = sizeof(T);

    static byte buffer[sizeof(T)] = {};

    // Convert the number to bytes. (reinterpreting memory bytes)
    std::memcpy(buffer, &num, size);

    // Reverse the bytes if the specified endianness is different from system's endianness.
    if (isBigEndian != _isBigEndian())
        _reverse(buffer, size);

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

    // @brief Construct a tag with tag type.
    explicit Tag(TagType type) : type_(type) {}

    // @note Just copy the other's base data (e.g. name, type, data value), not copy other's parent.
    Tag(const Tag& other) :
        type_(other.type_), dtype_(other.dtype_)
    {
        if (other.isNum())
            data_.num = other.data_.num;
        else if (other.isString() && other.data_.str)
            data_.str = new String(*other.data_.str);
        else if (other.isArray() && other.data_.bad)
            data_.bad = new Vec<byte>(*other.data_.bad);
        else if (other.isIntArray() && other.data_.iad)
            data_.iad = new Vec<int32>(*other.data_.iad);
        else if (other.isLongArray() && other.data_.lad)
            data_.lad = new Vec<int64>(*other.data_.lad);
        else if (other.isList() && other.data_.ld) {
            data_.ld = new Vec<Tag>();
            data_.ld->reserve(other.data_.ld->size());

            for (const auto& var : *other.data_.ld) {
                data_.ld->emplace_back(var);
                data_.ld->back().parent_ = this;
            }
        } else if (other.isCompound() && other.data_.cd) {
            data_.cd = new CompoundData();
            data_.cd->reserve(other.data_.cd->size());

            data_.cd->idxs = other.data_.cd->idxs;
            for (const auto& var : other.data_.cd->data) {
                data_.cd->data.emplace_back(var);
                data_.cd->data.back().parent_ = this;
            }
        }

        if (other.name_ && !other.name_->empty())
            name_ = new String(*other.name_);
    }

    // @ditto
    Tag(Tag&& other) noexcept :
        type_(other.type_), dtype_(other.dtype_), data_(other.data_), name_(other.name_)
    {
        if (isList() && data_.ld)
            for (auto& var : *data_.ld) var.parent_ = this;
        else if (isCompound() && data_.cd)
            for (auto& var : data_.cd->data) var.parent_ = this;

        other.data_.str = nullptr;
        other.name_ = nullptr;
    }

    // Release the all alloced memory and set it's parent to nullptr.
    ~Tag()
    {
        release_();
        parent_ = nullptr;
    }

    // @note Just copy the other's base data (e.g. name, type, data value), not copy other's parent.
    Tag& operator=(const Tag& other)
    {
        if (this == &other)
            return *this;

        assert(!(isListElement() && (type_ != other.type_)));
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
        if (isListElement() && (type_ != other.type_))
            throw std::logic_error("Can't assign a tag of incorrect tag type to list element.");
#endif

        release_();

        type_ = other.type_;
        dtype_ = other.dtype_;

        if (other.isNum())
            data_.num = other.data_.num;
        else if (other.isString() && other.data_.str)
            data_.str = new String(*other.data_.str);
        else if (other.isArray() && other.data_.bad)
            data_.bad = new Vec<byte>(*other.data_.bad);
        else if (other.isIntArray() && other.data_.iad)
            data_.iad = new Vec<int32>(*other.data_.iad);
        else if (other.isLongArray() && other.data_.lad)
            data_.lad = new Vec<int64>(*other.data_.lad);
        else if (other.isList() && other.data_.ld) {
            data_.ld = new Vec<Tag>();
            data_.ld->reserve(other.data_.ld->size());

            for (const auto& var : *other.data_.ld) {
                data_.ld->emplace_back(var);
                data_.ld->back().parent_ = this;
            }
        } else if (other.isCompound() && other.data_.cd) {
            data_.cd = new CompoundData();
            data_.cd->reserve(other.data_.cd->size());

            data_.cd->idxs = other.data_.cd->idxs;
            for (const auto& var : other.data_.cd->data) {
                data_.cd->data.emplace_back(var);
                data_.cd->data.back().parent_ = this;
            }
        }

        if (!isListElement() && other.name_ && !other.name_->empty())
            name_ = new String(*other.name_);

        return *this;
    }

    // @ditto
    Tag& operator=(Tag&& other)
    {
        if (this == &other)
            return *this;

        assert(!isContained(other));
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
        if (isContained(other))
            throw std::logic_error("Can't assign parent to self.");
#endif

        assert(!(isListElement() && (type_ != other.type_)));
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
        if (isListElement() && (type_ != other.type_))
            throw std::logic_error("Can't assign a tag of incorrect tag type to list element");
#endif

        release_();

        type_ = other.type_;
        dtype_ = other.dtype_;
        data_ = other.data_;
        name_ = other.name_;

        if (isList() && data_.ld)
            for (auto& var : *data_.ld) var.parent_ = this;
        else if (isCompound() && data_.cd)
            for (auto& var : data_.cd->data) var.parent_ = this;

        other.data_.str = nullptr;
        other.name_ = nullptr;

        if (isListElement() && name_) {
            delete name_;
            name_ = nullptr;
        }

        return *this;
    }

    // @brief Load the tag from binary input stream.
    // @param is The input stream.
    // @param isBigEndian Whether the read data from input stream with big endian.
    // @param headerSize The size of need discard data from input stream begin.
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

    // @brief Load the tag from a nbt file.
    static Tag fromFile(const String& filename, bool isBigEndian, size_t headerSize = 0)
    {
        IFStream ifs(filename, std::ios::binary);

        if (!ifs.is_open())
            throw std::runtime_error("Failed to open file: " + filename);

        Tag rslt = fromBinStream(ifs, isBigEndian, headerSize);

        ifs.close();

        return rslt;
    }

    // TODO
    static Tag fromSnbt(SStream& snbtSs);

    static Tag fromSnbt(const String& snbt)
    {
        SStream ss;

        ss << snbt;

        return fromSnbt(ss);
    }

    /*
    * Functions of check tag type.
    */

    bool isEnd() const { return nbt::isEnd(type_); }

    bool isByte() const { return nbt::isByte(type_); }

    bool isShort() const { return nbt::isShort(type_); }

    bool isInt() const { return nbt::isInt(type_); }

    bool isLong() const { return nbt::isLong(type_); }

    bool isFloat() const { return nbt::isFloat(type_); }

    bool isDouble() const { return nbt::isDouble(type_); }

    bool isString() const { return nbt::isString(type_); }

    bool isByteArray() const { return nbt::isByteArray(type_); }

    bool isIntArray() const { return nbt::isIntArray(type_); }

    bool isLongArray() const { return nbt::isLongArray(type_); }

    bool isList() const { return nbt::isList(type_); }

    bool isCompound() const { return nbt::isCompound(type_); }

    bool isInteger() const { return nbt::isInteger(type_); }

    bool isFloatPoint() const { return nbt::isFloatPoint(type_); }

    bool isNum() const { return nbt::isNum(type_); }

    bool isArray() const { return nbt::isArray(type_); }

    bool isContainer() const { return nbt::isContainer(type_); }

    /*
    * Functions of common.
    */

    // @brief Make a copy.
    // Usually used for add tag to list or compound.
    // (because default is move when add tag to list or compound)
    Tag copy() const { return Tag(*this); }

    // @brief Get the tag type.
    TagType type() const { return type_; }

    // @brief Get the name of tag.
    String name() const { return name_ ? *name_ : ""; }

    // @brief Get the name length of tag.
    int16 nameLength() const { return name_ ? static_cast<int16>(name_->size()) : 0; }

    // @brief Set the name of tag.
    // @attention Only be called by non-ListElement.
    Tag& setName(const String& name)
    {
        assert(!isListElement());
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
        if (isListElement())
            throw std::logic_error("Can't set name for list element.");
#endif

        if (name.empty() && !name_)
            return *this;

        String oldname = name_ ? *name_ : "";

        if (oldname == name)
            return *this;

        if (!parent_) {
            if (name_)
                *name_ = name;
            else
                name_ = new String(name);

            return *this;
        }

        Tag* p = parent_;
        if (p->hasTag(name))
            p->remove(name);

        Tag& t = (*p)[oldname];

        if (t.name_)
            *t.name_ = name;
        else
            t.name_ = new String(name);

        size_t idx = p->data_.cd->idxs[oldname];

        p->data_.cd->idxs.erase(oldname);
        p->data_.cd->idxs.insert({ name, idx });

        return t;
    }

    // @brief Check if is a list element.
    bool isListElement() const { return parent_ && parent_->isList(); }

    // @brief Check if the parent is exists.
    bool hasParent() const { return parent_ != nullptr; }

    // @return The parent pointer, maybe is nullptr.
    const Tag* parent() const { return parent_; }

    // @brief Check self if is be contained in specified tag.
    // @param container The tag that be checked whether self is contained in it.
    // @note It check all the parent (parent' parent) until happen a parent is nullptr.
    bool isContained(const Tag& container) const
    {
        const Tag* p = parent_;

        while (p) {
            if (p == &container)
                return true;

            p = p->parent_;
        }

        return false;
    }

    /*
    * Functions about list.
    */

    // @brief Check whether the list is initialized (#dtype_ is not TT_END).
    // @attention Only be called by #TT_LIST.
    bool isInitializedList() const
    {
        assert(isList());
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
        if (!isList())
            throw std::logic_error("Can't check initialized list for non-list tag.");
#endif

        return dtype_ != TT_END;
    }

    // @brief Get the element tag type of the list.
    // @attention Only be called by #TT_LIST.
    TagType listElementType() const
    {
        assert(isList());
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
        if (!isList())
            throw std::logic_error("Can't get element type for non-list tag.");
#endif

        return dtype_;
    }

    // @brief Initalize the element tag type of the list.
    // @attention Only be called by #TT_LIST.
    Tag& initListElementType(TagType type)
    {
        assert(isList());
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
        if (!isList())
            throw std::logic_error("Can't initialize element type for non-list tag.");

        if (dtype_ != TT_END)
            throw std::logic_error("Can't repeat initialize element type for already initialized list.");
#endif

        dtype_ = type;

        return *this;
    }

    // @brief Reset the element tag type of the list and clear all elements.
    // @attention Only be called by #TT_LIST.
    Tag& resetList()
    {
        assert(isList());
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
        if (!isList())
            throw std::logic_error("Can't reset list for non-list tag.");
#endif

        if (dtype_ == TT_END)
            return *this;

        if (data_.ld) {
            delete data_.ld;
            data_.ld = nullptr;
        }

        dtype_ = TT_END;

        return *this;
    }

    Tag assign(const Tag& tag) { return tag.copy(); }

    // @attention Only be called by #TT_LIST.
    Tag& assign(size_t size, const Tag& tag)
    {
        assert(isList());
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
        if (!isList())
            throw std::logic_error("Can't assign multiple tags to non-list tag.");

        if (dtype_ == TT_END)
            throw std::logic_error("Can't read or write a uninitialized list.");

        if (tag.type_ != dtype_) {
            String errmsg = "Can't assign the tag of " + getTagTypeString(tag.type());
            errmsg += " to the list of " + getTagTypeString(dtype_);

            throw std::logic_error(errmsg);
        }
#endif

        if (size == 0 && !data_.ld)
            return *this;

        if (!data_.ld)
            data_.ld = new Vec<Tag>();

        data_.ld->assign(size, tag);

        return *this;
    }

    /*
    * Functions about compound.
    */

    // @brief Check the compound if contains member of specified name.
    // @attention Only be called by #TT_COMPOUND.
    bool hasTag(const String& name) const
    {
        assert(isCompound());
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
        if (!isCompound())
            throw std::logic_error("Can't check tag existence for non-compound tag.");
#endif

        if (!data_.cd)
            return false;

        return data_.cd->idxs.find(name) != data_.cd->idxs.end();
    }

    /*
    * Functions about containers.
    */

    // @brief Get the length of string or size of array or tag counts of list and compound.
    // @attention Only be called by
    // #TT_STRING, #TT_BYTE_ARRAY, #TT_INT_ARRAY, #TT_LONG_ARRAY, #TT_LIST, #TT_COMPOUND.
    size_t size() const
    {
        assert(isString() || isArray() || isContainer());
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
        if (!isString() && !isArray() && !isContainer())
            throw std::logic_error("Can't get size for non-string, non-array, non-container tag.");
#endif

        if (isString())
            return !data_.str ? 0 : data_.str->size();
        if (isByteArray())
            return !data_.bad ? 0 : data_.bad->size();
        if (isIntArray())
            return !data_.iad ? 0 : data_.iad->size();
        if (isLongArray())
            return !data_.lad ? 0 : data_.lad->size();
        if (isList())
            return !data_.ld ? 0 : data_.ld->size();
        if (isCompound())
            return !data_.cd ? 0 : data_.cd->size();

        return 0;
    }

    // @brief Check whether the string or array or list or compound is empty.
    // @attention Only be called by
    // #TT_STRING, #TT_BYTE_ARRAY, #TT_INT_ARRAY, #TT_LONG_ARRAY, #TT_LIST, #TT_COMPOUND.
    bool empty() const { return size() == 0; }

    // @brief Reserve the space of string or array or list or compound.
    // @attention Only be called by
    // #TT_STRING, #TT_BYTE_ARRAY, #TT_INT_ARRAY, #TT_LONG_ARRAY, #TT_LIST, #TT_COMPOUND.
    void reserve(size_t size)
    {
        assert(isString() || isArray() || isContainer());
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
        if (!isString() && !isArray() && !isContainer())
            throw std::logic_error("Can't reserve space for non-string, non-array, non-container tag.");
#endif

        if (isString()) {
            if (!data_.str)
                data_.str = new String();

            data_.str->reserve(size);
        } else if (isByteArray()) {
            if (!data_.bad)
                data_.bad = new Vec<byte>();

            data_.bad->reserve(size);
        } else if (isIntArray()) {
            if (!data_.iad)
                data_.iad = new Vec<int32>();

            data_.iad->reserve(size);
        } else if (isLongArray()) {
            if (!data_.lad)
                data_.lad = new Vec<int64>();

            data_.lad->reserve(size);
        } else if (isList()) {
            if (!data_.ld)
                data_.ld = new Vec<Tag>();

            data_.ld->reserve(size);
        } else if (isCompound()) {
            if (!data_.cd)
                data_.cd = new CompoundData();

            data_.cd->reserve(size);
        }
    }

    /*
    * Functions for set tag's value. Only be called by corresponding tag.
    */

    // @attention Only be called by #TT_BYTE.
    Tag& setByte(byte value)
    {
        assert(isByte());
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
        if (!isByte())
            throw std::logic_error("Can't set byte value for non-byte tag.");
#endif

        data_.num.i8 = value;

        return *this;
    }

    // @attention Only be called by #TT_SHORT.
    Tag& setShort(int16 value)
    {
        assert(isShort());
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
        if (!isShort())
            throw std::logic_error("Can't set short value for non-short tag.");
#endif

        data_.num.i16 = value;

        return *this;
    }

    // @attention Only be called by #TT_INT.
    Tag& setInt(int32 value)
    {
        assert(isInt());
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
        if (!isInt())
            throw std::logic_error("Can't set int value for non-int tag.");
#endif

        data_.num.i32 = value;

        return *this;
    }

    // @attention Only be called by #TT_LONG.
    Tag& setLong(int64 value)
    {
        assert(isLong());
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
        if (!isLong())
            throw std::logic_error("Can't set long value for non-long tag.");
#endif

        data_.num.i64 = value;

        return *this;
    }

    // @attention Only be called by #TT_FLOAT.
    Tag& setFloat(fp32 value)
    {
        assert(isFloat());
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
        if (!isFloat())
            throw std::logic_error("Can't set float value for non-float tag.");
#endif

        data_.num.f32 = value;

        return *this;
    }

    // @attention Only be called by #TT_DOUBLE.
    Tag& setDouble(fp64 value)
    {
        assert(isDouble());
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
        if (!isDouble())
            throw std::logic_error("Can't set double value for non-double tag.");
#endif

        data_.num.f64 = value;

        return *this;
    }

    // @brief Fast way of set the integer value. (auto check the tag type)
    // @attention Only be called by #TT_BYTE, #TT_SHORT, #TT_INT, #TT_LONG.
    Tag& setInteger(int64 value)
    {
        assert(isInteger());
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
        if (!isInteger())
            throw std::logic_error("Can't set interger number for non-integer tag.");
#endif

        if (isByte())
            setByte(static_cast<byte>(value));
        else if (isShort())
            setShort(static_cast<int16>(value));
        else if (isInt())
            setInt(static_cast<int32>(value));
        else if (isLong())
            setLong(value);

        return *this;
    }

    // @brief Fast way of set the float point value. (auto check the tag type)
    // @attention Only be called by #TT_FLOAT, #TT_DOUBLE.
    Tag& setFloatPoint(fp64 value)
    {
        assert(isFloatPoint());
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
        if (!isFloatPoint())
            throw std::logic_error("Can't set float point number value for non-float point tag.");
#endif

        if (isFloat())
            setFloat(static_cast<fp32>(value));
        else if (isDouble())
            setDouble(value);

        return *this;
    }

    // @attention Only be called by #TT_STRING.
    Tag& setString(const String& value)
    {
        assert(isString());
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
        if (!isString())
            throw std::logic_error("Can't set string value for non-string tag.");
#endif

        if (value.empty() && !data_.str)
            return *this;

        if (data_.str)
            *data_.str = value;
        else
            data_.str = new String(value);

        return *this;
    }

    // @attention Only be called by #TT_BYTE_ARRAY.
    Tag& setByteArray(const Vec<byte>& value)
    {
        assert(isByteArray());
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
        if (!isByteArray())
            throw std::logic_error("Can't set byte array value for non-byte array tag.");
#endif

        if (value.empty() && !data_.bad)
            return *this;

        if (data_.bad)
            *data_.bad = value;
        else
            data_.bad = new Vec<byte>(value);

        return *this;
    }

    // @attention Only be called by #TT_INT_ARRAY.
    Tag& setIntArray(const Vec<int32>& value)
    {
        assert(isIntArray());
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
        if (!isIntArray())
            throw std::logic_error("Can't set int array value for non-int array tag.");
#endif

        if (value.empty() && !data_.iad)
            return *this;

        if (data_.iad)
            *data_.iad = value;
        else
            data_.iad = new Vec<int32>(value);

        return *this;
    }

    // @attention Only be called by #TT_LONG_ARRAY.
    Tag& setLongArray(const Vec<int64>& value)
    {
        assert(isLongArray());
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
        if (!isLongArray())
            throw std::logic_error("Can't set long array value for non-long array tag.");
#endif

        if (value.empty() && !data_.lad)
            return *this;

        if (data_.lad)
            *data_.lad = value;
        else
            data_.lad = new Vec<int64>(value);

        return *this;
    }

    // @attention Only be called by #TT_BYTE_ARRAY.
    Tag& setArray(const Vec<byte>& value) { return setByteArray(value); }

    // @overload
    // @attention Only be called by #TT_INT_ARRAY.
    Tag& setArray(const Vec<int32>& value) { return setIntArray(value); }

    // @overload
    // @attention Only be called by #TT_LONG_ARRAY.
    Tag& setArray(const Vec<int64>& value) { return setLongArray(value); }

    // @brief Add a value to the byte array.
    // @attention Only be called by #TT_BYTE_ARRAY.
    Tag& addByte(byte value)
    {
        assert(isByteArray());
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
        if (!isByteArray())
            throw std::logic_error("Can't add byte value to non-byte array tag.");
#endif

        if (!data_.bad)
            data_.bad = new Vec<byte>();

        data_.bad->emplace_back(value);

        return *this;
    }

    // @brief Add a value to the int array.
    // @attention Only be called by #TT_INT_ARRAY.
    Tag& addInt(int32 value)
    {
        assert(isIntArray());
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
        if (!isIntArray())
            throw std::logic_error("Can't add int value to non-int array tag.");
#endif

        if (!data_.iad)
            data_.iad = new Vec<int32>();

        data_.iad->emplace_back(value);

        return *this;
    }

    // @brief Add a value to the long array.
    // @attention Only be called by #TT_LONG_ARRAY.
    Tag& addLong(int64 value)
    {
        assert(isLongArray());
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
        if (!isLongArray())
            throw std::logic_error("Can't add long value to non-long array tag.");
#endif

        if (!data_.lad)
            data_.lad = new Vec<int64>();

        data_.lad->emplace_back(value);

        return *this;
    }

    // @brief Add a tag to the initialized list or compound.
    // @note Original tag will be moved to the new tag whether left-value or right-value reference,
    // and the original tag will invalid after this operation, but you can call #copy() function to avoid this.
    // @attention Only be called by #TT_LIST, #TT_COMPOUND.
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

        if (isList()) {
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
            if (dtype_ == TT_END)
                throw std::logic_error("Can't read or write a uninitialized list.");

            if (tag.type_ != dtype_) {
                String errmsg = "Can't add the tag of " + getTagTypeString(tag.type());
                errmsg += " to the list of " + getTagTypeString(dtype_);

                throw std::logic_error(errmsg);
            }
#endif

            if (!data_.ld)
                data_.ld = new Vec<Tag>();

            bool needShuffle = (data_.ld->capacity() - data_.ld->size()) == 0;
            data_.ld->emplace_back(std::move(tag));

            if (needShuffle)
                for (auto& var : *data_.ld) var.parent_ = this;
            else
                data_.ld->back().parent_ = this;

            if (data_.ld->back().name_) {
                delete data_.ld->back().name_;
                data_.ld->back().name_ = nullptr;
            }
        } else if (isCompound()) {
            if (!data_.cd)
                data_.cd = new CompoundData();

            if (hasTag(tag.name())) {
                data_.cd->data[data_.cd->idxs[tag.name()]] = std::move(tag);
            } else {
                bool needShuffle = (data_.cd->data.capacity() - data_.cd->size()) == 0;
                data_.cd->data.emplace_back(std::move(tag));
                data_.cd->idxs.insert({ data_.cd->data.back().name(), data_.cd->data.size() - 1 });

                if (needShuffle)
                    for (auto& var : data_.cd->data) var.parent_ = this;
                else
                    data_.cd->data.back().parent_ = this;
            }
        }

        return *this;
    }

    // @overload
    Tag& addTag(Tag& tag) { return addTag(std::move(tag)); }

    /*
    * Functions for get value. Only be called by corresponding tag.
    */

    // @attention Only be called by #TT_BYTE.
    byte getByte() const
    {
        assert(isByte());
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
        if (!isByte())
            throw std::logic_error("Can't get byte value for non-byte tag.");
#endif

        return data_.num.i8;
    }

    // @attention Only be called by #TT_SHORT.
    int16 getShort() const
    {
        assert(isShort());
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
        if (!isShort())
            throw std::logic_error("Can't get short value for non-short tag.");
#endif

        return data_.num.i16;
    }

    // @attention Only be called by #TT_INT.
    int32 getInt() const
    {
        assert(isInt());
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
        if (!isInt())
            throw std::logic_error("Can't get int value for non-int tag.");
#endif

        return data_.num.i32;
    }

    // @attention Only be called by #TT_LONG.
    int64 getLong() const
    {
        assert(isLong());
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
        if (!isLong())
            throw std::logic_error("Can't get long value for non-long tag.");
#endif

        return data_.num.i64;
    }

    // @attention Only be called by #TT_FLOAT.
    fp32 getFloat() const
    {
        assert(isFloat());
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
        if (!isFloat())
            throw std::logic_error("Can't get float value for non-float tag.");
#endif

        return data_.num.f32;
    }

    // @attention Only be called by #TT_DOUBLE.
    fp64 getDouble() const
    {
        assert(isDouble());
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
        if (!isDouble())
            throw std::logic_error("Can't get double value for non-double tag.");
#endif

        return data_.num.f64;
    }

    // @brief Fast way of get the integer value. (auto check the tag type)
    // @attention Only be called by #TT_BYTE, #TT_SHORT, #TT_INT, #TT_LONG.
    int64 getInteger() const
    {
        assert(isInteger());
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
        if (!isInteger())
            throw std::logic_error("Can't get interger number for non-integer tag.");
#endif

        if (isByte())
            return data_.num.i8;
        if (isShort())
            return data_.num.i16;
        if (isInt())
            return data_.num.i32;
        if (isLong())
            return data_.num.i64;

        return 0;
    }

    // @brief Fast way of get the float point value. (auto check the tag type)
    // @attention Only be called by #TT_FLOAT, #TT_DOUBLE.
    fp64 getFloatPoint() const
    {
        assert(isFloatPoint());
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
        if (!isFloatPoint())
            throw std::logic_error("Can't get float point number value for non-float point tag.");
#endif

        if (isFloat())
            return data_.num.f32;
        else if (isDouble())
            return data_.num.f64;

        return 0;
    }

    // @attention Only be called by #TT_STRING.
    String getString() const
    {
        assert(isString());
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
        if (!isString())
            throw std::logic_error("Can't get string value for non-string tag.");
#endif

        if (!data_.str)
            return String();

        return *data_.str;
    }

    // @attention Only be called by #TT_BYTE_ARRAY.
    Vec<byte> getByteArray() const
    {
        assert(isByteArray());
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
        if (!isByteArray())
            throw std::logic_error("Can't get byte array value for non-byte array tag.");
#endif

        if (!data_.bad)
            return Vec<byte>();

        return *data_.bad;
    }

    // @attention Only be called by #TT_INT_ARRAY.
    Vec<int32> getIntArray() const
    {
        assert(isIntArray());
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
        if (!isIntArray())
            throw std::logic_error("Can't get int array value for non-int array tag.");
#endif

        if (!data_.iad)
            return Vec<int32>();

        return *data_.iad;
    }

    // @attention Only be called by #TT_LONG_ARRAY.
    Vec<int64> getLongArray() const
    {
        assert(isLongArray());
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
        if (!isLongArray())
            throw std::logic_error("Can't get long array value for non-long array tag.");
#endif

        if (!data_.lad)
            return Vec<int64>();

        return *data_.lad;
    }

    // @overload
    // @brief Get a value from the byte array by index.
    // @attention Only be called by #TT_BYTE_ARRAY.
    byte getByte(size_t idx) const
    {
        assert(isByteArray());
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
        if (!isByteArray())
            throw std::logic_error("Can't get byte value from non-byte array tag.");
#endif

        if (!data_.bad || idx >= data_.bad->size())
            throw std::out_of_range("The specified index is out of range.");

        return (*data_.bad)[idx];
    }

    // @attention Only be called by #TT_BYTE_ARRAY.
    byte getFrontByte() const
    {
        assert(isByteArray());
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
        if (!isByteArray())
            throw std::logic_error("Can't get front byte value from non-byte array tag.");
#endif

        if (!data_.bad || data_.bad->empty())
            throw std::out_of_range("The front member is not exists.");

        return data_.bad->front();
    }

    // @attention Only be called by #TT_BYTE_ARRAY.
    byte getBackByte() const
    {
        assert(isByteArray());
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
        if (!isByteArray())
            throw std::logic_error("Can't get back byte value from non-byte array tag.");
#endif

        if (!data_.bad || data_.bad->empty())
            throw std::out_of_range("The back member is not exits.");

        return data_.bad->back();
    }

    // @overload
    // @brief Get a value from the int array by index.
    // @attention Only be called by #TT_INT_ARRAY.
    int32 getInt(size_t idx) const
    {
        assert(isIntArray());
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
        if (!isIntArray())
            throw std::logic_error("Can't get int value from non-int array tag.");
#endif

        if (!data_.iad || idx >= data_.iad->size())
            throw std::out_of_range("The specified index is out of range.");

        return (*data_.iad)[idx];
    }

    // @attention Only be called by #TT_INT_ARRAY.
    int32 getFrontInt() const
    {
        assert(isIntArray());
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
        if (!isIntArray())
            throw std::logic_error("Can't get front int value from non-int array tag.");
#endif

        if (!data_.iad || data_.iad->empty())
            throw std::out_of_range("The front member is not exists.");

        return data_.iad->front();
    }

    // @attention Only be called by #TT_INT_ARRAY.
    int32 getBackInt() const
    {
        assert(isIntArray());
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
        if (!isIntArray())
            throw std::logic_error("Can't get back int value from non-int array tag.");
#endif

        if (!data_.iad || data_.iad->empty())
            throw std::out_of_range("The back member is not exits.");

        return data_.iad->back();
    }

    // @overload
    // @brief Get a value from long array by index.
    // @attention Only be called by #TT_LONG_ARRAY.
    int64 getLong(size_t idx) const
    {
        assert(isLongArray());
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
        if (!isLongArray())
            throw std::logic_error("Can't get long value from non-long array tag.");
#endif

        if (!data_.lad || idx >= data_.lad->size())
            throw std::out_of_range("The specified index is out of range.");

        return (*data_.lad)[idx];
    }

    // @attention Only be called by #TT_LONG_ARRAY.
    int64 getFrontLong() const
    {
        assert(isLongArray());
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
        if (!isLongArray())
            throw std::logic_error("Can't get front long value from non-long array tag.");
#endif

        if (!data_.lad || data_.lad->empty())
            throw std::out_of_range("The front member is not exists.");

        return data_.lad->front();
    }

    // @attention Only be called by #TT_LONG_ARRAY.
    int64 getBackLong() const
    {
        assert(isLongArray());
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
        if (!isLongArray())
            throw std::logic_error("Can't get back long value from non-long array tag.");
#endif

        if (!data_.lad || data_.lad->empty())
            throw std::out_of_range("The back member is not exits.");

        return data_.lad->back();
    }

    // @brief Get the tag by index.
    // @attention Only be called by #TT_LIST, #TT_COMPOUND.
    Tag& getTag(size_t idx)
    {
        assert(isContainer());
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
        if (!isContainer())
            throw std::logic_error("Can't get tag from non-container tag.");
#endif

        if (isList()) {
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
            if (dtype_ == TT_END)
                throw std::logic_error("Can't read or write a uninitialized list.");
#endif

            if (!data_.ld || idx >= data_.ld->size())
                throw std::out_of_range("The specified index is out of range.");

            return (*data_.ld)[idx];
        } else if (isCompound()) {
            if (!data_.cd || idx >= data_.cd->size())
                throw std::out_of_range("The specified index is out of range.");

            return data_.cd->data[idx];
        }

        return *this;
    }

    // @overload
    // @brief Get the tag by name.
    // @attention Only be called by #TT_COMPOUND.
    Tag& getTag(const String& name)
    {
        assert(isCompound());
        assert(hasTag(name));
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
        if (!isCompound())
            throw std::logic_error("Can't get tag from non-compound tag.");

        if (!hasTag(name))
            throw std::logic_error("The specified name is not exists.");

        if (!data_.cd)
            throw std::logic_error("The member of specified name is not exists.");
#endif

        return data_.cd->data[data_.cd->idxs[name]];
    }

    // @attention Only be called by #TT_LIST, #TT_COMPOUND.
    Tag& getFrontTag()
    {
        assert(isContainer());
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
        if (!isContainer())
            throw std::logic_error("Can't get front tag from non-container tag.");
#endif

        if (isList()) {
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
            if (dtype_ == TT_END)
                throw std::logic_error("Can't read or write a uninitialized list.");
#endif

            if (!data_.ld || data_.ld->empty())
                throw std::out_of_range("The front member is not exists.");

            return data_.ld->front();
        } else if (isCompound()) {
            if (!data_.cd || data_.cd->empty())
                throw std::out_of_range("The front member is not exists.");

            return data_.cd->data.front();
        }

        return *this;
    }

    // @attention Only be called by #TT_LIST, #TT_COMPOUND.
    Tag& getBackTag()
    {
        assert(isContainer());
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
        if (!isContainer())
            throw std::logic_error("Can't get back tag from non-container tag.");
#endif

        if (isList()) {
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
            if (dtype_ == TT_END)
                throw std::logic_error("Can't read or write a uninitialized list.");
#endif

            if (!data_.ld || data_.ld->empty())
                throw std::out_of_range("The back member is not exists.");

            return data_.ld->back();
        } else if (isCompound()) {
            if (!data_.cd || data_.cd->empty())
                throw std::out_of_range("The back member is not exists.");

            return data_.cd->data.back();
        }

        return *this;
    }

    // @brief Remove the element by index.
    // @attention Only be called by
    // #TT_STRING, #TT_BYTE_ARRAY, #TT_INT_ARRAY, #TT_LONG_ARRAY, #TT_LIST, #TT_COMPOUND.
    Tag& remove(size_t idx)
    {
        assert(isString() || isArray() || isContainer());
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
        if (!isString() && !isArray() && !isContainer())
            throw std::logic_error("Can't remove element from non-string, non-array, non-container tag.");
#endif

        if (isString()) {
            if (!data_.str || idx >= data_.str->size())
                throw std::out_of_range("The specified index is out of range.");

            data_.str->erase(data_.str->begin() + idx);
        } else if (isByteArray()) {
            if (!data_.bad || idx >= data_.bad->size())
                throw std::out_of_range("The specified index is out of range.");

            data_.bad->erase(data_.bad->begin() + idx);
        } else if (isIntArray()) {
            if (!data_.iad || idx >= data_.iad->size())
                throw std::out_of_range("The specified index is out of range.");

            data_.iad->erase(data_.iad->begin() + idx);
        } else if (isLongArray()) {
            if (!data_.lad || idx >= data_.lad->size())
                throw std::out_of_range("The specified index is out of range.");

            data_.lad->erase(data_.lad->begin() + idx);
        } else if (isList()) {
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
            if (dtype_ == TT_END)
                throw std::logic_error("Can't read or write a uninitialized list.");
#endif

            if (!data_.ld || idx >= data_.ld->size())
                throw std::out_of_range("The specified index is out of range.");

            data_.ld->erase(data_.ld->begin() + idx);
        } else if (isCompound()) {
            if (!data_.cd || idx >= data_.cd->size())
                throw std::out_of_range("The specified index is out of range.");

            data_.cd->idxs.erase(data_.cd->data[idx].name());
            data_.cd->data.erase(data_.cd->data.begin() + idx);

            for (auto& var : data_.cd->idxs)
                if (var.second > idx) var.second--;
        }

        return *this;
    }

    // @overload
    // @brief Remove the tag by name.
    // @attention Only be called by #TT_COMPOUND.
    Tag& remove(const String& name)
    {
        assert(isCompound());
        assert(hasTag(name));
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
        if (!isCompound())
            throw std::logic_error("Can't remove tag from non-compound tag.");

        if (!hasTag(name))
            throw std::logic_error("The specified name is not exists.");

        if (!data_.cd)
            throw std::logic_error("The member of specified name is not exists.");
#endif

        size_t idx = data_.cd->idxs[name];

        data_.cd->data.erase(data_.cd->data.begin() + idx);
        data_.cd->idxs.erase(name);

        for (auto& var : data_.cd->idxs)
            if (var.second > idx) var.second--;

        return *this;
    }

    // @brief Remove the first element.
    // @attention Only be called by
    // #TT_STRING, #TT_BYTE_ARRAY, #TT_INT_ARRAY, #TT_LONG_ARRAY, #TT_LIST, #TT_COMPOUND.
    Tag& removeFront()
    {
        assert(isString() || isArray() || isContainer());
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
        if (!isString() && !isArray() && !isContainer())
            throw std::logic_error("Can't remove front element from non-string, non-array, non-container tag.");
#endif

        if (isString()) {
            if (!data_.str || data_.str->empty())
                throw std::out_of_range("The front member is not exists.");

            data_.str->erase(data_.str->begin());
        } else if (isByteArray()) {
            if (!data_.bad || data_.bad->empty())
                throw std::out_of_range("The front member is not exists.");

            data_.bad->erase(data_.bad->begin());
        } else if (isIntArray()) {
            if (!data_.iad || data_.iad->empty())
                throw std::out_of_range("The front member is not exists.");

            data_.iad->erase(data_.iad->begin());
        } else if (isLongArray()) {
            if (!data_.lad || data_.lad->empty())
                throw std::out_of_range("The front member is not exists.");

            data_.lad->erase(data_.lad->begin());
        } else if (isList()) {
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
            if (dtype_ == TT_END)
                throw std::logic_error("Can't read or write a uninitialized list.");
#endif

            if (!data_.ld || data_.ld->empty())
                throw std::out_of_range("The front member is not exists.");

            data_.ld->erase(data_.ld->begin());
        } else if (isCompound()) {
            if (!data_.cd || data_.cd->empty())
                throw std::out_of_range("The front member is not exists.");

            data_.cd->idxs.erase(data_.cd->data.front().name());
            data_.cd->data.erase(data_.cd->data.begin());

            for (auto& var : data_.cd->idxs)
                var.second--;
        }

        return *this;
    }

    // @brief Remove the last element.
    // @attention Only be called by
    // #TT_STRING, #TT_BYTE_ARRAY, #TT_INT_ARRAY, #TT_LONG_ARRAY, #TT_LIST, #TT_COMPOUND.
    Tag& removeBack()
    {
        assert(isString() || isArray() || isContainer());
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
        if (!isString() && !isArray() && !isContainer())
            throw std::logic_error("Can't remove back element from non-string, non-array, non-container tag.");
#endif

        if (isString()) {
            if (!data_.str || data_.str->empty())
                throw std::out_of_range("The back member is not exists.");

            data_.str->pop_back();
        } else if (isByteArray()) {
            if (!data_.bad || data_.bad->empty())
                throw std::out_of_range("The back member is not exists.");

            data_.bad->pop_back();
        } else if (isIntArray()) {
            if (!data_.iad || data_.iad->empty())
                throw std::out_of_range("The back member is not exists.");

            data_.iad->pop_back();
        } else if (isLongArray()) {
            if (!data_.lad || data_.lad->empty())
                throw std::out_of_range("The back member is not exists.");

            data_.lad->pop_back();
        } else if (isList()) {
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
            if (dtype_ == TT_END)
                throw std::logic_error("Can't read or write a uninitialized list.");
#endif

            if (!data_.ld || data_.ld->empty())
                throw std::out_of_range("The back member is not exists.");

            data_.ld->pop_back();
        } else if (isCompound()) {
            if (!data_.cd || data_.cd->empty())
                throw std::out_of_range("The back member is not exists.");

            data_.cd->idxs.erase(data_.cd->data.back().name());
            data_.cd->data.pop_back();
        }

        return *this;
    }

    // @brief Remove the all elements.
    // @attention Only be called by
    // #TT_STRING, #TT_BYTE_ARRAY, #TT_INT_ARRAY, #TT_LONG_ARRAY, #TT_LIST, #TT_COMPOUND.
    Tag& removeAll()
    {
        assert(isString() || isArray() || isContainer());
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
        if (!isString() && !isArray() && !isContainer())
            throw std::logic_error("Can't remove all elements from non-string, non-array, non-container tag.");
#endif

        if (isString()) {
            if (data_.str)
                data_.str->clear();
        } else if (isByteArray()) {
            if (data_.bad)
                data_.bad->clear();
        } else if (isIntArray()) {
            if (data_.iad)
                data_.iad->clear();
        } else if (isLongArray()) {
            if (data_.lad)
                data_.lad->clear();
        } else if (isList()) {
#if !defined(_DEBUG) && !defined(MCNBT_DISABLE_LOGIC_EXCEPTION)
            if (dtype_ == TT_END)
                throw std::logic_error("Can't read or write a uninitialized list.");
#endif

            if (data_.ld)
                data_.ld->clear();
        } else if (isCompound()) {
            if (data_.cd)
                data_.cd->clear();
        }

        return *this;
    }

#ifdef MCNBT_USE_GZIP
    // @brief Write the tag to output stream.
    void write(OStream& os, bool isBigEndian, bool isCompressed = false) const
    {
        if (isCompressed) {
            SStream ss;
            write_(ss, isBigEndian, (isListElement()));
            os << gzip::compress(ss.str());
        } else {
            write_(os, isBigEndian, (isListElement()));
        }
    }

    // @overload
    void write(const String& filename, bool isBigEndian, bool isCompressed = false) const
    {
        OFStream ofs(filename, std::ios_base::binary);

        if (ofs.is_open())
            write(ofs, isBigEndian, isCompressed);
        else
            throw std::runtime_error("Failed to open file: " + filename);

        ofs.close();
    }
#else
    // @brief Write the tag to output stream.
    void write(OStream& os, bool isBigEndian) const { write_(os, isBigEndian, (isListElement())); }

    // @overload
    void write(const String& filename, bool isBigEndian) const
    {
        OFStream ofs(filename, std::ios_base::binary);

        if (ofs.is_open())
            write(ofs, isBigEndian);
        else
            throw std::runtime_error("Failed to open file: " + filename);

        ofs.close();
    }
#endif // MCNBT_USE_GZIP

    // @brief Get the SNBT (The string representation of NBT).
    // @param isWrappedIndented If true, the output string will be wrapped and indented.
    String toSnbt(bool isWrappedIndented = true) const { return toSnbt_(isWrappedIndented, (isListElement())); }

    /*
    * Operators overloading.
    */

    // @brief Fast way of get the tag by index.
    Tag& operator[](size_t idx) { return getTag(idx); }

    // @overload
    // @brief Fast way of get the tag by name.
    Tag& operator[](const String& name) { return getTag(name); }

    // @brief Fast way of add the tag.
    Tag& operator<<(Tag&& tag) { return addTag(std::move(tag)); }

    // @overload
    Tag& operator<<(Tag& tag) { return addTag(tag); }

private:
    // Nums.
    // Contains interger and float point number.
    union Num
    {
        Num() : i64(0) {}
        byte i8;
        int16 i16;
        int32 i32;
        int64 i64;
        fp32 f32;
        fp64 f64;
    };

    // A simple wrapper of std::vector<tag> and std::map<string, size_t>.
    struct CompoundData
    {
        Vec<Tag> data;
        Map<String, size_t> idxs;

        bool empty() const { return data.empty(); }

        size_t size() const { return data.size(); }

        void reserve(size_t size)
        {
            data.reserve(size);
            idxs.reserve(size);
        }

        void clear()
        {
            data.clear();
            idxs.clear();
        }
    };

    // Value of tag.
    // Individual tag is like key-value pair.
    // The key is the name of tag (can be empty, and all list element not has name).
    // The value is stored in the following union.
    union Data
    {
        Data() : num(Num()) {}
        // Number data
        Num num;
        // String data
        String* str;
        // Byte Array data
        Vec<byte>* bad;
        // Int Array data
        Vec<int32>* iad;
        // Long Array data
        Vec<int64>* lad;
        // List data
        Vec<Tag>* ld;
        // Compound data
        CompoundData* cd;
    };

    // @param tagType If the param isListElement is false, ignore it.
    // If the param isListElement is true, the tagType must be set to the same as the parent tag.
    static Tag fromBinStream_(IStream& is, bool isBigEndian, bool isListElement, int tagType = -1)
    {
        Tag tag;

        if (!isListElement)
            tagType = is.get();
        tag.type_ = static_cast<TagType>(tagType);

        if (tagType == TT_END)
            return tag;

        // If the tag is not a list elment read it's name.
        if (!isListElement) {
            int16 nameLen = _bytes2num<int16>(is, isBigEndian);
            if (nameLen != 0) {
                byte* bytes = new byte[nameLen];

                is.read(bytes, nameLen);
                tag.name_ = new String();
                tag.name_->assign(bytes, static_cast<size_t>(is.gcount()));

                delete[] bytes;
            }
        }

        // Read value.
        if (tagType == TT_BYTE)
            tag.data_.num.i8 = _bytes2num<byte>(is, isBigEndian);

        if (tagType == TT_SHORT)
            tag.data_.num.i16 = _bytes2num<int16>(is, isBigEndian);

        if (tagType == TT_INT)
            tag.data_.num.i32 = _bytes2num<int32>(is, isBigEndian);

        if (tagType == TT_LONG)
            tag.data_.num.i64 = _bytes2num<int64>(is, isBigEndian);

        if (tagType == TT_FLOAT)
            tag.data_.num.f32 = _bytes2num<fp32>(is, isBigEndian);

        if (tagType == TT_DOUBLE)
            tag.data_.num.f64 = _bytes2num<fp64>(is, isBigEndian);

        if (tagType == TT_STRING) {
            int16 strlen = _bytes2num<int16>(is, isBigEndian);

            if (strlen != 0) {
                byte* bytes = new byte[strlen];

                is.read(bytes, strlen);
                tag.data_.str = new String();
                tag.data_.str->assign(bytes, static_cast<size_t>(is.gcount()));

                delete[] bytes;
            }
        }

        if (tagType == TT_BYTE_ARRAY) {
            int32 dsize = _bytes2num<int32>(is, isBigEndian);

            if (dsize != 0) {
                tag.data_.bad = new Vec<byte>();
                tag.data_.bad->reserve(dsize);

                for (int32 i = 0; i < dsize; ++i)
                    tag.addByte(_bytes2num<byte>(is, isBigEndian));
            }
        }

        if (tagType == TT_INT_ARRAY) {
            int32 dsize = _bytes2num<int32>(is, isBigEndian);

            if (dsize != 0) {
                tag.data_.iad = new Vec<int32>();
                tag.data_.iad->reserve(dsize);

                for (int32 i = 0; i < dsize; ++i)
                    tag.addInt(_bytes2num<int32>(is, isBigEndian));
            }
        }

        if (tagType == TT_LONG_ARRAY) {
            int32 dsize = _bytes2num<int32>(is, isBigEndian);

            if (dsize != 0) {
                tag.data_.lad = new Vec<int64>();
                tag.data_.lad->reserve(dsize);

                for (int32 i = 0; i < dsize; ++i)
                    tag.addLong(_bytes2num<int64>(is, isBigEndian));
            }
        }

        if (tagType == TT_LIST) {
            tag.dtype_ = static_cast<TagType>(is.get());
            int32 dsize = _bytes2num<int32>(is, isBigEndian);

            if (dsize != 0) {
                tag.data_.ld = new Vec<Tag>();
                tag.data_.ld->reserve(dsize);

                for (int32 i = 0; i < dsize; ++i)
                    tag.addTag(fromBinStream_(is, isBigEndian, true, tag.dtype_));
            }
        }

        if (tagType == TT_COMPOUND) {
            while (!is.eof()) {
                if (is.peek() == TT_END) {
                    // Give up End tag and move stream point to next byte.
                    is.get();
                    break;
                }

                tag.addTag(fromBinStream_(is, isBigEndian, false));
            }
        }

        return tag;
    }

    // TODO
    static Tag fromSnbt_(SStream& snbtSs);

    void write_(OStream& os, bool isBigEndian, bool isListElement) const
    {
        if (!isListElement) {
            os.put(static_cast<byte>(type_));

            if (!name_ || name_->empty()) {
                _num2bytes<int16>(static_cast<int16>(0), os, isBigEndian);
            } else {
                _num2bytes<int16>(static_cast<int16>(name_->size()), os, isBigEndian);
                os.write(name_->c_str(), name_->size());
            }
        }

        if (isEnd()) {
            os.put(TT_END);
            return;
        }

        if (isByte()) {
            os.put(data_.num.i8);
            return;
        }

        if (isShort()) {
            _num2bytes<int16>(data_.num.i16, os, isBigEndian);
            return;
        }

        if (isInt()) {
            _num2bytes<int32>(data_.num.i32, os, isBigEndian);
            return;
        }

        if (isLong()) {
            _num2bytes<int64>(data_.num.i64, os, isBigEndian);
            return;
        }

        if (isFloat()) {
            _num2bytes<fp32>(data_.num.f32, os, isBigEndian);
            return;
        }

        if (isDouble()) {
            _num2bytes<fp64>(data_.num.f64, os, isBigEndian);
            return;
        }

        if (isString()) {
            if (!data_.str || data_.str->empty()) {
                _num2bytes<int16>(static_cast<int16>(0), os, isBigEndian);
                return;
            }

            _num2bytes<int16>(static_cast<int16>(data_.str->size()), os, isBigEndian);
            os.write(data_.str->c_str(), data_.str->size());

            return;
        }

        if (isByteArray()) {
            if (!data_.bad || data_.bad->empty()) {
                _num2bytes<int32>(static_cast<int32>(0), os, isBigEndian);
                return;
            }

            _num2bytes<int32>(static_cast<int32>(data_.bad->size()), os, isBigEndian);

            for (const auto& var : *data_.bad)
                os.put(var);

            return;
        }

        if (isIntArray()) {
            if (!data_.iad || data_.iad->empty()) {
                _num2bytes<int32>(static_cast<int32>(0), os, isBigEndian);
                return;
            }

            _num2bytes<int32>(static_cast<int32>(data_.iad->size()), os, isBigEndian);

            for (const auto& var : *data_.iad)
                _num2bytes<int32>(var, os, isBigEndian);

            return;
        }

        if (isLongArray()) {
            if (!data_.lad || data_.lad->empty()) {
                _num2bytes<int32>(static_cast<int32>(0), os, isBigEndian);
                return;
            }

            _num2bytes<int32>(static_cast<int32>(data_.lad->size()), os, isBigEndian);

            for (const auto& var : *data_.lad)
                _num2bytes<int64>(var, os, isBigEndian);

            return;
        }

        if (isList()) {
            if (!data_.ld || data_.ld->empty()) {
                os.put(static_cast<byte>(TT_END));
                _num2bytes<int32>(static_cast<int32>(0), os, isBigEndian);
                return;
            }

            os.put(static_cast<byte>(dtype_));
            _num2bytes<int32>(static_cast<int32>(data_.ld->size()), os, isBigEndian);

            for (const auto& var : *data_.ld)
                var.write_(os, isBigEndian, true);

            return;
        }

        if (isCompound()) {
            if (!data_.cd || data_.cd->data.empty()) {
                os.put(TT_END);
                return;
            }

            for (const auto& var : data_.cd->data)
                var.write_(os, isBigEndian, false);

            os.put(TT_END);

            return;
        }
    }

    String toSnbt_(bool isWrappedIndented, bool isListElement) const
    {
        static size_t indentCount = 0;
        String inheritedIndentStr(indentCount * _SNBT_INDENT_WIDTH, ' ');

        String key = isWrappedIndented ? inheritedIndentStr : "";

        if (!isListElement && name_ && !name_->empty())
            key += *name_ + (isWrappedIndented ? ": " : ":");

        if (isEnd())
            return "";

        if (isByte())
            return key + std::to_string(static_cast<int>(data_.num.i8)) + 'b';

        if (isShort())
            return key + std::to_string(static_cast<int>(data_.num.i16)) + 's';

        if (isInt())
            return key + std::to_string(data_.num.i32);

        if (isLong())
            return key + std::to_string(data_.num.i64) + 'l';

        if (isFloat())
            return key + std::to_string(data_.num.f32) + 'f';

        if (isDouble())
            return key + std::to_string(data_.num.f64) + 'd';

        if (isString())
            return key + '"' + (data_.str ? *data_.str : "") + '"';

        if (isByteArray()) {
            if (!data_.bad || data_.bad->empty())
                return key + "[B;]";

            // If has indent add the newline character and indent string.
            String result = key + '[';
            result += isWrappedIndented ? ('\n' + inheritedIndentStr + _SNBT_INDENT_STR) : "";
            result += "B;";

            for (const auto& var : *data_.bad) {
                result += isWrappedIndented ? ('\n' + inheritedIndentStr + _SNBT_INDENT_STR) : "";
                result += std::to_string(static_cast<int>(var)) + "b,";
            }

            if (result.back() == ',')
                result.pop_back();

            result += isWrappedIndented ? ('\n' + inheritedIndentStr) : "";
            result += ']';

            return result;
        }

        if (isIntArray()) {
            if (!data_.iad || data_.iad->empty())
                return key + "[I;]";

            // If has indent add the newline character and indent string.
            String result = key + '[';
            result += isWrappedIndented ? ('\n' + inheritedIndentStr + _SNBT_INDENT_STR) : "";
            result += "I;";

            for (const auto& var : *data_.iad) {
                result += isWrappedIndented ? ('\n' + inheritedIndentStr + _SNBT_INDENT_STR) : "";
                result += std::to_string(var) + ",";
            }

            if (result.back() == ',')
                result.pop_back();

            result += isWrappedIndented ? ('\n' + inheritedIndentStr) : "";
            result += ']';

            return result;
        }

        if (isLongArray()) {
            if (!data_.lad || data_.lad->empty())
                return key + "[L;]";

            // If has indent add the newline character and indent string.
            String result = key + '[';
            result += isWrappedIndented ? ('\n' + inheritedIndentStr + _SNBT_INDENT_STR) : "";
            result += "L;";

            for (const auto& var : *data_.lad) {
                result += isWrappedIndented ? ('\n' + inheritedIndentStr + _SNBT_INDENT_STR) : "";
                result += std::to_string(var) + "l,";
            }

            if (result.back() == ',')
                result.pop_back();

            result += isWrappedIndented ? ('\n' + inheritedIndentStr) : "";
            result += ']';

            return result;
        }

        if (isList()) {
            if (!data_.ld || data_.ld->empty())
                return key + "[]";

            String result = key + "[";

            indentCount++;
            for (const auto& var : *data_.ld) {
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

        if (isCompound()) {
            if (!data_.cd || data_.cd->data.empty())
                return key + "{}";

            String result = key + "{";

            indentCount++;
            for (const auto& var : data_.cd->data) {
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

        throw std::runtime_error("Read the tag of undefined tag typer.");
    }

    // Release the all alloced memory.
    // (e.g. name, string, byte array, int array, long array, list, compound)
    void release_()
    {
        if (isString() && data_.str)
            delete data_.str;
        else if (isByteArray() && data_.bad)
            delete data_.bad;
        else if (isIntArray() && data_.iad)
            delete data_.iad;
        else if (isLongArray() && data_.lad)
            delete data_.lad;
        else if (isList() && data_.ld)
            delete data_.ld;
        else if (isCompound() && data_.cd)
            delete data_.cd;

        data_.str = nullptr;

        if (name_) {
            delete name_;
            name_ = nullptr;
        }
    }

    // Tag type.
    TagType type_ = TT_END;
    // Tag type of the list element. Just usefull for list tag.
    TagType dtype_ = TT_END;
    // Tag value (value of key-value pair).
    Data data_;
    // Tag name (key of key-value pair), Just usefull for non-ListElement tag.
    String* name_ = nullptr;
    // Parent tag (list or compound tag).
    Tag* parent_ = nullptr;
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
