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

#ifndef MCNBT_HPP
#define MCNBT_HPP

// Whether to use GZip to un/compress MCNBT.
// #define MCNBT_USE_GZIP

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

using Str = std::string;

template<typename T>
using Vec = std::vector<T>;

template<typename K, typename V, typename Hash, typename KEqual>
using Map = std::unordered_map<K, V, Hash, KEqual>;

}

// Enum, constants and aux functions.
namespace nbt
{

// NBT tag types.
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

inline Str getTagTypeString(TagType type)
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

inline bool isEnd(TagType type) { return type == TT_END; }

inline bool isByte(TagType type) { return type == TT_BYTE; }

inline bool isShort(TagType type) { return type == TT_SHORT; }

inline bool isInt(TagType type) { return type == TT_INT; }

inline bool isLong(TagType type) { return type == TT_LONG; }

inline bool isFloat(TagType type) { return type == TT_FLOAT; }

inline bool isDouble(TagType type) { return type == TT_DOUBLE; }

inline bool isString(TagType type) { return type == TT_STRING; }

inline bool isByteArray(TagType type) { return type == TT_BYTE_ARRAY; }

inline bool isIntArray(TagType type) { return type == TT_INT_ARRAY; }

inline bool isLongArray(TagType type) { return type == TT_LONG_ARRAY; }

inline bool isList(TagType type) { return type == TT_LIST; }

inline bool isCompound(TagType type) { return type == TT_COMPOUND; }

inline bool isInteger(TagType type) { return isByte(type) || isShort(type) || isInt(type) || isLong(type); }

inline bool isFloatPoint(TagType type) { return isFloat(type) || isDouble(type); }

inline bool isNum(TagType type) { return isInteger(type) || isFloatPoint(type); }

inline bool isArray(TagType type) { return isByteArray(type) || isIntArray(type) || isLongArray(type); }

inline bool isContainer(TagType type) { return isList(type) || isCompound(type); }

// Error messages.
constexpr const char* _ERR_UNDEFINED_TT =
"Undefined tag type. (Usually caused by read a invalid tag type from input stream.)";
constexpr const char* _ERR_INCORRECT_TT =
"Incorrect tag type. (Usually caused by call a function that is not support the current tag type.)";
constexpr const char* _ERR_OUT_OF_RANGE =
"The specified index is out of range.";
constexpr const char* _ERR_EMPTY_CONTAINER =
"The container is empty and #front()/#back() etc. can not be call.";
constexpr const char* _ERR_NO_SPECIFY_MEMBER =
"No member of the specified name.";
constexpr const char* _ERR_READ_WRITE_UNINIT_LIST =
"Read/Write an uninitialized list.";
constexpr const char* _ERR_REPEAT_INIT_TT_LIST =
"Repeat initialize list.";
constexpr const char* _ERR_OTHER =
"Other error.";

// About of the indent of snbt.
constexpr size_t _INDENT_SIZE = 2;
static const Str _INDENT_STR(_INDENT_SIZE, ' ');

}

// Core of read and write binary data.
namespace nbt
{

// @brief Reverse a C style string.
// @param size The size of need reversed range, and reverse all if the size is 0.
inline void _reverse(char* str, size_t size = 0)
{
    if (size == 0)
        size = std::strlen(str);

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
template<typename T>
T _bytes2num(std::istream& is, bool isBigEndian = false, bool resumeCursor = false)
{
    size_t size = sizeof(T);
    T result = T();

    auto begpos = is.tellg();

    static byte buffer[sizeof(T)] = {};
    is.read(buffer, size);

    size = static_cast<size_t>(is.gcount());

    if (isBigEndian != _isBigEndian())
        _reverse(buffer, size);

    std::memcpy(&result, buffer, size);

    if (resumeCursor)
        is.seekg(begpos);

    return result;
}

// @brief Convert the number to bytes, and write it to output stream.
template<typename T>
void _num2bytes(T num, std::ostream& os, bool isBigEndian = false)
{
    size_t size = sizeof(T);

    static byte buffer[sizeof(T)] = {};

    std::memcpy(buffer, &num, size);

    if (isBigEndian != _isBigEndian())
        _reverse(buffer, size);

    os.write(buffer, size);
}

}

// Main.
namespace nbt
{

class Tag
{
public:
    class ListData
    {
    public:
        ListData() = default;

        size_t size() const { return data_.size(); }

        bool empty() const { return data_.empty(); }

        void reserve(size_t size) { data_.reserve(size); }

        void pushBack(Tag&& tag)
        {
            data_.emplace_back(std::move(tag));

            data_.back().isListElement_ = true;
            if (data_.back().name_) {
                delete data_.back().name_;
                data_.back().name_ = nullptr;
            }
        }

        void pushBack(Tag& value) { pushBack(std::move(value)); }

        void erase(size_t idx)
        {
            if (idx >= data_.size())
                throw std::out_of_range(_ERR_OUT_OF_RANGE);

            data_.erase(data_.begin() + idx);
        }

        void erase(Vec<Tag>::iterator it)
        {
            if (it >= data_.end())
                throw std::out_of_range(_ERR_OUT_OF_RANGE);

            data_.erase(it);
        }

        void popFront()
        {
            if (data_.empty())
                throw std::out_of_range(_ERR_EMPTY_CONTAINER);

            data_.erase(data_.begin());
        }

        void popBack()
        {
            if (data_.empty())
                throw std::out_of_range(_ERR_EMPTY_CONTAINER);

            data_.pop_back();
        }

        void clear() { data_.clear(); }

        Tag& front()
        {
            if (data_.empty())
                throw std::out_of_range(_ERR_EMPTY_CONTAINER);

            return data_.front();
        }

        const Tag& front() const
        {
            if (data_.empty())
                throw std::out_of_range(_ERR_EMPTY_CONTAINER);

            return data_.front();
        }

        Tag& back()
        {
            if (data_.empty())
                throw std::out_of_range(_ERR_EMPTY_CONTAINER);

            return data_.back();
        }

        const Tag& back() const
        {
            if (data_.empty())
                throw std::out_of_range(_ERR_EMPTY_CONTAINER);

            return data_.back();
        }

        Vec<Tag>::iterator begin() { return data_.begin(); }

        Vec<Tag>::const_iterator begin() const { return data_.begin(); }

        Vec<Tag>::iterator end() { return data_.end(); }

        Vec<Tag>::const_iterator end() const { return data_.end(); }

        Tag& operator[](size_t idx)
        {
            if (idx >= data_.size())
                throw std::out_of_range(_ERR_OUT_OF_RANGE);

            return data_[idx];
        }

        const Tag& operator[](size_t idx) const
        {
            if (idx >= data_.size())
                throw std::out_of_range(_ERR_OUT_OF_RANGE);

            return data_[idx];
        }

    private:
        Vec<Tag> data_;
    };

    class CompoundData
    {
        struct TagpHash
        {
            size_t operator()(const Tag* tag) const
            {
                // std::cout << "Hash tag name:" << tag->name() << std::endl;
                return std::hash<Str>()(tag->name());
            }
        };
        
        struct TagpEqual
        {
            bool operator()(const Tag* tag1, const Tag* tag2) const
            {
                // std::cout << "Equal tag name:" << tag1->name() << " " << tag2->name() << std::endl;
                return tag1->name() == tag2->name();
            }
        };

    public:
        CompoundData() = default;

        size_t size() const { return data_.size(); }

        bool empty() const { return data_.empty(); }

        void reserve(size_t size) { data_.reserve(size); }

        void pushBack(Tag&& tag)
        {
            data_.emplace_back(std::move(tag));
            idxs_.insert( { &data_.back(), data_.size() - 1 } );

            data_.back().isListElement_ = false;
        }

        void pushBack(Tag& tag) { pushBack(std::move(tag)); }

        void erase(size_t idx)
        {
            if (idx >= data_.size())
                throw std::out_of_range(_ERR_OUT_OF_RANGE);

            idxs_.erase(&data_[idx]);
            data_.erase(data_.begin() + idx);
        }

        void erase(Vec<Tag>::iterator it)
        {
            if (it >= data_.end())
                throw std::out_of_range(_ERR_OUT_OF_RANGE);

            idxs_.erase(&(*it));
            data_.erase(it);
        }

        void erase(const Str& name)
        {
            Tag tmp = Tag().setName(name);

            if (idxs_.find(&tmp) == idxs_.end())
                throw std::out_of_range(_ERR_NO_SPECIFY_MEMBER);

            data_.erase(data_.begin() + idxs_[&tmp]);
        }

        void popFront()
        {
            if (data_.empty())
                throw std::out_of_range(_ERR_EMPTY_CONTAINER);

            idxs_.erase(&data_.front());
            data_.erase(data_.begin());
        }

        void popBack()
        {
            if (data_.empty())
                throw std::out_of_range(_ERR_EMPTY_CONTAINER);

            idxs_.erase(&data_.back());
            data_.pop_back();
        }

        void clear()
        {
            data_.clear();
            idxs_.clear();
        }

        Tag& front()
        {
            if (data_.empty())
                throw std::out_of_range(_ERR_EMPTY_CONTAINER);

            return data_.front();
        }

        const Tag& front() const
        {
            if (data_.empty())
                throw std::out_of_range(_ERR_EMPTY_CONTAINER);

            return data_.front();
        }

        Tag& back()
        {
            if (data_.empty())
                throw std::out_of_range(_ERR_EMPTY_CONTAINER);

            return data_.back();
        }

        const Tag& back() const
        {
            if (data_.empty())
                throw std::out_of_range(_ERR_EMPTY_CONTAINER);

            return data_.back();
        }

        Vec<Tag>::iterator begin() { return data_.begin(); }

        Vec<Tag>::const_iterator begin() const { return data_.begin(); }

        Vec<Tag>::iterator end() { return data_.end(); }

        const Vec<Tag>::iterator end() const { return data_.end(); }

        bool hasTag(const Str& name) const
        {
            Tag tmp = Tag().setName(name);

            for (const auto& var : name2idx_) {
                std::cout << "Address: " << var.first << " " << var.second << std::endl;
            }

            return name2idx_.find(&tmp) != name2idx_.end();
        }

        Tag& operator[](size_t idx)
        {
            if (idx >= size())
                throw std::out_of_range(_ERR_OUT_OF_RANGE);

            return data_.operator[](idx);
        }

        Tag& operator[](const Str& name)
        {
            Tag tmp = Tag().setName(name);

            if (name2idx_.find(&tmp) == name2idx_.end())
                throw std::out_of_range(_ERR_NO_SPECIFY_MEMBER);

            return data_.operator[](name2idx_[&tmp]);
        }
        
    private:
        Vec<Tag> data_;
        Map<Tag*, size_t, std::hash<Tag*>, TagpEqual> idxs_;
    };

    Tag() = default;

    // @brief Construct a tag with tag type.
    Tag(TagType type) :
        type_(type)
    {}

    // @note If the other tag is a list element and it's name is exist, delete the name.
    // @note You can ignore following content, it's just about details of implementation.
    // Q: Why is there a situation where the tag is a list elemet but there is a name?
    // A: Although we can't set the name of list element in outside,
    // but we need add a non-ListElement tag to list, so we set non-ListElement tag's #isListElement_ variable to true,
    // and delete it's name in there. (list element not need name.)
    Tag(Tag&& other) noexcept :
        isListElement_(other.isListElement_), type_(other.type_), dtype_(other.dtype_),
        name_(other.name_), data_(other.data_)
    {
        other.name_ = nullptr;
        other.data_.s = nullptr;

        if (isListElement_ && name_) {
            delete name_;
            name_ = nullptr;
        }
    }

    Tag(const Tag& other) :
        isListElement_(other.isListElement_), type_(other.type_), dtype_(other.dtype_)
    {
        if (!isListElement_ && other.name_)
            name_ = new Str(*other.name_);

        if (other.isNum())
            data_.n = other.data_.n;
        else if (other.isString() && other.data_.s)
            data_.s = new Str(*other.data_.s);
        else if (other.isArray() && other.data_.bad)
            data_.bad = new Vec<byte>(*other.data_.bad);
        else if (other.isIntArray() && other.data_.iad)
            data_.iad = new Vec<int32>(*other.data_.iad);
        else if (other.isLongArray() && other.data_.lad)
            data_.lad = new Vec<int64>(*other.data_.lad);
        else if (other.isList() && other.data_.ld)
            data_.ld = new ListData(*other.data_.ld);
        else if (other.isCompound() && other.data_.cd)
            data_.cd = new CompoundData(*other.data_.cd);
    }

    ~Tag()
    {
        if (name_) {
            delete name_;
            name_ = nullptr;
        }

        if (isString() && data_.s)
            delete data_.s;
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

        data_.s = nullptr;
    }

    // @brief Load the tag from binary input stream.
    // @param is The input stream.
    // @param isBigEndian Whether the read data from input stream by big endian.
    // @param headerSize The size of need discard data from input stream begin.
    // (usually is 0, but bedrock edition map file is 8, some useless dat.)
    static Tag fromBinStream(std::ifstream& is, bool isBigEndian, size_t headerSize = 0)
    {
    #ifdef MCNBT_USE_GZIP
        std::stringstream buf;
        buf << is.rdbuf();
        Str content = buf.str();
        buf.clear();

        std::stringstream ss;
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

    // @brief Load the tag from a nbt file. (Fast way version of #fromBinStream.)
    static Tag fromFile(const Str& filename, bool isBigEndian, size_t headerSize = 0)
    {
        std::ifstream is(filename, std::ios::binary);

        if (!is.is_open())
            throw std::runtime_error("Failed to open file: " + filename);

        Tag rslt = fromBinStream(is, isBigEndian, headerSize);
        is.close();

        return rslt;
    }

    /*
    * Functions for check tag type.
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
    * Functions for set tag's value. Only be called by corresponding tag.
    */

    // @attention Only be called by #TT_BYTE tag.
    Tag& setByte(byte value)
    {
        if (!isByte())
            throw std::logic_error(_ERR_INCORRECT_TT);

        data_.n.i8 = value;

        return *this;
    }

    // @attention Only be called by #TT_SHORT tag.
    Tag& setShort(int16 value)
    {
        if (!isShort())
            throw std::logic_error(_ERR_INCORRECT_TT);

        data_.n.i16 = value;

        return *this;
    }

    // @attention Only be called by #TT_INT tag.
    Tag& setInt(int32 value)
    {
        if (!isInt())
            throw std::logic_error(_ERR_INCORRECT_TT);

        data_.n.i32 = value;

        return *this;
    }

    // @attention Only be called by #TT_LONG tag.
    Tag& setLong(int64 value)
    {
        if (!isLong())
            throw std::logic_error(_ERR_INCORRECT_TT);

        data_.n.i64 = value;

        return *this;
    }

    Tag& setInteger(int64 value)
    {
        if (isByte())
            setByte(static_cast<byte>(value));
        else if (isShort())
            setShort(static_cast<int16>(value));
        else if (isInt())
            setInt(static_cast<int32>(value));
        else if (isLong())
            setLong(value);
        else
            throw std::logic_error(_ERR_INCORRECT_TT);

        return *this;
    }

    // @attention Only be called by #TT_FLOAT tag.
    Tag& setFloat(fp32 value)
    {
        if (!isFloat())
            throw std::logic_error(_ERR_INCORRECT_TT);

        data_.n.f32 = value;

        return *this;
    }

    // @attention Only be called by #TT_DOUBLE tag.
    Tag& setDouble(fp64 value)
    {
        if (!isDouble())
            throw std::logic_error(_ERR_INCORRECT_TT);

        data_.n.f64 = value;

        return *this;
    }

    Tag& setFloatPoint(fp64 value)
    {
        if (isFloat())
            setFloat(static_cast<fp32>(value));
        else if (isDouble())
            setDouble(value);
        else
            throw std::logic_error(_ERR_INCORRECT_TT);

        return *this;
    }

    // @attention Only be called by #TT_STRING tag.
    Tag& setString(const Str& value)
    {
        if (!isString())
            throw std::logic_error(_ERR_INCORRECT_TT);

        if (value.empty())
            return *this;

        if (data_.s)
            *data_.s = value;
        else
            data_.s = new Str(value);

        return *this;
    }

    // @attention Only be called by #TT_BYTE_ARRAY tag.
    Tag& setArray(const Vec<byte>& value)
    {
        if (!isByteArray())
            throw std::logic_error(_ERR_INCORRECT_TT);

        if (value.empty())
            return *this;

        if (data_.bad)
            *data_.bad = value;
        else
            data_.bad = new Vec<byte>(value);

        return *this;
    }

    // @attention Only be called by #TT_INT_ARRAY tag.
    Tag& setArray(const Vec<int32>& value)
    {
        if (!isIntArray())
            throw std::logic_error(_ERR_INCORRECT_TT);

        if (value.empty())
            return *this;

        if (data_.iad)
            *data_.iad = value;
        else
            data_.iad = new Vec<int32>(value);

        return *this;
    }

    // @attention Only be called by #TT_LONG_ARRAY tag.
    Tag& setArray(const Vec<int64>& value)
    {
        if (!isLongArray())
            throw std::logic_error(_ERR_INCORRECT_TT);

        if (value.empty())
            return *this;

        if (data_.lad)
            *data_.lad = value;
        else
            data_.lad = new Vec<int64>(value);

        return *this;
    }

    Tag& addByte(byte value)
    {
        if (!isByteArray())
            throw std::logic_error(_ERR_INCORRECT_TT);

        if (!data_.bad)
            data_.bad = new Vec<byte>();

        data_.bad->push_back(value);

        return *this;
    }

    Tag& addInt(int32 value)
    {
        if (!isIntArray())
            throw std::logic_error(_ERR_INCORRECT_TT);

        if (!data_.iad)
            data_.iad = new Vec<int32>();

        data_.iad->push_back(value);

        return *this;
    }

    Tag& addLong(int64 value)
    {
        if (!isLongArray())
            throw std::logic_error(_ERR_INCORRECT_TT);

        if (!data_.lad)
            data_.lad = new Vec<int64>();

        data_.lad->push_back(value);

        return *this;
    }

    Tag& append(const Vec<byte>& value)
    {
        if (!isByteArray())
            throw std::logic_error(_ERR_INCORRECT_TT);

        if (!data_.bad)
            data_.bad = new Vec<byte>();

        data_.bad->insert(data_.bad->end(), value.begin(), value.end());

        return *this;
    }

    Tag& append(const Vec<int32>& value)
    {
        if (!isIntArray())
            throw std::logic_error(_ERR_INCORRECT_TT);

        if (!data_.iad)
            data_.iad = new Vec<int32>();

        data_.iad->insert(data_.iad->end(), value.begin(), value.end());

        return *this;
    }

    Tag& append(const Vec<int64>& value)
    {
        if (!isLongArray())
            throw std::logic_error(_ERR_INCORRECT_TT);

        if (!data_.lad)
            data_.lad = new Vec<int64>();

        data_.lad->insert(data_.lad->end(), value.begin(), value.end());

        return *this;
    }

    // @brief Add a tag to the initialized list or compound.
    // @note Original tag will be moved to the new tag whether left-value or right-value reference,
    // and the original tag will invlid after this operation, but you can call #copy function to avoid this.
    // @attention Only be called by #TT_LIST, #TT_COMPOUND tag.
    Tag& addTag(Tag&& tag)
    {
        if (isList()) {
            if (tag.type() != dtype_)
                throw std::logic_error(_ERR_INCORRECT_TT);

            if (dtype_ == TT_END)
                throw std::logic_error(_ERR_READ_WRITE_UNINIT_LIST);
            
            if (!data_.ld)
                data_.ld = new ListData();
            
            data_.ld->pushBack(std::move(tag));
            data_.ld->back().isListElement_ = true;

            if (data_.ld->back().name_) {
                delete data_.ld->back().name_;
                data_.ld->back().name_ = nullptr;
            }
        } else if (isCompound()) {
            if (!data_.cd)
                data_.cd = new CompoundData();

            // If the tag is already exist, replace it.
            if (data_.cd->hasTag(tag.name())) {
                (*data_.cd)[tag.name()] = std::move(tag);
                (*data_.cd)[tag.name()].isListElement_ = false;
            } else {
                data_.cd->pushBack(std::move(tag));
            }
        } else {
            throw std::logic_error(_ERR_INCORRECT_TT);
        }

        return *this;
    }

    // @overload
    Tag& addTag(Tag& tag) { return addTag(std::move(tag)); }

    /*
    * Functions for get value. Only be called by corresponding tag.
    */

    // @attention Only be called by #TT_BYTE tag.
    byte getByte() const
    {
        if (!isByte())
            throw std::logic_error(_ERR_INCORRECT_TT);

        return data_.n.i8;
    }

    // @attention Only be called by #TT_SHORT tag.
    int16 getShort() const
    {
        if (!isShort())
            throw std::logic_error(_ERR_INCORRECT_TT);

        return data_.n.i16;
    }

    // @attention Only be called by #TT_INT tag.
    int32 getInt() const
    {
        if (!isInt())
            throw std::logic_error(_ERR_INCORRECT_TT);

        return data_.n.i32;
    }

    // @attention Only be called by #TT_LONG tag.
    int64 getLong() const
    {
        if (!isLong())
            throw std::logic_error(_ERR_INCORRECT_TT);

        return data_.n.i64;
    }

    // @attention Only be called by #TT_FLOAT tag.
    fp32 getFloat() const
    {
        if (!isFloat())
            throw std::logic_error(_ERR_INCORRECT_TT);

        return data_.n.f32;
    }

    // @attention Only be called by #TT_DOUBLE tag.
    fp64 getDouble() const
    {
        if (!isDouble())
            throw std::logic_error(_ERR_INCORRECT_TT);

        return data_.n.f64;
    }

    // @brief Get the value of string.
    // @attention Only be called by #TT_STRING tag.
    Str getString() const
    {
        if (!isString())
            throw std::logic_error(_ERR_INCORRECT_TT);

        if (!data_.s)
            return "";

        return *data_.s;
    }

    // @attention Only be called by #TT_BYTE_ARRAY tag.
    Vec<byte> getByteArray() const
    {
        if (!isByteArray())
            throw std::logic_error(_ERR_INCORRECT_TT);

        if (!data_.bad)
            return Vec<byte>();

        return *data_.bad;
    }

    byte getByte(size_t idx) const
    {
        if (!isByteArray())
            throw std::logic_error(_ERR_INCORRECT_TT);

        if (!data_.bad || idx >= data_.bad->size())
            throw std::out_of_range(_ERR_OUT_OF_RANGE);

        return (*data_.bad)[idx];
    }

    byte getFrontByte() const
    {
        if (!isByteArray())
            throw std::logic_error(_ERR_INCORRECT_TT);

        if (!data_.bad || data_.bad->empty())
            throw std::out_of_range(_ERR_EMPTY_CONTAINER);

        return data_.bad->front();
    }

    byte getBackByte() const
    {
        if (!isByteArray())
            throw std::logic_error(_ERR_INCORRECT_TT);

        if (!data_.bad || data_.bad->empty())
            throw std::out_of_range(_ERR_EMPTY_CONTAINER);

        return data_.bad->back();
    }

    // @attention Only be called by #TT_INT_ARRAY tag.
    Vec<int32> getIntArray() const
    {
        if (!isIntArray())
            throw std::logic_error(_ERR_INCORRECT_TT);

        if (!data_.iad)
            return Vec<int32>();

        return *data_.iad;
    }

    int32 getInt(size_t idx) const
    {
        if (!isIntArray())
            throw std::logic_error(_ERR_INCORRECT_TT);

        if (!data_.iad || idx >= data_.iad->size())
            throw std::out_of_range(_ERR_OUT_OF_RANGE);

        return (*data_.iad)[idx];
    }

    int32 getFrontInt() const
    {
        if (!isIntArray())
            throw std::logic_error(_ERR_INCORRECT_TT);

        if (!data_.iad || data_.iad->empty())
            throw std::out_of_range(_ERR_EMPTY_CONTAINER);

        return data_.iad->front();
    }

    int32 getBackInt() const
    {
        if (!isIntArray())
            throw std::logic_error(_ERR_INCORRECT_TT);

        if (!data_.iad || data_.iad->empty())
            throw std::out_of_range(_ERR_EMPTY_CONTAINER);

        return data_.iad->back();
    }

    // @attention Only be called by #TT_LONG_ARRAY tag.
    Vec<int64> getLongArray() const
    {
        if (!isLongArray())
            throw std::logic_error(_ERR_INCORRECT_TT);

        if (!data_.lad)
            return Vec<int64>();

        return *data_.lad;
    }

    int64 getLong(size_t idx) const
    {
        if (!isLongArray())
            throw std::logic_error(_ERR_INCORRECT_TT);

        if (!data_.lad || idx >= data_.lad->size())
            throw std::out_of_range(_ERR_OUT_OF_RANGE);

        return (*data_.lad)[idx];
    }

    int64 getFrontLong() const
    {
        if (!isLongArray())
            throw std::logic_error(_ERR_INCORRECT_TT);

        if (!data_.lad || data_.lad->empty())
            throw std::out_of_range(_ERR_EMPTY_CONTAINER);

        return data_.lad->front();
    }

    int64 getBackLong() const
    {
        if (!isLongArray())
            throw std::logic_error(_ERR_INCORRECT_TT);

        if (!data_.lad || data_.lad->empty())
            throw std::out_of_range(_ERR_EMPTY_CONTAINER);

        return data_.lad->back();
    }

    // @attention Only be called by #TT_LIST.
    ListData getList() const
    {
        if (!isList())
            throw std::logic_error(_ERR_INCORRECT_TT);

        if (dtype_ == TT_END)
            throw std::logic_error(_ERR_READ_WRITE_UNINIT_LIST);

        if (!data_.ld)
            return ListData();

        return *data_.ld;
    }

    // @attention Only be called by #TT_COMPOUND.
    CompoundData getCompound() const
    {
        if (!isCompound())
            throw std::logic_error(_ERR_INCORRECT_TT);

        if (!data_.cd)
            return CompoundData();

        return *data_.cd;
    }

    // @brief Get the tag by index.
    // @attention Only be called by #TT_LIST, #TT_COMPOUND tag.
    Tag& getTag(size_t idx)
    {
        if (isList()) {
            if (dtype_ == TT_END)
                throw std::logic_error(_ERR_READ_WRITE_UNINIT_LIST);

            return (*data_.ld)[idx];
        } else if (isCompound()) {
            return (*data_.cd)[idx];
        } else {
            throw std::logic_error(_ERR_INCORRECT_TT);
        }
    }

    // @overload
    // @brief Get the tag by name.
    // @attention Only be called by #TT_COMPOUND tag.
    Tag& getTag(const Str& name)
    {
        if (!isCompound())
            throw std::logic_error(_ERR_INCORRECT_TT);

        if (!data_.cd)
            throw std::out_of_range(_ERR_NO_SPECIFY_MEMBER);

        return (*data_.cd)[name];
    }

    // @attention Only be called by #TT_LIST, #TT_COMPOUND tag.
    Tag& getFrontTag()
    {
        if (isList()) {
            if (dtype_ == TT_END)
                throw std::logic_error(_ERR_READ_WRITE_UNINIT_LIST);

            return data_.ld->front();
        } else if (isCompound()) {
            return data_.cd->front();
        } else {
            throw std::logic_error(_ERR_INCORRECT_TT);
        }
    }

    // @attention Only be called by #TT_LIST, #TT_COMPOUND tag.
    Tag& getBackTag()
    {
        if (isList()) {
            if (dtype_ == TT_END)
                throw std::logic_error(_ERR_READ_WRITE_UNINIT_LIST);

            return data_.ld->back();
        } else if (isCompound()) {
            return data_.cd->back();
        } else {
            throw std::logic_error(_ERR_INCORRECT_TT);
        }
    }

    /*
    * Functions for other operations about tag.
    */

    // @brief Get the tag type.
    TagType type() const { return type_; }

    // @brief Get the name of tag.
    Str name() const
    {
        if (!name_)
            return "";

        std::cout << "name address: " << name_ << std::endl;
        return *name_;
    }

    // @brief Get the name length of tag.
    int16 nameLen() const
    {
        if (!name_)
            return 0;

        return static_cast<int16>(name_->size());
    }

    // @brief Get the size of container (string, array, list, compound).
    // @attention Only be called by
    // #TT_STRING, #TT_BYTE_ARRAY, #TT_INT_ARRAY, #TT_LONG_ARRAY, #TT_LIST, #TT_COMPOUND tag.
    size_t size() const
    {
        if (isString())
            return !data_.s ? 0 : data_.s->size();
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

        throw std::logic_error(_ERR_INCORRECT_TT);
    }

    // @brief Whether container is empty (string, array, list, compound).
    // @attention Only be called by
    // #TT_STRING, #TT_BYTE_ARRAY, #TT_INT_ARRAY, #TT_LONG_ARRAY, #TT_LIST, #TT_COMPOUND tag.
    bool empty() const { return size() == 0; }

    void reserve(size_t size)
    {
        if (isString()) {
            if (!data_.s)
                data_.s = new Str();

            data_.s->reserve(size);
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
            if (dtype_ == TT_END)
                throw std::logic_error(_ERR_READ_WRITE_UNINIT_LIST);

            if (!data_.ld)
                data_.ld = new ListData();

            data_.ld->reserve(size);
        } else if (isCompound()) {
            if (!data_.cd)
                data_.cd = new CompoundData();

            data_.cd->reserve(size);
        } else {
            throw std::logic_error(_ERR_INCORRECT_TT);
        }
    }

    // @brief Whether the list is initialized (#dtype_ is not TT_END).
    // @attention Only be called by #TT_LIST tag.
    bool isInitializedList() const
    {
        if (!isList())
            throw std::logic_error(_ERR_INCORRECT_TT);

        return dtype_ != TT_END;
    }

    // @brief Get the element's tag type of list.
    // @attention Only be called by #TT_LIST tag
    TagType listElementType() const
    {
        if (!isList())
            throw std::logic_error(_ERR_INCORRECT_TT);

        return dtype_;
    }

    // @brief Whether the tag of specify name is exist.
    // @attention Only be called by #TT_COMPOUND tag.
    bool hasTag(const Str& name) const
    {
        if (!isCompound())
            throw std::logic_error(_ERR_INCORRECT_TT);

        if (!data_.cd)
            return false;

        return data_.cd->hasTag(name);
    }

    // @brief Set the name of tag.
    // @attention Only be called by non-ListElement tag.
    Tag& setName(const Str& name)
    {
        if (isListElement_)
            throw std::logic_error(_ERR_INCORRECT_TT);

        if (name.empty() && !name_)
            return *this;

        if (name_)
            *name_ = name;
        else
            name_ = new Str(name);

        return *this;
    }

    // @brief Initalize the element tag type of the list.
    // @attention Only be called by #TT_LIST tag.
    Tag& initListElementType(TagType type)
    {
        if (!isList())
            throw std::logic_error(_ERR_INCORRECT_TT);

        if (dtype_ != TT_END)
            throw std::logic_error(_ERR_REPEAT_INIT_TT_LIST);

        dtype_ = type;

        return *this;
    }

    // @brief Reset the element tag type of the list and clear all elements.
    // @attention Only be called by #TT_LIST tag.
    Tag& resetList()
    {
        if (!isList())
            throw std::logic_error(_ERR_INCORRECT_TT);

        if (dtype_ == TT_END)
            return *this;

        if (data_.ld) {
            delete data_.ld;
            data_.ld = nullptr;
        }

        dtype_ = TT_END;

        return *this;
    }

    // @brief Remove the element of container (string, array, list, compound) by index.
    // @attention Only be called by
    // #TT_STRING, #TT_BYTE_ARRAY, #TT_INT_ARRAY, #TT_LONG_ARRAY, #TT_LIST, #TT_COMPOUND tag.
    Tag& remove(size_t idx)
    {
        if (isString()) {
            if (!data_.s || idx >= data_.s->size())
                throw std::out_of_range(_ERR_OUT_OF_RANGE);

            data_.s->erase(data_.s->begin() + idx);
        } else if (isByteArray()) {
            if (!data_.bad || idx >= data_.bad->size())
                throw std::out_of_range(_ERR_OUT_OF_RANGE);

            data_.bad->erase(data_.bad->begin() + idx);
        } else if (isIntArray()) {
            if (!data_.iad || idx >= data_.iad->size())
                throw std::out_of_range(_ERR_OUT_OF_RANGE);

            data_.iad->erase(data_.iad->begin() + idx);
        } else if (isLongArray()) {
            if (!data_.lad || idx >= data_.lad->size())
                throw std::out_of_range(_ERR_OUT_OF_RANGE);

            data_.lad->erase(data_.lad->begin() + idx);
        } else if (isList()) {
            if (dtype_ == TT_END)
                throw std::logic_error(_ERR_READ_WRITE_UNINIT_LIST);

            data_.ld->erase(idx);
        } else if (isCompound()) {
            data_.cd->erase(idx);
        } else {
            throw std::logic_error(_ERR_INCORRECT_TT);
        }

        return *this;
    }

    // @overload
    // @brief Remove the tag by name.
    // @attention Only be called by #TT_COMPOUND tag.
    Tag& remove(const Str& name)
    {
        if (!isCompound())
            throw std::logic_error(_ERR_INCORRECT_TT);
            
        data_.cd->erase(name);

        return *this;
    }

    // @brief Remove the first element of container (string, array, list, compound).
    // @attention Only be called by
    // #TT_STRING, #TT_BYTE_ARRAY, #TT_INT_ARRAY, #TT_LONG_ARRAY, #TT_LIST, #TT_COMPOUND tag.
    Tag& removeFront()
    {
        if (isString()) {
            if (!data_.s || data_.s->empty())
                throw std::out_of_range(_ERR_EMPTY_CONTAINER);

            data_.s->erase(data_.s->begin());
        } else if (isByteArray()) {
            if (!data_.bad || data_.bad->empty())
                throw std::out_of_range(_ERR_EMPTY_CONTAINER);

            data_.bad->erase(data_.bad->begin());
        } else if (isIntArray()) {
            if (!data_.iad || data_.iad->empty())
                throw std::out_of_range(_ERR_EMPTY_CONTAINER);

            data_.iad->erase(data_.iad->begin());
        } else if (isLongArray()) {
            if (!data_.lad || data_.lad->empty())
                throw std::out_of_range(_ERR_EMPTY_CONTAINER);

            data_.lad->erase(data_.lad->begin());
        } else if (isList()) {
            if (dtype_ == TT_END)
                throw std::logic_error(_ERR_READ_WRITE_UNINIT_LIST);

            data_.ld->popFront();
        } else if (isCompound()) {
            data_.cd->popFront();
        } else {
            throw std::logic_error(_ERR_INCORRECT_TT);
        }

        return *this;
    }

    // @brief Remove the last element of container (string, array, list, compound).
    // @attention Only be called by
    // #TT_STRING, #TT_BYTE_ARRAY, #TT_INT_ARRAY, #TT_LONG_ARRAY, #TT_LIST, #TT_COMPOUND tag.
    Tag& removeBack()
    {
        if (isString()) {
            if (!data_.s || data_.s->empty())
                throw std::out_of_range(_ERR_EMPTY_CONTAINER);

            data_.s->pop_back();
        } else if (isByteArray()) {
            if (!data_.bad || data_.bad->empty())
                throw std::out_of_range(_ERR_EMPTY_CONTAINER);

            data_.bad->pop_back();
        } else if (isIntArray()) {
            if (!data_.iad || data_.iad->empty())
                throw std::out_of_range(_ERR_EMPTY_CONTAINER);

            data_.iad->pop_back();
        } else if (isLongArray()) {
            if (!data_.lad || data_.lad->empty())
                throw std::out_of_range(_ERR_EMPTY_CONTAINER);

            data_.lad->pop_back();
        } else if (isList()) {
            if (dtype_ == TT_END)
                throw std::logic_error(_ERR_READ_WRITE_UNINIT_LIST);

            data_.ld->popBack();
        } else if (isCompound()) {
            data_.cd->popBack();
        } else {
            throw std::logic_error(_ERR_INCORRECT_TT);
        }

        return *this;
    }

    // @brief Clear the container (string, array, list, compound).
    // @attention Only be called by
    // #TT_STRING, #TT_BYTE_ARRAY, #TT_INT_ARRAY, #TT_LONG_ARRAY, #TT_LIST, #TT_COMPOUND tag.
    Tag& removeAll()
    {
        if (isString()) {
            if (data_.s)
                data_.s->clear();
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
            if (dtype_ == TT_END)
                throw std::logic_error(_ERR_READ_WRITE_UNINIT_LIST);

            if (data_.ld)
                data_.ld->clear();
        } else if (isCompound()) {
            if (data_.cd)
                data_.cd->clear();
        } else {
            throw std::logic_error(_ERR_INCORRECT_TT);
        }

        return *this;
    }

    /*
    * Utility functions.
    */

    // @brief Make a copy. usually used for add tag to list or compound.
    // (because default add tag to list or compound is move.)
    Tag copy() const { return Tag(*this); }

#ifdef MCNBT_USE_GZIP
    // @brief Write the tag to output stream.
    void write(std::ostream& os, bool isBigEndian, bool isCompressed = false) const
    {
        if (isCompressed) {
            std::stringstream ss;
            write_(ss, isBigEndian);
            os << gzip::compress(ss.str());
        } else {
            write_(os, isBigEndian);
        }
    }

    // @overload
    // @brief Fast way of #write.
    void write(const Str& filename, bool isBigEndian, bool isCompressed = false) const
    {
        std::ofstream ofs(filename, std::ios_base::binary);

        if (ofs.is_open())
            write(ofs, isBigEndian, isCompressed);
        else
            throw std::runtime_error("Failed to open file: " + filename);

        ofs.close();
    }
#else
    // @brief Write the tag to output stream.
    void write(std::ostream& os, bool isBigEndian) const
    {
        write_(os, isBigEndian);
    }

    // @overload
    // @brief Fast way of #write.
    void write(const Str& filename, bool isBigEndian) const
    {
        std::ofstream ofs(filename, std::ios_base::binary);

        if (ofs.is_open())
            write(ofs, isBigEndian);
        else
            throw std::runtime_error("Failed to open file: " + filename);

        ofs.close();
    }
#endif // MCNBT_USE_GZIP

    // @brief Get the SNBT. (The string representation of NBT.)
    // @param isIndented If true, the output string will be indented and proper newline.
    Str toSnbt(bool isIndented = true) const
    {
        static unsigned int indentCount = 0;
        Str inheritedIndentStr(indentCount * _INDENT_SIZE, ' ');

        Str key = isIndented ? inheritedIndentStr : "";

        if (!isListElement_ && name_ && !name_->empty())
            key += *name_ + (isIndented ? ": " : ":");

        if (isEnd())
            return "";

        if (isByte())
            return key + std::to_string(static_cast<int>(data_.n.i8)) + 'b';

        if (isShort())
            return key + std::to_string(static_cast<int>(data_.n.i16)) + 's';

        if (isInt())
            return key + std::to_string(data_.n.i32);

        if (isLong())
            return key + std::to_string(data_.n.i64) + 'l';

        if (isFloat())
            return key + std::to_string(data_.n.f32) + 'f';

        if (isDouble())
            return key + std::to_string(data_.n.f64) + 'd';

        if (isString())
            return key + '"' + (data_.s ? *data_.s : "") + '"';

        if (isByteArray()) {
            if (!data_.bad)
                return key + "[B;]";

            // If has indent add the newline character and indent string.
            Str result = key + '[';
            result += isIndented ? ('\n' + inheritedIndentStr + _INDENT_STR) : "";
            result += "B;";

            for (const auto& var : *data_.bad) {
                result += isIndented ? ('\n' + inheritedIndentStr + _INDENT_STR) : "";
                result += std::to_string(static_cast<int>(var)) + "b,";
            }

            if (result.back() == ',')
                result.pop_back();

            result += isIndented ? ('\n' + inheritedIndentStr) : "";
            result += ']';

            return result;
        }

        if (isIntArray()) {
            if (!data_.iad)
                return key + "[I;]";

            // If has indent add the newline character and indent string.
            Str result = key + '[';
            result += isIndented ? ('\n' + inheritedIndentStr + _INDENT_STR) : "";
            result += "I;";

            for (const auto& var : *data_.iad) {
                result += isIndented ? ('\n' + inheritedIndentStr + _INDENT_STR) : "";
                result += std::to_string(var) + ",";
            }

            if (result.back() == ',')
                result.pop_back();

            result += isIndented ? ('\n' + inheritedIndentStr) : "";
            result += ']';

            return result;
        }

        if (isLongArray()) {
            if (!data_.lad)
                return key + "[L;]";

            // If has indent add the newline character and indent string.
            Str result = key + '[';
            result += isIndented ? ('\n' + inheritedIndentStr + _INDENT_STR) : "";
            result += "L;";

            for (const auto& var : *data_.lad) {
                result += isIndented ? ('\n' + inheritedIndentStr + _INDENT_STR) : "";
                result += std::to_string(var) + "l,";
            }

            if (result.back() == ',')
                result.pop_back();

            result += isIndented ? ('\n' + inheritedIndentStr) : "";
            result += ']';

            return result;
        }

        if (isList()) {
            if (!data_.ld)
                return key + "[]";

            Str result = key + "[";

            indentCount++;
            for (const auto& var : *data_.ld) {
                result += isIndented ? "\n" : "";
                result += var.toSnbt(isIndented) + ",";
            }
            indentCount--;

            if (result.back() == ',')
                result.pop_back();

            result += isIndented ? ('\n' + inheritedIndentStr) : "";
            result += ']';

            return result;
        }

        if (isCompound()) {
            if (!data_.cd)
                return key + "{}";

            Str result = key + "{";

            indentCount++;
            for (const auto& var : *data_.cd) {
                result += isIndented ? "\n" : "";
                result += var.toSnbt(isIndented) + ",";
            }
            indentCount--;

            if (result.back() == ',')
                result.pop_back();

            result += isIndented ? ('\n' + inheritedIndentStr) : "";
            result += '}';

            return result;
        }

        throw std::runtime_error(_ERR_UNDEFINED_TT);
    }

    /*
    * Operators overloading.
    */

    Tag& operator=(Tag&& other) noexcept
    {
        this->~Tag();

        isListElement_ = other.isListElement_;
        type_ = other.type_;
        dtype_ = other.dtype_;

        name_ = other.name_;
        data_ = other.data_;

        other.name_ = nullptr;
        other.data_.s = nullptr;

        // Following code is forever not execute, but i have OCD.
        if (isListElement_ && name_) {
            delete name_;
            name_ = nullptr;
        }

        return *this;
    }

    Tag& operator=(const Tag& other)
    {
        this->~Tag();

        isListElement_ = other.isListElement_;
        type_ = other.type_;
        dtype_ = other.dtype_;

        if (!isListElement_ && other.name_)
            name_ = new Str(*other.name_);

        if (other.isNum())
            data_.n = other.data_.n;
        else if (other.isString() && other.data_.s)
            data_.s = new Str(*other.data_.s);
        else if (other.isArray() && other.data_.bad)
            data_.bad = new Vec<byte>(*other.data_.bad);
        else if (other.isIntArray() && other.data_.iad)
            data_.iad = new Vec<int32>(*other.data_.iad);
        else if (other.isLongArray() && other.data_.lad)
            data_.lad = new Vec<int64>(*other.data_.lad);
        else if (other.isList() && other.data_.ld)
            data_.ld = new ListData(*other.data_.ld);
        else if (other.isCompound() && other.data_.cd)
            data_.cd = new CompoundData(*other.data_.cd);

        return *this;
    }

    // @brief Fast way of #getTag by index.
    Tag& operator[](size_t idx) { return getTag(idx); }

    // @overload
    // @brief Fast way of #getTag by name;
    Tag& operator[](const Str& name) { return getTag(name); }
    
    Tag& operator<<(Tag&& tag) { return addTag(std::move(tag)); }

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

    // Value of tag.
    // Individual tag is like key-value pair.
    // The key is the name of tag (can be empty, and all list element not has name.).
    // The value is stored in the following union.
    union Data
    {
        Data() : s(nullptr) {}
        // Number data
        Num n;
        // String data
        Str* s;
        // Byte Array data
        Vec<byte>* bad;
        // Int Array data
        Vec<int32>* iad;
        // Long Array data
        Vec<int64>* lad;
        // List data
        ListData* ld;
        // Compound data
        CompoundData* cd;
    };

    // @param tagType If the param isListElement is false, ignore it.
    // If the param isListElement is true, the tagType must be set to the same as the parent tag.
    static Tag fromBinStream_(std::istream& is, bool isBigEndian, bool isListElement, int tagType = -1)
    {
        Tag tag;
        tag.isListElement_ = isListElement;

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
                tag.name_ = new Str();
                tag.name_->assign(bytes, static_cast<size_t>(is.gcount()));

                delete[] bytes;
            }
        }

        // Read value.
        if (tagType == TT_BYTE)
            tag.data_.n.i8 = _bytes2num<byte>(is, isBigEndian);

        if (tagType == TT_SHORT)
            tag.data_.n.i16 = _bytes2num<int16>(is, isBigEndian);

        if (tagType == TT_INT)
            tag.data_.n.i32 = _bytes2num<int32>(is, isBigEndian);

        if (tagType == TT_LONG)
            tag.data_.n.i64 = _bytes2num<int64>(is, isBigEndian);

        if (tagType == TT_FLOAT)
            tag.data_.n.f32 = _bytes2num<fp32>(is, isBigEndian);

        if (tagType == TT_DOUBLE)
            tag.data_.n.f64 = _bytes2num<fp64>(is, isBigEndian);

        if (tagType == TT_STRING) {
            int16 strlen = _bytes2num<int16>(is, isBigEndian);

            if (strlen != 0) {
                byte* bytes = new byte[strlen];

                is.read(bytes, strlen);
                tag.data_.s = new Str();
                tag.data_.s->assign(bytes, static_cast<size_t>(is.gcount()));

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
                tag.data_.ld = new ListData();
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

    void write_(std::ostream& os, bool isBigEndian) const
    {
        if (!isListElement_) {
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
            os.put(data_.n.i8);
            return;
        }

        if (isShort()) {
            _num2bytes<int16>(data_.n.i16, os, isBigEndian);
            return;
        }

        if (isInt()) {
            _num2bytes<int32>(data_.n.i32, os, isBigEndian);
            return;
        }

        if (isLong()) {
            _num2bytes<int64>(data_.n.i64, os, isBigEndian);
            return;
        }

        if (isFloat()) {
            _num2bytes<fp32>(data_.n.f32, os, isBigEndian);
            return;
        }

        if (isDouble()) {
            _num2bytes<fp64>(data_.n.f64, os, isBigEndian);
            return;
        }

        if (isString()) {
            if (!data_.s || data_.s->empty()) {
                _num2bytes<int16>(static_cast<int16>(0), os, isBigEndian);
                return;
            }

            _num2bytes<int16>(static_cast<int16>(data_.s->size()), os, isBigEndian);
            os.write(data_.s->c_str(), data_.s->size());

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
                var.write_(os, isBigEndian);

            return;
        }

        if (isCompound()) {
            if (!data_.cd || data_.cd->empty()) {
                os.put(TT_END);
                return;
            }

            for (const auto& var : *data_.cd)
                var.write_(os, isBigEndian);

            os.put(TT_END);

            return;
        }
    }

    // A flag to indicate if the tag is a list element.
    bool isListElement_ = false;
    // Tag type.
    TagType type_ = TT_END;
    // Tag type of the list element. Only used if the tag is a list.
    TagType dtype_ = TT_END;
    // Tag name (key of key-value pair), Only used if the tag is not a list element.
    Str* name_ = nullptr;
    // Tag value (value of key-value pair).
    Data data_;
};

}

// Faster way for construct a Tag object.
namespace nbt
{

inline Tag gByte(byte value, const Str& name = "")
{
    Tag tag(TT_BYTE);

    if (!name.empty())
        tag.setName(name);

    return tag.setByte(value);
}

inline Tag gShort(int16 value, const Str& name = "")
{
    Tag tag(TT_SHORT);

    if (!name.empty())
        tag.setName(name);

    return tag.setShort(value);
}

inline Tag gInt(int32 value, const Str& name = "")
{
    Tag tag(TT_INT);

    if (!name.empty())
        tag.setName(name);

    return tag.setInt(value);
}

inline Tag gLong(int64 value, const Str& name = "")
{
    Tag tag(TT_LONG);

    if (!name.empty())
        tag.setName(name);

    return tag.setLong(value);
}

inline Tag gFloat(fp32 value, const Str& name = "")
{
    Tag tag(TT_FLOAT);

    if (!name.empty())
        tag.setName(name);

    return tag.setFloat(value);
}

inline Tag gDouble(fp64 value, const Str& name = "")
{
    Tag tag(TT_DOUBLE);

    if (!name.empty())
        tag.setName(name);

    return tag.setDouble(value);
}

inline Tag gString(const Str& value, const Str& name = "")
{
    Tag tag(TT_STRING);

    if (!name.empty())
        tag.setName(name);

    return tag.setString(value);
}

inline Tag gByteArray(const Vec<byte>& value, const Str& name = "")
{
    Tag tag(TT_BYTE_ARRAY);

    if (!name.empty())
        tag.setName(name);

    return tag.setArray(value);
}

inline Tag gIntArray(const Vec<int32>& value, const Str& name = "")
{
    Tag tag(TT_INT_ARRAY);

    if (!name.empty())
        tag.setName(name);

    return tag.setArray(value);
}

inline Tag gLongArray(const Vec<int64>& value, const Str& name = "")
{
    Tag tag(TT_LONG_ARRAY);

    if (!name.empty())
        tag.setName(name);

    return tag.setArray(value);
}

inline Tag gList(TagType dtype, const Str& name = "")
{
    Tag tag(TagType::TT_LIST);
    tag.initListElementType(dtype);

    if (!name.empty())
        tag.setName(name);

    return tag;
}

inline Tag gCompound(const Str& name = "")
{
    Tag tag(TT_COMPOUND);

    if (!name.empty())
        tag.setName(name);

    return tag;
}

}

#endif // !MCNBT_HPP
