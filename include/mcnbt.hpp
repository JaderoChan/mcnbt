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

inline std::string getTagTypeString(TagType type)
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

// Error messages.
constexpr const char* _ERR_UNDEFINED_TT = "Undefined tag type.";
constexpr const char* _ERR_INCORRECT_TT = "Incorrect tag type.";
constexpr const char* _ERR_OVER_RANGE = "The index or position is out of range.";
constexpr const char* _ERR_NO_SPECIFY_MEMBER = "No specify member.";
constexpr const char* _ERR_READ_WRITE_UNINIT_TT_LIST = "Read/Write an uninitialized list.";
constexpr const char* _ERR_REPEAT_INIT_TT_LIST = "Repeat initialize list.";
constexpr const char* _ERR_OTHER = "Other error.";

// About of the indent of snbt.
constexpr unsigned int _INDENT_SIZE = 2;
static const std::string _INDENT_STR(_INDENT_SIZE, ' ');

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
        std::string* s;
        // Byte Array data
        std::vector<byte>* bs;
        // Int Array data
        std::vector<int32>* is;
        // Long Array data
        std::vector<int64>* ls;
        // List data and Compound data
        std::vector<Tag>* d;
    };

public:
    Tag() = default;

    // @brief Construct a tag with tag type.
    Tag(TagType type) :
        type_(type)
    {}

    Tag(const Tag& other) :
        isListElement_(other.isListElement_), type_(other.type_), dtype_(other.dtype_)
    {
        if (!isListElement_ && other.name_)
            name_ = new std::string(*other.name_);

        if (other.isNum())
            data_.n = other.data_.n;
        else if (other.isString() && other.data_.s)
            data_.s = new std::string(*other.data_.s);
        else if (other.isArray() && other.data_.bs)
            data_.bs = new std::vector<byte>(*other.data_.bs);
        else if (other.isIntArray() && other.data_.is)
            data_.is = new std::vector<int32>(*other.data_.is);
        else if (other.isLongArray() && other.data_.ls)
            data_.ls = new std::vector<int64>(*other.data_.ls);
        else if (other.isComplex() && other.data_.d)
            data_.d = new std::vector<Tag>(*other.data_.d);
    }

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

    ~Tag()
    {
        if (name_) {
            delete name_;
            name_ = nullptr;
        }

        if (isString() && data_.s)
            delete data_.s;
        else if (isByteArray() && data_.bs)
            delete data_.bs;
        else if (isIntArray() && data_.is)
            delete data_.is;
        else if (isLongArray() && data_.ls)
            delete data_.ls;
        else if (isComplex() && data_.d)
            delete data_.d;

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
        std::string content = buf.str();
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
    static Tag fromFile(const std::string& filename, bool isBigEndian, size_t headerSize = 0)
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

    bool isEnd() const { return type_ == TT_END; }

    bool isByte() const { return type_ == TT_BYTE; }

    bool isShort() const { return type_ == TT_SHORT; }

    bool isInt() const { return type_ == TT_INT; }

    bool isLong() const { return type_ == TT_LONG; }

    bool isFloat() const { return type_ == TT_FLOAT; }

    bool isDouble() const { return type_ == TT_DOUBLE; }

    bool isString() const { return type_ == TT_STRING; }

    bool isByteArray() const { return type_ == TT_BYTE_ARRAY; }

    bool isIntArray() const { return type_ == TT_INT_ARRAY; }

    bool isLongArray() const { return type_ == TT_LONG_ARRAY; }

    bool isList() const { return type_ == TT_LIST; }

    bool isCompound() const { return type_ == TT_COMPOUND; }

    bool isInteger() const
    {
        return type_ == TT_BYTE ||
               type_ == TT_SHORT ||
               type_ == TT_INT ||
               type_ == TT_LONG;
    }

    bool isFloatPoint() const { return type_ == TT_FLOAT || type_ == TT_DOUBLE; }

    bool isNum() const { return isInteger() || isFloatPoint(); }

    bool isArray() const
    {
        return type_ == TT_BYTE_ARRAY ||
               type_ == TT_INT_ARRAY ||
               type_ == TT_LONG_ARRAY;
    }

    bool isComplex() const { return type_ == TT_LIST || type_ == TT_COMPOUND; }

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

    // @attention Only be called by #TT_STRING tag.
    Tag& setString(const std::string& value)
    {
        if (!isString())
            throw std::logic_error(_ERR_INCORRECT_TT);

        if (value.empty())
            return *this;

        if (data_.s)
            *data_.s = value;
        else
            data_.s = new std::string(value);

        return *this;
    }

    // @attention Only be called by #TT_BYTE_ARRAY tag.
    Tag& setByteArray(const std::vector<byte>& value)
    {
        if (!isByteArray())
            throw std::logic_error(_ERR_INCORRECT_TT);

        if (value.empty())
            return *this;

        if (data_.bs)
            *data_.bs = value;
        else
            data_.bs = new std::vector<byte>(value);

        return *this;
    }

    // @attention Only be called by #TT_INT_ARRAY tag.
    Tag& setIntArray(const std::vector<int32>& value)
    {
        if (!isIntArray())
            throw std::logic_error(_ERR_INCORRECT_TT);

        if (value.empty())
            return *this;

        if (data_.is)
            *data_.is = value;
        else
            data_.is = new std::vector<int32>(value);

        return *this;
    }

    // @attention Only be called by #TT_LONG_ARRAY tag.
    Tag& setLongArray(const std::vector<int64>& value)
    {
        if (!isLongArray())
            throw std::logic_error(_ERR_INCORRECT_TT);

        if (value.empty())
            return *this;

        if (data_.ls)
            *data_.ls = value;
        else
            data_.ls = new std::vector<int64>(value);

        return *this;
    }

    // @brief Add a value to the byte array.
    // @attention Only be called by #TT_BYTE_ARRAY tag.
    Tag& addByte(byte value)
    {
        if (!isByteArray())
            throw std::logic_error(_ERR_INCORRECT_TT);

        if (!data_.bs)
            data_.bs = new std::vector<byte>();

        data_.bs->push_back(value);

        return *this;
    }

    // @brief Add a value to the int array.
    // @attention Only be called by #TT_INT_ARRAY tag.
    Tag& addInt(int32 value)
    {
        if (!isIntArray())
            throw std::logic_error(_ERR_INCORRECT_TT);

        if (!data_.is)
            data_.is = new std::vector<int32>();

        data_.is->push_back(value);

        return *this;
    }

    // @brief Add a value to the long array.
    // @attention Only be called by #TT_LONG_ARRAY tag.
    Tag& addLong(int64 value)
    {
        if (!isLongArray())
            throw std::logic_error(_ERR_INCORRECT_TT);

        if (!data_.ls)
            data_.ls = new std::vector<int64>();

        data_.ls->push_back(value);

        return *this;
    }

    // @brief Add a tag to the initialized list or compound.
    // @note Original tag will be moved to the new tag whether left-value or right-value reference,
    // and the original tag will invlid after this operation, but you can call #copy function to avoid this.
    // @attention Only be called by #TT_LIST, #TT_COMPOUND tag.
    Tag& addTag(Tag&& tag)
    {
        if (!isComplex())
            throw std::logic_error(_ERR_INCORRECT_TT);

        if (isList() && tag.type() != dtype_)
            throw std::logic_error(_ERR_INCORRECT_TT);

        if (isList() && dtype_ == TT_END)
            throw std::logic_error(_ERR_READ_WRITE_UNINIT_TT_LIST);

        if (!data_.d)
            data_.d = new std::vector<Tag>();

        if (isList()) {
            data_.d->emplace_back(std::move(tag));
            data_.d->back().isListElement_ = true;

            if (data_.d->back().name_) {
                delete data_.d->back().name_;
                data_.d->back().name_ = nullptr;
            }
        } else {
            try {
                // If the tag is already exist, replace it.
                auto& t = getTag(tag.name());
                t = std::move(tag);
                t.isListElement_ = false;
            } catch (...) {
                data_.d->emplace_back(std::move(tag));
                data_.d->back().isListElement_ = false;
            }
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
    std::string getString() const
    {
        if (!isString())
            throw std::logic_error(_ERR_INCORRECT_TT);

        if (!data_.s)
            return "";

        return *data_.s;
    }

    // @attention Only be called by #TT_BYTE_ARRAY tag.
    std::vector<byte>* getByteArray() const
    {
        if (!isByteArray())
            throw std::logic_error(_ERR_INCORRECT_TT);

        return data_.bs;
    }

    // @overload
    // @brief Get a value from the byte array by index.
    // @attention Only be called by #TT_BYTE_ARRAY tag.
    byte getByte(size_t pos) const
    {
        if (!isByteArray())
            throw std::logic_error(_ERR_INCORRECT_TT);

        if (!data_.bs || pos >= data_.bs->size())
            throw std::range_error(_ERR_OVER_RANGE);

        return (*data_.bs)[pos];
    }

    // @attention Only be called by #TT_BYTE_ARRAY tag.
    byte frontByte() const
    {
        if (!isByteArray())
            throw std::logic_error(_ERR_INCORRECT_TT);

        if (!data_.bs || data_.bs->empty())
            throw std::range_error(_ERR_OVER_RANGE);

        return data_.bs->front();
    }

    // @attention Only be called by #TT_BYTE_ARRAY tag.
    byte backByte() const
    {
        if (!isByteArray())
            throw std::logic_error(_ERR_INCORRECT_TT);

        if (!data_.bs || data_.bs->empty())
            throw std::range_error(_ERR_OVER_RANGE);

        return data_.bs->back();
    }

    // @attention Only be called by #TT_INT_ARRAY tag.
    std::vector<int32>* getIntArray() const
    {
        if (!isIntArray())
            throw std::logic_error(_ERR_INCORRECT_TT);

        return data_.is;
    }

    // @overload
    // @brief Get a value from the int array by index.
    // @attention Only be called by #TT_INT_ARRAY tag.
    int32 getInt(size_t pos) const
    {
        if (!isIntArray())
            throw std::logic_error(_ERR_INCORRECT_TT);

        if (!data_.is || pos >= data_.is->size())
            throw std::range_error(_ERR_OVER_RANGE);

        return (*data_.is)[pos];
    }

    // @attention Only be called by #TT_INT_ARRAY tag.
    int32 frontInt() const
    {
        if (!isIntArray())
            throw std::logic_error(_ERR_INCORRECT_TT);

        if (!data_.is || data_.is->empty())
            throw std::range_error(_ERR_OVER_RANGE);

        return data_.is->front();
    }

    // @attention Only be called by #TT_INT_ARRAY tag.
    int32 backInt() const
    {
        if (!isIntArray())
            throw std::logic_error(_ERR_INCORRECT_TT);

        if (!data_.is || data_.is->empty())
            throw std::range_error(_ERR_OVER_RANGE);

        return data_.is->back();
    }

    // @attention Only be called by #TT_LONG_ARRAY tag.
    std::vector<int64>* getLongArray() const
    {
        if (!isLongArray())
            throw std::logic_error(_ERR_INCORRECT_TT);

        return data_.ls;
    }

    // @overload
    // @brief Get a value from long array by index.
    // @attention Only be called by #TT_LONG_ARRAY tag.
    int64 getLong(size_t pos) const
    {
        if (!isLongArray())
            throw std::logic_error(_ERR_INCORRECT_TT);

        if (!data_.ls || pos >= data_.ls->size())
            throw std::range_error(_ERR_OVER_RANGE);

        return (*data_.ls)[pos];
    }

    // @attention Only be called by #TT_LONG_ARRAY tag.
    int64 frontLong() const
    {
        if (!isLongArray())
            throw std::logic_error(_ERR_INCORRECT_TT);

        if (!data_.ls || data_.ls->empty())
            throw std::range_error(_ERR_OVER_RANGE);

        return data_.ls->front();
    }

    // @attention Only be called by #TT_LONG_ARRAY tag.
    int64 backLong() const
    {
        if (!isLongArray())
            throw std::logic_error(_ERR_INCORRECT_TT);

        if (!data_.ls || data_.ls->empty())
            throw std::range_error(_ERR_OVER_RANGE);

        return data_.ls->back();
    }

    // @attention Only be called by #TT_LIST, #TT_COMPOUND tag.
    std::vector<Tag>* getTags() const
    {
        if (!isComplex())
            throw std::logic_error(_ERR_INCORRECT_TT);

        return data_.d;
    }

    // @brief Get the tag by index.
    // @attention Only be called by #TT_LIST, #TT_COMPOUND tag.
    Tag& getTag(size_t pos)
    {
        if (!isComplex())
            throw std::logic_error(_ERR_INCORRECT_TT);

        if (isList() && dtype_ == TT_END)
            throw std::logic_error(_ERR_READ_WRITE_UNINIT_TT_LIST);

        if (!data_.d || pos >= data_.d->size())
            throw std::range_error(_ERR_OVER_RANGE);

        return (*data_.d)[pos];
    }

    // @overload
    // @brief Get the tag by name.
    // @attention Only be called by #TT_COMPOUND tag.
    Tag& getTag(const std::string& name)
    {
        if (!isCompound())
            throw std::logic_error(_ERR_INCORRECT_TT);

        if (!data_.d)
            throw std::logic_error(_ERR_NO_SPECIFY_MEMBER);

        for (auto& var : *data_.d) {
            if (!var.name_)
                continue;

            if (*var.name_ == name)
                return var;
        }

        throw std::logic_error(_ERR_NO_SPECIFY_MEMBER);
    }

    // @attention Only be called by #TT_LIST, #TT_COMPOUND tag.
    Tag& frontTag()
    {
        if (!isComplex())
            throw std::logic_error(_ERR_INCORRECT_TT);

        if (isList() && dtype_ == TT_END)
            throw std::logic_error(_ERR_READ_WRITE_UNINIT_TT_LIST);

        if (!data_.d || data_.d->empty())
            throw std::range_error(_ERR_OVER_RANGE);

        return data_.d->front();
    }

    // @attention Only be called by #TT_LIST, #TT_COMPOUND tag.
    Tag& backTag()
    {
        if (!isComplex())
            throw std::logic_error(_ERR_INCORRECT_TT);

        if (isList() && dtype_ == TT_END)
            throw std::logic_error(_ERR_READ_WRITE_UNINIT_TT_LIST);

        if (!data_.d || data_.d->empty())
            throw std::range_error(_ERR_OVER_RANGE);

        return data_.d->back();
    }

    /*
    * Functions for other operations about tag.
    */

    // @brief Get the tag type.
    TagType type() const { return type_; }

    // @brief Get the name of tag.
    std::string name() const
    {
        if (!name_)
            return "";

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
            return !data_.bs ? 0 : data_.bs->size();
        if (isIntArray())
            return !data_.is ? 0 : data_.is->size();
        if (isLongArray())
            return !data_.ls ? 0 : data_.ls->size();
        if (isComplex())
            return !data_.d ? 0 : data_.d->size();

        throw std::logic_error(_ERR_INCORRECT_TT);
    }

    // @brief Whether container is empty (string, array, list, compound).
    // @attention Only be called by
    // #TT_STRING, #TT_BYTE_ARRAY, #TT_INT_ARRAY, #TT_LONG_ARRAY, #TT_LIST, #TT_COMPOUND tag.
    bool empty() const { return size() == 0; }

    // @brief Whether the list is initialized (#dtype_ is not TT_END).
    // @attention Only be called by #TT_LIST tag.
    bool isInitializedList() const
    {
        if (!isList())
            throw std::logic_error(_ERR_INCORRECT_TT);

        return dtype_ != TT_END;
    }

    // @brief Get the string length of string.
    // @attention Only be called by #TT_STRING tag.
    int32 stringLen() const
    {
        if (!isString())
            throw std::logic_error(_ERR_INCORRECT_TT);

        if (!data_.s)
            return 0;

        return static_cast<int32>(data_.s->size());
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
    bool hasTag(const std::string& name) const
    {
        if (!isCompound())
            throw std::logic_error(_ERR_INCORRECT_TT);

        if (!data_.d)
            return false;

        for (const auto& var : *data_.d) {
            if (!var.name_)
                continue;

            if (*var.name_ == name)
                return true;
        }

        return false;
    }

    // @brief Set the name of tag.
    // @attention Only be called by non-ListElement tag.
    Tag& setName(const std::string& name)
    {
        if (isListElement_)
            throw std::logic_error(_ERR_INCORRECT_TT);

        if (name.empty())
            return *this;

        if (name_)
            *name_ = name;
        else
            name_ = new std::string(name);

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

        if (data_.d) {
            delete data_.d;
            data_.d = nullptr;
        }

        dtype_ = TT_END;

        return *this;
    }

    // @brief Remove the element of container (string, array, list, compound) by index.
    // @attention Only be called by
    // #TT_STRING, #TT_BYTE_ARRAY, #TT_INT_ARRAY, #TT_LONG_ARRAY, #TT_LIST, #TT_COMPOUND tag.
    Tag& remove(size_t pos)
    {
        if (isString()) {
            if (!data_.s || pos >= data_.s->size())
                throw std::range_error(_ERR_OVER_RANGE);

            data_.s->erase(data_.s->begin() + pos);
        } else if (isByteArray()) {
            if (!data_.bs || pos >= data_.bs->size())
                throw std::range_error(_ERR_OVER_RANGE);

            data_.bs->erase(data_.bs->begin() + pos);
        } else if (isIntArray()) {
            if (!data_.is || pos >= data_.is->size())
                throw std::range_error(_ERR_OVER_RANGE);

            data_.is->erase(data_.is->begin() + pos);
        } else if (isLongArray()) {
            if (!data_.ls || pos >= data_.ls->size())
                throw std::range_error(_ERR_OVER_RANGE);

            data_.ls->erase(data_.ls->begin() + pos);
        } else if (isComplex()) {
            if (isList() && dtype_ == TT_END)
                throw std::logic_error(_ERR_READ_WRITE_UNINIT_TT_LIST);

            if (!data_.d || pos >= data_.d->size())
                throw std::range_error(_ERR_OVER_RANGE);

            data_.d->erase(data_.d->begin() + pos);
        } else {
            throw std::logic_error(_ERR_INCORRECT_TT);
        }

        return *this;
    }

    // @overload
    // @brief Remove the tag by name.
    // @attention Only be called by #TT_COMPOUND tag.
    Tag& remove(const std::string& name)
    {
        if (!isCompound())
            throw std::logic_error(_ERR_INCORRECT_TT);

        if (!data_.d)
            throw std::logic_error(_ERR_NO_SPECIFY_MEMBER);

        for (auto it = data_.d->begin(); it != data_.d->end(); ++it) {
            if (!it->name_)
                continue;

            if (*(it->name_) == name) {
                data_.d->erase(it);
                return *this;
            }
        }

        throw std::logic_error(_ERR_NO_SPECIFY_MEMBER);
    }

    // @brief Remove the first element of container (string, array, list, compound).
    // @attention Only be called by
    // #TT_STRING, #TT_BYTE_ARRAY, #TT_INT_ARRAY, #TT_LONG_ARRAY, #TT_LIST, #TT_COMPOUND tag.
    Tag& removeFront()
    {
        if (isString()) {
            if (!data_.s || data_.s->empty())
                throw std::range_error(_ERR_OVER_RANGE);

            data_.s->erase(data_.s->begin());
        } else if (isByteArray()) {
            if (!data_.bs || data_.bs->empty())
                throw std::range_error(_ERR_OVER_RANGE);

            data_.bs->erase(data_.bs->begin());
        } else if (isIntArray()) {
            if (!data_.is || data_.is->empty())
                throw std::range_error(_ERR_OVER_RANGE);

            data_.is->erase(data_.is->begin());
        } else if (isLongArray()) {
            if (!data_.ls || data_.ls->empty())
                throw std::range_error(_ERR_OVER_RANGE);

            data_.ls->erase(data_.ls->begin());
        } else if (isComplex()) {
            if (isList() && dtype_ == TT_END)
                throw std::logic_error(_ERR_READ_WRITE_UNINIT_TT_LIST);

            if (!data_.d || data_.d->empty())
                throw std::range_error(_ERR_OVER_RANGE);

            data_.d->erase(data_.d->begin());
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
                throw std::range_error(_ERR_OVER_RANGE);

            data_.s->pop_back();
        } else if (isByteArray()) {
            if (!data_.bs || data_.bs->empty())
                throw std::range_error(_ERR_OVER_RANGE);

            data_.bs->pop_back();
        } else if (isIntArray()) {
            if (!data_.is || data_.is->empty())
                throw std::range_error(_ERR_OVER_RANGE);

            data_.is->pop_back();
        } else if (isLongArray()) {
            if (!data_.ls || data_.ls->empty())
                throw std::range_error(_ERR_OVER_RANGE);

            data_.ls->pop_back();
        } else if (isComplex()) {
            if (isList() && dtype_ == TT_END)
                throw std::logic_error(_ERR_READ_WRITE_UNINIT_TT_LIST);

            if (!data_.d || data_.d->empty())
                throw std::range_error(_ERR_OVER_RANGE);

            data_.d->pop_back();
        } else {
            throw std::logic_error(_ERR_INCORRECT_TT);
        }

        return *this;
    }

    // @brief Clear the container (string, array, list, compound).
    // @attention Only be called by
    // #TT_STRING, #TT_BYTE_ARRAY, #TT_INT_ARRAY, #TT_LONG_ARRAY, #TT_LIST, #TT_COMPOUND tag.
    Tag& clear()
    {
        if (isString()) {
            if (data_.s)
                data_.s->clear();
        } else if (isByteArray()) {
            if (data_.bs)
                data_.bs->clear();
        } else if (isIntArray()) {
            if (data_.is)
                data_.is->clear();
        } else if (isLongArray()) {
            if (data_.ls)
                data_.ls->clear();
        } else if (isComplex()) {
            if (isList() && dtype_ == TT_END)
                throw std::logic_error(_ERR_READ_WRITE_UNINIT_TT_LIST);

            if (data_.d)
                data_.d->clear();
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
    void write(const std::string& filename, bool isBigEndian, bool isCompressed = false) const
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
    void write(const std::string& filename, bool isBigEndian) const
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
    std::string toSnbt(bool isIndented = true) const
    {
        static unsigned int indentCount = 0;
        std::string inheritedIndentStr(indentCount * _INDENT_SIZE, ' ');

        std::string key = isIndented ? inheritedIndentStr : "";

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
            if (!data_.bs)
                return key + "[B;]";

            // If has indent add the newline character and indent string.
            std::string result = key + '[';
            result += isIndented ? ('\n' + inheritedIndentStr + _INDENT_STR) : "";
            result += "B;";

            for (const auto& var : *data_.bs) {
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
            if (!data_.is)
                return key + "[I;]";

            // If has indent add the newline character and indent string.
            std::string result = key + '[';
            result += isIndented ? ('\n' + inheritedIndentStr + _INDENT_STR) : "";
            result += "I;";

            for (const auto& var : *data_.is) {
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
            if (!data_.ls)
                return key + "[L;]";

            // If has indent add the newline character and indent string.
            std::string result = key + '[';
            result += isIndented ? ('\n' + inheritedIndentStr + _INDENT_STR) : "";
            result += "L;";

            for (const auto& var : *data_.ls) {
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
            if (!data_.d)
                return key + "[]";

            std::string result = key + "[";

            indentCount++;
            for (const auto& var : *data_.d) {
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
            if (!data_.d)
                return key + "{}";

            std::string result = key + "{";

            indentCount++;
            for (const auto& var : *data_.d) {
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

    Tag& operator=(const Tag& other)
    {
        this->~Tag();

        isListElement_ = other.isListElement_;
        type_ = other.type_;
        dtype_ = other.dtype_;

        if (!isListElement_ && other.name_)
            name_ = new std::string(*other.name_);

        if (other.isNum())
            data_.n = other.data_.n;
        else if (other.isString() && other.data_.s)
            data_.s = new std::string(*other.data_.s);
        else if (other.isArray() && other.data_.bs)
            data_.bs = new std::vector<byte>(*other.data_.bs);
        else if (other.isIntArray() && other.data_.is)
            data_.is = new std::vector<int32>(*other.data_.is);
        else if (other.isLongArray() && other.data_.ls)
            data_.ls = new std::vector<int64>(*other.data_.ls);
        else if (other.isComplex() && other.data_.d)
            data_.d = new std::vector<Tag>(*other.data_.d);

        return *this;
    }

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

    // @brief Fast way of #getTag by index.
    Tag& operator[](size_t pos) { return getTag(pos); }

    // @overload
    // @brief Fast way of #getTag by name;
    Tag& operator[](const std::string& name) { return getTag(name); }

    // @brief Fast way of #addTag.
    Tag& operator<<(Tag&& tag) { return addTag(std::move(tag)); }

    // @overload
    Tag& operator<<(Tag& tag) { return addTag(std::move(tag)); }

private:
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
                tag.name_ = new std::string();
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
                tag.data_.s = new std::string();
                tag.data_.s->assign(bytes, static_cast<size_t>(is.gcount()));

                delete[] bytes;
            }
        }

        if (tagType == TT_BYTE_ARRAY) {
            int32 dsize = _bytes2num<int32>(is, isBigEndian);

            if (dsize != 0) {
                tag.data_.bs = new std::vector<byte>();
                tag.data_.bs->reserve(dsize);

                for (int32 i = 0; i < dsize; ++i)
                    tag.addByte(_bytes2num<byte>(is, isBigEndian));
            }
        }

        if (tagType == TT_INT_ARRAY) {
            int32 dsize = _bytes2num<int32>(is, isBigEndian);

            if (dsize != 0) {
                tag.data_.is = new std::vector<int32>();
                tag.data_.is->reserve(dsize);

                for (int32 i = 0; i < dsize; ++i)
                    tag.addInt(_bytes2num<int32>(is, isBigEndian));
            }
        }

        if (tagType == TT_LONG_ARRAY) {
            int32 dsize = _bytes2num<int32>(is, isBigEndian);

            if (dsize != 0) {
                tag.data_.ls = new std::vector<int64>();
                tag.data_.ls->reserve(dsize);

                for (int32 i = 0; i < dsize; ++i)
                    tag.addLong(_bytes2num<int64>(is, isBigEndian));
            }
        }

        if (tagType == TT_LIST) {
            tag.dtype_ = static_cast<TagType>(is.get());
            int32 dsize = _bytes2num<int32>(is, isBigEndian);

            if (dsize != 0) {
                tag.data_.d = new std::vector<Tag>();
                tag.data_.d->reserve(dsize);

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
            if (!data_.bs || data_.bs->empty()) {
                _num2bytes<int32>(static_cast<int32>(0), os, isBigEndian);
                return;
            }

            _num2bytes<int32>(static_cast<int32>(data_.bs->size()), os, isBigEndian);

            for (const auto& var : *data_.bs)
                os.put(var);

            return;
        }

        if (isIntArray()) {
            if (!data_.is || data_.is->empty()) {
                _num2bytes<int32>(static_cast<int32>(0), os, isBigEndian);
                return;
            }

            _num2bytes<int32>(static_cast<int32>(data_.is->size()), os, isBigEndian);

            for (const auto& var : *data_.is)
                _num2bytes<int32>(var, os, isBigEndian);

            return;
        }

        if (isLongArray()) {
            if (!data_.ls || data_.ls->empty()) {
                _num2bytes<int32>(static_cast<int32>(0), os, isBigEndian);
                return;
            }

            _num2bytes<int32>(static_cast<int32>(data_.ls->size()), os, isBigEndian);

            for (const auto& var : *data_.ls)
                _num2bytes<int64>(var, os, isBigEndian);

            return;
        }

        if (isList()) {
            if (!data_.d || data_.d->empty()) {
                os.put(static_cast<byte>(TT_END));
                _num2bytes<int32>(static_cast<int32>(0), os, isBigEndian);
                return;
            }

            os.put(static_cast<byte>(dtype_));
            _num2bytes<int32>(static_cast<int32>(data_.d->size()), os, isBigEndian);

            for (const auto& var : *data_.d)
                var.write_(os, isBigEndian);

            return;
        }

        if (isCompound()) {
            if (!data_.d || data_.d->empty()) {
                os.put(TT_END);
                return;
            }

            for (const auto& var : *data_.d)
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
    std::string* name_ = nullptr;
    // Tag value (value of key-value pair).
    Data data_;
};

}

// Faster way for construct a Tag object.
namespace nbt
{

inline Tag gByte(byte value, const std::string& name = "")
{
    Tag tag(TT_BYTE);

    if (!name.empty())
        tag.setName(name);

    return tag.setByte(value);
}

inline Tag gShort(int16 value, const std::string& name = "")
{
    Tag tag(TT_SHORT);

    if (!name.empty())
        tag.setName(name);

    return tag.setShort(value);
}

inline Tag gInt(int32 value, const std::string& name = "")
{
    Tag tag(TT_INT);

    if (!name.empty())
        tag.setName(name);

    return tag.setInt(value);
}

inline Tag gLong(int64 value, const std::string& name = "")
{
    Tag tag(TT_LONG);

    if (!name.empty())
        tag.setName(name);

    return tag.setLong(value);
}

inline Tag gFloat(fp32 value, const std::string& name = "")
{
    Tag tag(TT_FLOAT);

    if (!name.empty())
        tag.setName(name);

    return tag.setFloat(value);
}

inline Tag gDouble(fp64 value, const std::string& name = "")
{
    Tag tag(TT_DOUBLE);

    if (!name.empty())
        tag.setName(name);

    return tag.setDouble(value);
}

inline Tag gString(const std::string& value, const std::string& name = "")
{
    Tag tag(TT_STRING);

    if (!name.empty())
        tag.setName(name);

    return tag.setString(value);
}

inline Tag gByteArray(const std::vector<byte>& value, const std::string& name = "")
{
    Tag tag(TT_BYTE_ARRAY);

    if (!name.empty())
        tag.setName(name);

    return tag.setByteArray(value);
}

inline Tag gIntArray(const std::vector<int32>& value, const std::string& name = "")
{
    Tag tag(TT_INT_ARRAY);

    if (!name.empty())
        tag.setName(name);

    return tag.setIntArray(value);
}

inline Tag gLongArray(const std::vector<int64>& value, const std::string& name = "")
{
    Tag tag(TT_LONG_ARRAY);

    if (!name.empty())
        tag.setName(name);

    return tag.setLongArray(value);
}

inline Tag gList(TagType dtype, const std::string& name = "")
{
    Tag tag(TagType::TT_LIST);
    tag.initListElementType(dtype);

    if (!name.empty())
        tag.setName(name);

    return tag;
}

inline Tag gCompound(const std::string& name = "")
{
    Tag tag(TT_COMPOUND);

    if (!name.empty())
        tag.setName(name);

    return tag;
}

}

#endif // !MCNBT_HPP
