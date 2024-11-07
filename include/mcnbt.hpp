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
    END = 0,
    BYTE = 1,
    SHORT = 2,
    INT = 3,
    LONG = 4,
    FLOAT = 5,
    DOUBLE = 6,
    BYTE_ARRAY = 7,
    STRING = 8,
    LIST = 9,
    COMPOUND = 10,
    INT_ARRAY = 11,
    LONG_ARRAY = 12
};

inline std::string getTagTypeString(TagType type)
{
    switch (type) {
        case END:
            return "End";
        case BYTE:
            return "Byte";
        case SHORT:
            return "Short";
        case INT:
            return "Int";
        case LONG:
            return "Long";
        case FLOAT:
            return "Float";
        case DOUBLE:
            return "Double";
        case BYTE_ARRAY:
            return "Byte Array";
        case STRING:
            return "String";
        case LIST:
            return "List";
        case COMPOUND:
            return "Compound";
        case INT_ARRAY:
            return "Int Array";
        case LONG_ARRAY:
            return "Long Array";
        default:
            return "";
    }
}

// Error messages.
constexpr const char* _ERR_INCORRECT_TAGTYPE = "The error tag type.";
constexpr const char* _ERR_OVER_RANGE = "The index is out of range.";
constexpr const char* _ERR_NO_SPECIFY_MEMBER = "Not find the specify member.";
constexpr const char* _ERR_UNDEFINED_TAGTYPE = "The tag type is undefined.";
constexpr const char* _ERR_OPERATE_UNINIT_LIST = "Can't operate (get, add, remove, etc.) an uninitialized list.";
constexpr const char* _ERR_REPEAT_INIT_LIST = "Can't initialize a list that has already been initialized.";
constexpr const char* _ERR_OTHER = "The other error occured.";

constexpr unsigned int _INDENT_SIZE = 2;
static const std::string _INDENT_STR(_INDENT_SIZE, ' ');

}

// Core of read and write binary data.
namespace nbt
{

// @brief Reverse a C style string.
// @param size The size of range that need reversed, and reverser all if the size is 0.
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

// @brief Check whther the system is big endian.
inline bool _isBigEndian()
{
    static bool isInited = false;
    static bool isBigEndian = false;

    if (isInited)
        return isBigEndian;

    int32 num = 1;
    char* numPtr = reinterpret_cast<char*>(&num);
    isBigEndian = numPtr[0] == 0 ? true : false;

    isInited = true;

    return isBigEndian;
}

// @brief Obtain bytes from input stream, and convert it to number.
// @param resumeCursor Whether to resume the input stream cursor position after read.
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

// @brief Convert the number to bytes, and output it to output stream.
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
    // Num union contain interger and float point number.
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

    // The data of NBT tag.
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

    Tag(TagType type) :
        type_(type) {}

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

    static Tag fromFile(const std::string& filename, bool isBigEndian, size_t headerSize = 0)
    {
        std::ifstream is(filename, std::ios::binary);

        if (!is.is_open())
            throw std::runtime_error("Failed to open file: " + filename);

        return fromBinStream(is, isBigEndian, headerSize);
    }

    static Tag fromSnbt(const std::string& snbt)
    {
        return fromSnbt_(snbt);
    }

    Tag copy() { return Tag(*this); }

    bool isEnd() const { return type_ == END; }

    bool isByte() const { return type_ == BYTE; }

    bool isShort() const { return type_ == SHORT; }

    bool isInt() const { return type_ == INT; }

    bool isLong() const { return type_ == LONG; }

    bool isFloat() const { return type_ == FLOAT; }

    bool isDouble() const { return type_ == DOUBLE; }

    bool isInteger() const { return type_ == BYTE || type_ == SHORT || type_ == INT || type_ == LONG; }

    bool isFloatPoint() const { return type_ == FLOAT || type_ == DOUBLE; }

    bool isNum() const { return isInteger() || isFloatPoint(); }

    bool isString() const { return type_ == STRING; }

    bool isByteArray() const { return type_ == BYTE_ARRAY; }

    bool isIntArray() const { return type_ == INT_ARRAY; }

    bool isLongArray() const { return type_ == LONG_ARRAY; }

    bool isArray() const { return type_ == BYTE_ARRAY || type_ == INT_ARRAY || type_ == LONG_ARRAY; }

    bool isList() const { return type_ == LIST; }

    bool isCompound() const { return type_ == COMPOUND; }

    bool isComplex() const { return type_ == LIST || type_ == COMPOUND; }

    bool isInitializedList() const
    {
        if (!isList())
            throw std::logic_error(_ERR_INCORRECT_TAGTYPE);

        return dtype_ != END;
    }

    TagType type() const { return type_; }

    TagType listElementType() const
    {
        if (!isList())
            throw std::logic_error(_ERR_INCORRECT_TAGTYPE);

        return dtype_;
    }

    std::string name() const
    {
        if (isListElement_)
            throw std::logic_error(_ERR_INCORRECT_TAGTYPE);

        if (!name_)
            return "";

        return *name_;
    }

    int16 nameLen() const
    {
        if (isListElement_)
            throw std::logic_error(_ERR_INCORRECT_TAGTYPE);

        if (!name_)
            return 0;

        return static_cast<int16>(name_->size());
    }

    int32 stringLen() const
    {
        if (!isString())
            throw std::logic_error(_ERR_INCORRECT_TAGTYPE);

        if (!data_.s)
            return 0;

        return static_cast<int32>(data_.s->size());
    }

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

        throw std::logic_error(_ERR_INCORRECT_TAGTYPE);
    }

    bool empty() const { return size() == 0; }

    byte getByte() const
    {
        if (!isByte())
            throw std::logic_error(_ERR_INCORRECT_TAGTYPE);

        return data_.n.i8;
    }

    byte getByte(size_t pos) const
    {
        if (!isByteArray())
            throw std::logic_error(_ERR_INCORRECT_TAGTYPE);

        if (!data_.bs || pos >= data_.bs->size())
            throw std::range_error(_ERR_OVER_RANGE);

        return (*data_.bs)[pos];
    }

    int16 getShort() const
    {
        if (!isShort())
            throw std::logic_error(_ERR_INCORRECT_TAGTYPE);

        return data_.n.i16;
    }

    int32 getInt() const
    {
        if (!isInt())
            throw std::logic_error(_ERR_INCORRECT_TAGTYPE);

        return data_.n.i32;
    }

    int32 getInt(size_t pos) const
    {
        if (!isIntArray())
            throw std::logic_error(_ERR_INCORRECT_TAGTYPE);

        if (!data_.is || pos >= data_.is->size())
            throw std::range_error(_ERR_OVER_RANGE);

        return (*data_.is)[pos];
    }

    int64 getLong() const
    {
        if (!isLong())
            throw std::logic_error(_ERR_INCORRECT_TAGTYPE);

        return data_.n.i64;
    }

    int64 getLong(size_t pos) const
    {
        if (!isLongArray())
            throw std::logic_error(_ERR_INCORRECT_TAGTYPE);

        if (!data_.ls || pos >= data_.ls->size())
            throw std::range_error(_ERR_OVER_RANGE);

        return (*data_.ls)[pos];
    }

    fp32 getFloat() const
    {
        if (!isFloat())
            throw std::logic_error(_ERR_INCORRECT_TAGTYPE);

        return data_.n.f32;
    }

    fp64 getDouble() const
    {
        if (!isDouble())
            throw std::logic_error(_ERR_INCORRECT_TAGTYPE);

        return data_.n.f64;
    }

    std::string getString() const
    {
        if (!isString())
            throw std::logic_error(_ERR_INCORRECT_TAGTYPE);

        if (!data_.s)
            return "";

        return *data_.s;
    }

    std::vector<byte>* getByteArray() const
    {
        if (!isByteArray())
            throw std::logic_error(_ERR_INCORRECT_TAGTYPE);

        return data_.bs;
    }

    byte frontByte() const
    {
        if (!isByteArray())
            throw std::logic_error(_ERR_INCORRECT_TAGTYPE);

        if (!data_.bs || data_.bs->empty())
            throw std::range_error(_ERR_OVER_RANGE);

        return data_.bs->front();
    }

    byte backByte() const
    {
        if (!isByteArray())
            throw std::logic_error(_ERR_INCORRECT_TAGTYPE);

        if (!data_.bs || data_.bs->empty())
            throw std::range_error(_ERR_OVER_RANGE);

        return data_.bs->back();
    }

    std::vector<int32>* getIntArray() const
    {
        if (!isIntArray())
            throw std::logic_error(_ERR_INCORRECT_TAGTYPE);

        return data_.is;
    }

    int32 frontInt() const
    {
        if (!isIntArray())
            throw std::logic_error(_ERR_INCORRECT_TAGTYPE);

        if (!data_.is || data_.is->empty())
            throw std::range_error(_ERR_OVER_RANGE);

        return data_.is->front();
    }

    int32 backInt() const
    {
        if (!isIntArray())
            throw std::logic_error(_ERR_INCORRECT_TAGTYPE);

        if (!data_.is || data_.is->empty())
            throw std::range_error(_ERR_OVER_RANGE);

        return data_.is->back();
    }

    std::vector<int64>* getLongArray() const
    {
        if (!isLongArray())
            throw std::logic_error(_ERR_INCORRECT_TAGTYPE);

        return data_.ls;
    }

    int64 frontLong() const
    {
        if (!isLongArray())
            throw std::logic_error(_ERR_INCORRECT_TAGTYPE);

        if (!data_.ls || data_.ls->empty())
            throw std::range_error(_ERR_OVER_RANGE);

        return data_.ls->front();
    }

    int64 backLong() const
    {
        if (!isLongArray())
            throw std::logic_error(_ERR_INCORRECT_TAGTYPE);

        if (!data_.ls || data_.ls->empty())
            throw std::range_error(_ERR_OVER_RANGE);

        return data_.ls->back();
    }

    bool hasTag(const std::string& name) const
    {
        if (!isCompound())
            throw std::logic_error(_ERR_INCORRECT_TAGTYPE);

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

    std::vector<Tag>* getTags() const
    {
        if (!isComplex())
            throw std::logic_error(_ERR_INCORRECT_TAGTYPE);

        return data_.d;
    }

    Tag& getTag(size_t pos)
    {
        if (!isComplex())
            throw std::logic_error(_ERR_INCORRECT_TAGTYPE);

        if (isList() && dtype_ == END)
            throw std::logic_error(_ERR_OPERATE_UNINIT_LIST);

        if (!data_.d || pos >= data_.d->size())
            throw std::range_error(_ERR_OVER_RANGE);

        return (*data_.d)[pos];
    }

    Tag& getTag(const std::string& name)
    {
        if (!isCompound())
            throw std::logic_error(_ERR_INCORRECT_TAGTYPE);

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

    Tag& frontTag()
    {
        if (!isComplex())
            throw std::logic_error(_ERR_INCORRECT_TAGTYPE);

        if (isList() && dtype_ == END)
            throw std::logic_error(_ERR_OPERATE_UNINIT_LIST);

        if (!data_.d || data_.d->empty())
            throw std::range_error(_ERR_OVER_RANGE);

        return data_.d->front();
    }

    Tag& backTag()
    {
        if (!isComplex())
            throw std::logic_error(_ERR_INCORRECT_TAGTYPE);

        if (isList() && dtype_ == END)
            throw std::logic_error(_ERR_OPERATE_UNINIT_LIST);

        if (!data_.d || data_.d->empty())
            throw std::range_error(_ERR_OVER_RANGE);

        return data_.d->back();
    }

    Tag& setName(const std::string& name)
    {
        if (isListElement_)
            throw std::logic_error(_ERR_INCORRECT_TAGTYPE);

        if (name_)
            *name_ = name;
        else
            name_ = new std::string(name);

        return *this;
    }

    // @brief Initalize the element tag type of a list.
    Tag& initListElementType(TagType type)
    {
        if (!isList())
            throw std::logic_error(_ERR_INCORRECT_TAGTYPE);

        if (dtype_ != END)
            throw std::logic_error(_ERR_REPEAT_INIT_LIST);

        dtype_ = type;

        return *this;
    }

    Tag& resetList()
    {
        if (!isList())
            throw std::logic_error(_ERR_INCORRECT_TAGTYPE);

        if (dtype_ == END)
            return *this;

        if (data_.d) {
            delete data_.d;
            data_.d = nullptr;
        }

        dtype_ = END;

        return *this;
    }

    Tag& setByte(byte value)
    {
        if (!isByte())
            throw std::logic_error(_ERR_INCORRECT_TAGTYPE);

        data_.n.i8 = value;

        return *this;
    }

    Tag& setShort(int16 value)
    {
        if (!isShort())
            throw std::logic_error(_ERR_INCORRECT_TAGTYPE);

        data_.n.i16 = value;

        return *this;
    }

    Tag& setInt(int32 value)
    {
        if (!isInt())
            throw std::logic_error(_ERR_INCORRECT_TAGTYPE);

        data_.n.i32 = value;

        return *this;
    }

    Tag& setLong(int64 value)
    {
        if (!isLong())
            throw std::logic_error(_ERR_INCORRECT_TAGTYPE);

        data_.n.i64 = value;

        return *this;
    }

    Tag& setFloat(fp32 value)
    {
        if (!isFloat())
            throw std::logic_error(_ERR_INCORRECT_TAGTYPE);

        data_.n.f32 = value;

        return *this;
    }

    Tag& setDouble(fp64 value)
    {
        if (!isDouble())
            throw std::logic_error(_ERR_INCORRECT_TAGTYPE);

        data_.n.f64 = value;

        return *this;
    }

    Tag& setString(const std::string& value)
    {
        if (!isString())
            throw std::logic_error(_ERR_INCORRECT_TAGTYPE);

        if (data_.s)
            *data_.s = value;
        else
            data_.s = new std::string(value);

        return *this;
    }

    Tag& setByteArray(const std::vector<byte>& value)
    {
        if (!isByteArray())
            throw std::logic_error(_ERR_INCORRECT_TAGTYPE);

        if (data_.bs)
            *data_.bs = value;
        else
            data_.bs = new std::vector<byte>(value);

        return *this;
    }

    Tag& setIntArray(const std::vector<int32>& value)
    {
        if (!isIntArray())
            throw std::logic_error(_ERR_INCORRECT_TAGTYPE);

        if (data_.is)
            *data_.is = value;
        else
            data_.is = new std::vector<int32>(value);

        return *this;
    }

    Tag& setLongArray(const std::vector<int64>& value)
    {
        if (!isLongArray())
            throw std::logic_error(_ERR_INCORRECT_TAGTYPE);

        if (data_.ls)
            *data_.ls = value;
        else
            data_.ls = new std::vector<int64>(value);

        return *this;
    }

    Tag& addByte(byte value)
    {
        if (!isByteArray())
            throw std::logic_error(_ERR_INCORRECT_TAGTYPE);

        if (!data_.bs)
            data_.bs = new std::vector<byte>();

        data_.bs->push_back(value);

        return *this;
    }

    Tag& addInt(int32 value)
    {
        if (!isIntArray())
            throw std::logic_error(_ERR_INCORRECT_TAGTYPE);

        if (!data_.is)
            data_.is = new std::vector<int32>();

        data_.is->push_back(value);

        return *this;
    }

    Tag& addLong(int64 value)
    {
        if (!isLongArray())
            throw std::logic_error(_ERR_INCORRECT_TAGTYPE);

        if (!data_.ls)
            data_.ls = new std::vector<int64>();

        data_.ls->push_back(value);

        return *this;
    }

    Tag& addTag(Tag& tag)
    {
        if (!isComplex())
            throw std::logic_error(_ERR_INCORRECT_TAGTYPE);

        if (isList() && tag.type() != dtype_)
            throw std::logic_error(_ERR_INCORRECT_TAGTYPE);

        if (isList() && dtype_ == END)
            throw std::logic_error(_ERR_OPERATE_UNINIT_LIST);

        if (!data_.d)
            data_.d = new std::vector<Tag>();

        data_.d->emplace_back(std::move(tag));

        if (isList()) {
            data_.d->back().isListElement_ = true;
            if (data_.d->back().name_) {
                delete data_.d->back().name_;
                data_.d->back().name_ = nullptr;
            }
        } else {
            data_.d->back().isListElement_ = false;
        }

        return *this;
    }

    // @overload
    // The rigth value overloaded version of the function addMember()
    Tag& addTag(Tag&& tag)
    {
        if (!isComplex())
            throw std::logic_error(_ERR_INCORRECT_TAGTYPE);

        if (isList() && tag.type() != dtype_)
            throw std::logic_error(_ERR_INCORRECT_TAGTYPE);

        if (isList() && dtype_ == END)
            throw std::logic_error(_ERR_OPERATE_UNINIT_LIST);

        if (!data_.d)
            data_.d = new std::vector<Tag>();

        data_.d->emplace_back(std::move(tag));

        if (isList()) {
            data_.d->back().isListElement_ = true;
            if (data_.d->back().name_) {
                delete data_.d->back().name_;
                data_.d->back().name_ = nullptr;
            }
        } else {
            data_.d->back().isListElement_ = false;
        }

        return *this;
    }

    Tag& remove(size_t pos)
    {
        if (isByteArray()) {
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
            if (isList() && dtype_ == END)
                throw std::logic_error(_ERR_OPERATE_UNINIT_LIST);

            if (!data_.d || pos >= data_.d->size())
                throw std::range_error(_ERR_OVER_RANGE);

            data_.d->erase(data_.d->begin() + pos);
        } else {
            throw std::logic_error(_ERR_INCORRECT_TAGTYPE);
        }

        return *this;
    }

    Tag& removeFront()
    {
        if (isByteArray()) {
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
            if (isList() && dtype_ == END)
                throw std::logic_error(_ERR_OPERATE_UNINIT_LIST);

            if (!data_.d || data_.d->empty())
                throw std::range_error(_ERR_OVER_RANGE);

            data_.d->erase(data_.d->begin());
        } else {
            throw std::logic_error(_ERR_INCORRECT_TAGTYPE);
        }

        return *this;
    }

    Tag& remove(const std::string& name)
    {
        if (!isCompound())
            throw std::logic_error(_ERR_INCORRECT_TAGTYPE);

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
            if (isList() && dtype_ == END)
                throw std::logic_error(_ERR_OPERATE_UNINIT_LIST);

            if (data_.d)
                data_.d->clear();
        } else {
            throw std::logic_error(_ERR_INCORRECT_TAGTYPE);
        }

        return *this;
    }

#ifdef MCNBT_USE_GZIP
    // @brief Output the binay NBT tag to output stream.
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
    // @brief Output the binay NBT tag to output stream.
    void write(std::ostream& os, bool isBigEndian) const
    {
        write_(os, isBigEndian);
    }

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

    // @brief Get the SNBT. (The string representation of NBT)
    std::string toSnbt(bool isIndented = true) const
    {
        std::string key;

        if (!isListElement_ && name_ && !name_->empty())
            key = *name_ + (isIndented ? ": " : ":");

        if (isEnd())
            return "";

        if (isByte())
            return key + std::to_string(static_cast<int>(data_.n.i8)) + 'b';

        if (isShort())
            return key + std::to_string(static_cast<int>(data_.n.i16)) +'s';

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
            std::string result = key + "[";
            result += (isIndented ? "\n" + _INDENT_STR : "") + "B;";

            for (const auto& var : *data_.bs)
                result += (isIndented ? "\n" + _INDENT_STR : "") + std::to_string(static_cast<int>(var)) + "b,";

            if (result.back() == ',')
                result.pop_back();

            result += isIndented ? "\n]" : "]";

            return result;
        }

        if (isIntArray()) {
            if (!data_.is)
                return key + "[I;]";

            // If has indent add the newline character and indent string.
            std::string result = key + "[";
            result += (isIndented ? "\n" + _INDENT_STR : "") + "I;";

            for (const auto& var : *data_.is)
                result += (isIndented ? "\n" + _INDENT_STR : "") + std::to_string(var) + ",";

            if (result.back() == ',')
                result.pop_back();

            result += isIndented ? "\n]" : "]";

            return result;
        }

        if (isLongArray()) {
            if (!data_.ls)
                return key + "[L;]";

            // If has indent add the newline character and indent string.
            std::string result = key + "[";
            result += (isIndented ? "\n" + _INDENT_STR : "") + "L;";

            for (const auto& var : *data_.ls)
                result += (isIndented ? "\n" + _INDENT_STR : "") + std::to_string(var) + "l,";

            if (result.back() == ',')
                result.pop_back();

            result += isIndented ? "\n]" : "]";

            return result;
        }

        if (isList()) {
            if (!data_.d)
                return key + "[]";

            // If has indent add the newline character and indent string.
            std::string result = key + "[";

            for (const auto& var : *data_.d)
                result += (isIndented ? "\n" + _INDENT_STR : "") + var.toSnbt(isIndented) + ",";

            if (result.back() == ',')
                result.pop_back();

            result += isIndented ? "\n]" : "]";

            return result;
        }

         if (isCompound()) {
            if (!data_.d)
                return key + "{}";

            // If has indent add the newline character and indent string.
            std::string result = key + "{";

            if (data_.d) {
                for (const auto& var : *data_.d)
                    result += (isIndented ? "\n" + _INDENT_STR : "") + var.toSnbt(isIndented) + ",";
            }

            if (result.back() == ',')
                result.pop_back();

            result += isIndented ? "\n}" : "}";

            return result;
        }

        throw std::runtime_error(_ERR_UNDEFINED_TAGTYPE);
    }

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

        if (isListElement_ && name_) {
            delete name_;
            name_ = nullptr;
        }

        return *this;
    }

    Tag& operator[](size_t pos) { return getTag(pos); }

    Tag& operator[](const std::string& name) { return getTag(name); }

    Tag& operator<<(Tag& tag)
    {
        return addTag(tag);
    }

    Tag& operator<<(Tag&& tag)
    {
        return addTag(std::move(tag));
    }

private:
    friend Tag gList(TagType dtype, const std::string& name);

    // @param tagType If the param isListElement is false, ignore it.
    // If the param isListElement is true, the tagType must be the same as the parent tag.
    static Tag fromBinStream_(std::istream& is, bool isBigEndian, bool isListElement, int tagType = -1)
    {
        Tag tag;
        tag.isListElement_ = isListElement;

        if (!isListElement)
            tagType = is.get();
        tag.type_ = static_cast<TagType>(tagType);

        if (tagType == END)
            return tag;

        // If the nbt tag not is a list elment read it's name.
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
        if (tagType == BYTE)
            tag.data_.n.i8 = _bytes2num<byte>(is, isBigEndian);

        if (tagType == SHORT)
            tag.data_.n.i16 = _bytes2num<int16>(is, isBigEndian);

        if (tagType == INT)
            tag.data_.n.i32 = _bytes2num<int32>(is, isBigEndian);

        if (tagType == LONG)
            tag.data_.n.i64 = _bytes2num<int64>(is, isBigEndian);

        if (tagType == FLOAT)
            tag.data_.n.f32 = _bytes2num<fp32>(is, isBigEndian);

        if (tagType == DOUBLE)
            tag.data_.n.f64 = _bytes2num<fp64>(is, isBigEndian);

        if (tagType == STRING) {
            int16 strlen = _bytes2num<int16>(is, isBigEndian);

            if (strlen != 0) {
                byte* bytes = new byte[strlen];

                is.read(bytes, strlen);
                tag.data_.s = new std::string();
                tag.data_.s->assign(bytes, static_cast<size_t>(is.gcount()));

                delete[] bytes;
            }
        }

        if (tagType == BYTE_ARRAY) {
            int32 dsize = _bytes2num<int32>(is, isBigEndian);

            if (dsize != 0) {
                tag.data_.bs = new std::vector<byte>();
                tag.data_.bs->reserve(dsize);

                for (int32 i = 0; i < dsize; ++i)
                    tag.addByte(_bytes2num<byte>(is, isBigEndian));
            }
        }

        if (tagType == INT_ARRAY) {
            int32 dsize = _bytes2num<int32>(is, isBigEndian);

            if (dsize != 0) {
                tag.data_.is = new std::vector<int32>();
                tag.data_.is->reserve(dsize);

                for (int32 i = 0; i < dsize; ++i)
                    tag.addInt(_bytes2num<int32>(is, isBigEndian));
            }
        }

        if (tagType == LONG_ARRAY) {
            int32 dsize = _bytes2num<int32>(is, isBigEndian);

            if (dsize != 0) {
                tag.data_.ls = new std::vector<int64>();
                tag.data_.ls->reserve(dsize);

                for (int32 i = 0; i < dsize; ++i)
                    tag.addLong(_bytes2num<int64>(is, isBigEndian));
            }
        }

        if (tagType == LIST) {
            tag.dtype_ = static_cast<TagType>(is.get());
            int32 dsize = _bytes2num<int32>(is, isBigEndian);

            if (dsize != 0) {
                tag.data_.d = new std::vector<Tag>();
                tag.data_.d->reserve(dsize);

                for (int32 i = 0; i < dsize; ++i)
                    tag.addTag(fromBinStream_(is, isBigEndian, true, tag.dtype_));
            }
        }

        if (tagType == COMPOUND) {
            while (!is.eof()) {
                if (is.peek() == END) {
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
    // @brief Get a NBT tag from SNBT.
    static Tag fromSnbt_(const std::string& snbt) {
        return Tag();
    };

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
            os.put(END);
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
                os.put(static_cast<byte>(END));
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
                os.put(END);
                return;
            }

            for (const auto& var : *data_.d)
                var.write_(os, isBigEndian);

            os.put(END);

            return;
        }
    }

    bool isListElement_ = false;
    // The tag type.
    TagType type_ = END;
    // The tag type of element, only used to List.
    TagType dtype_ = END;
    // The tag "key".
    std::string* name_ = nullptr;
    // The tag "value".
    Data data_;
};

}

// Faster way.
namespace nbt
{

inline Tag gByte(byte value, const std::string& name = "")
{
    Tag tag(BYTE);

    if (!name.empty())
        tag.setName(name);

    return tag.setByte(value);
}

inline Tag gShort(int16 value, const std::string& name = "")
{
    Tag tag(SHORT);

    if (!name.empty())
        tag.setName(name);

    return tag.setShort(value);
}

inline Tag gInt(int32 value, const std::string& name = "")
{
    Tag tag(INT);

    if (!name.empty())
        tag.setName(name);

    return tag.setInt(value);
}

inline Tag gLong(int64 value, const std::string& name = "")
{
    Tag tag(LONG);

    if (!name.empty())
        tag.setName(name);

    return tag.setLong(value);
}

inline Tag gFloat(fp32 value, const std::string& name = "")
{
    Tag tag(FLOAT);

    if (!name.empty())
        tag.setName(name);

    return tag.setFloat(value);
}

inline Tag gDouble(fp64 value, const std::string& name = "")
{
    Tag tag(DOUBLE);

    if (!name.empty())
        tag.setName(name);

    return tag.setDouble(value);
}

inline Tag gString(const std::string& value, const std::string& name = "")
{
    Tag tag(STRING);

    if (!name.empty())
        tag.setName(name);

    return tag.setString(value);
}

inline Tag gByteArray(const std::vector<byte>& value, const std::string& name = "")
{
    Tag tag(BYTE_ARRAY);

    if (!name.empty())
        tag.setName(name);

    return tag.setByteArray(value);
}

inline Tag gIntArray(const std::vector<int32>& value, const std::string& name = "")
{
    Tag tag(INT_ARRAY);

    if (!name.empty())
        tag.setName(name);

    return tag.setIntArray(value);
}

inline Tag gLongArray(const std::vector<int64>& value, const std::string& name = "")
{
    Tag tag(LONG_ARRAY);

    if (!name.empty())
        tag.setName(name);

    return tag.setLongArray(value);
}

inline Tag gList(TagType dtype, const std::string& name = "")
{
    Tag tag(TagType::LIST);
    tag.dtype_ = dtype;

    if (!name.empty())
        tag.setName(name);

    return tag;
}

inline Tag gCompound(const std::string& name = "")
{
    Tag tag(COMPOUND);

    if (!name.empty())
        tag.setName(name);

    return tag;
}

}

#endif // !MCNBT_HPP
