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
#include <gzip/utils.hpp>
#include <gzip/compress.hpp>
#include <gzip/decompress.hpp>
#endif // MCNBT_USE_GZIP

// McNbt namespace.
namespace mcnbt
{

// Just for the intellisense better show "tip about namespace". :)

}

// Type alias.
namespace mcnbt
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
namespace mcnbt
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

std::string getTypeString(TagType type)
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
constexpr const char* _ERR_NOT_FIND = "Not find the specify member.";
constexpr const char* _ERR_OTHER = "The other error occured.";
constexpr const char* _ERR_UNDEFINED_TAGTYPE = "The tag type is undefined.";

}

// Core of read and write binary data.
namespace mcnbt
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
namespace mcnbt
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
        // List and Compound data
        std::vector<Tag>* d;
    };

public:
    Tag() = default;

    explicit Tag(TagType type, bool isPuredata = false) :
        type_(type), pureData_(isPuredata) {}

    Tag(TagType type, const std::string& name) :
        type_(type), name_(new std::string(name)) {}

    Tag(TagType type, std::istream& is, bool isBigEndian = false, size_t headerSize = 0) :
        type_(type)
    {
        std::stringstream ss;

        is.seekg(0, is.end);
        size_t size = is.tellg();
        is.seekg(0, is.beg);

        byte* buffer = new byte[size];

        is.read(buffer, size);
        std::string content = std::string(buffer, size);

        delete[] buffer;
    #ifdef MCNBT_USE_GZIP
        if (gzip::is_compressed(content.c_str(), size))
            content = gzip::decompress(content.c_str(), content.size());
    #endif // MCNBT_USE_GZIP
        ss << content;

        if (headerSize != 0) {
            byte* buffer = new byte[headerSize];

            is.read(buffer, headerSize);

            delete[] buffer;
        }

        loadFromStream_(ss, isBigEndian);
    }

    Tag(const Tag& other) :
        type_(other.type_), pureData_(other.pureData_), dtype_(other.dtype_)
    {
        if (other.type_ == END)
            return;

        if (!other.pureData_ && other.name_)
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
        type_(other.type_), pureData_(other.pureData_),
        dtype_(other.dtype_), name_(other.name_), data_(other.data_)
    {
        other.name_ = nullptr;
        other.data_.s = nullptr;
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

    Tag copy() { return Tag(*this); }

    bool isEnd() const { return type_ == END; }

    bool isByte() const { return type_ == BYTE; }

    bool isShort() const { return type_ == SHORT; }

    bool isInt() const { return type_ == INT; }

    bool isLong() const { return type_ == LONG; }

    bool isFloat() const { return type_ == FLOAT; }

    bool isDouble() const { return type_ == DOUBLE; }

    bool isString() const { return type_ == STRING; }

    bool isByteArray() const { return type_ == BYTE_ARRAY; }

    bool isIntArray() const { return type_ == INT_ARRAY; }

    bool isLongArray() const { return type_ == LONG_ARRAY; }

    bool isList() const { return type_ == LIST; }

    bool isCompound() const { return type_ == COMPOUND; }

    bool isInteger() const { return type_ == BYTE || type_ == SHORT || type_ == INT || type_ == LONG; }

    bool isFloatPoint() const { return type_ == FLOAT || type_ == DOUBLE; }

    bool isNum() const { return isInteger() || isFloatPoint(); }

    bool isArray() const { return type_ == BYTE_ARRAY || type_ == INT_ARRAY || type_ == LONG_ARRAY; }

    // @brief Whether the object is a List and Compound.
    // @return Return true if the object is a List or Compound if not return false.
    bool isComplex() const { return type_ == COMPOUND || type_ == LIST; }

    // @brief Whether the Compound has member with specify name, only valid when tag type is Compound.
    // @return Return true if the object is a Compound and conatins a specify member else return false.
    bool hasMember(const std::string& name) const
    {
        if (!isCompound())
            throw std::logic_error(_ERR_INCORRECT_TAGTYPE);

        if (data_.d == nullptr)
            return false;

        for (const auto& var : *data_.d) {
            if (var.name_ == nullptr)
                continue;

            if (*var.name_ == name)
                return true;
        }

        return false;
    }

    // @return Return a empty string if the object is not named or it is "pure data".
    std::string name() const
    {
        if (pureData_ || name_ == nullptr)
            return "";

        return *name_;
    }

    TagType type() const { return type_; }

    // @brief Get the element type of List, only valid when tag type is List.
    TagType dtype() const
    {
        if (!isList())
            throw std::logic_error(_ERR_INCORRECT_TAGTYPE);

        return dtype_;
    }

    // @return Return 0 if the object is not named or it is "pure data".
    int16 nameLen() const
    {
        if (pureData_ || name_ == nullptr)
            return 0;

        return static_cast<int16>(name_->size());
    }

    // @brief Get the string length, only valid when tag type is String.
    int32 stringLen() const
    {
        if (!isString())
            throw std::logic_error(_ERR_INCORRECT_TAGTYPE);

        if (data_.s == nullptr)
            return 0;

        return static_cast<int32>(data_.s->size());
    }

    // @brief Get the size of the container, only valid when tag type not is Number.
    size_t size() const
    {
        if (!isString() && !isArray() && !isComplex())
            throw std::logic_error(_ERR_INCORRECT_TAGTYPE);

        switch (type_) {
            case BYTE_ARRAY:
                return data_.bs == nullptr ? 0 : data_.bs->size();
            case STRING:
                return data_.s == nullptr ? 0 : data_.s->size();
            case LIST:
                return data_.d == nullptr ? 0 : data_.d->size();
            case COMPOUND:
                return data_.d == nullptr ? 0 : data_.d->size();
            case INT_ARRAY:
                return data_.is == nullptr ? 0 : data_.is->size();
            case LONG_ARRAY:
                return data_.ls == nullptr ? 0 : data_.ls->size();
            default:
                return 0;
        }
    }

    byte getByte() const
    {
        if (!isByte())
            throw std::logic_error(_ERR_INCORRECT_TAGTYPE);

        return data_.n.i8;
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

    int64 getLong() const
    {
        if (!isLong())
            throw std::logic_error(_ERR_INCORRECT_TAGTYPE);

        return data_.n.i64;
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

        if (data_.s == nullptr)
            return "";

        return *data_.s;
    }

    std::vector<byte>* getByteArray() const
    {
        if (!isByteArray())
            throw std::logic_error(_ERR_INCORRECT_TAGTYPE);

        return data_.bs;
    }

    std::vector<int32>* getIntArray() const
    {
        if (!isIntArray())
            throw std::logic_error(_ERR_INCORRECT_TAGTYPE);

        return data_.is;
    }

    std::vector<int64>* getLongArray() const
    {
        if (!isLongArray())
            throw std::logic_error(_ERR_INCORRECT_TAGTYPE);

        return data_.ls;
    }

    Tag& getMember(size_t pos)
    {
        if (!isComplex())
            throw std::logic_error(_ERR_INCORRECT_TAGTYPE);

        if (data_.d == nullptr || pos >= data_.d->size())
            throw std::range_error(_ERR_OVER_RANGE);

        return (*data_.d)[pos];
    }

    Tag& getMember(const std::string& name)
    {
        if (!isCompound())
            throw std::logic_error(_ERR_INCORRECT_TAGTYPE);

        if (data_.d == nullptr)
            throw std::logic_error(_ERR_NOT_FIND);

        for (auto& var : *data_.d) {
            if (var.name_ == nullptr)
                continue;

            if (*var.name_ == name)
                return var;
        }

        throw std::logic_error(_ERR_NOT_FIND);
    }

    Tag& front()
    {
        if (!isComplex())
            throw std::logic_error(_ERR_INCORRECT_TAGTYPE);

        if (data_.d == nullptr)
            throw std::range_error(_ERR_OVER_RANGE);

        return data_.d->front();
    }

    Tag& back()
    {
        if (!isComplex())
            throw std::logic_error(_ERR_INCORRECT_TAGTYPE);

        if (data_.d == nullptr)
            throw std::range_error(_ERR_OVER_RANGE);

        return data_.d->back();
    }

    // @note If the tag is "pure data" it do nothing.
    void setName(const std::string& name)
    {
        if (pureData_)
            return;

        if (name_) {
            *name_ = name;
            return;
        }

        name_ = new std::string(name);
    }

    void setByte(byte value)
    {
        if (!isByte())
            throw std::logic_error(_ERR_INCORRECT_TAGTYPE);

        data_.n.i8 = value;
    }

    void setShort(int16 value)
    {
        if (!isShort())
            throw std::logic_error(_ERR_INCORRECT_TAGTYPE);

        data_.n.i16 = value;
    }

    void setInt(int32 value)
    {
        if (!isInt())
            throw std::logic_error(_ERR_INCORRECT_TAGTYPE);

        data_.n.i32 = value;
    }

    void setLong(int64 value)
    {
        if (!isLong())
            throw std::logic_error(_ERR_INCORRECT_TAGTYPE);

        data_.n.i64 = value;
    }

    void setFloat(fp32 value)
    {
        if (!isFloat())
            throw std::logic_error(_ERR_INCORRECT_TAGTYPE);

        data_.n.f32 = value;
    }

    void setDouble(fp64 value)
    {
        if (!isDouble())
            throw std::logic_error(_ERR_INCORRECT_TAGTYPE);

        data_.n.f64 = value;
    }

    void setString(const std::string& value)
    {
        if (!isString())
            throw std::logic_error(_ERR_INCORRECT_TAGTYPE);

        if (data_.s) {
            *data_.s = value;
            return;
        }

        data_.s = new std::string(value);
    }

    void setByteArray(const std::vector<byte>& value)
    {
        if (!isByteArray())
            throw std::logic_error(_ERR_INCORRECT_TAGTYPE);

        if (data_.bs) {
            *data_.bs = value;
            return;
        }

        data_.bs = new std::vector<byte>(value);
    }

    void setIntArray(const std::vector<int32>& value)
    {
        if (!isIntArray())
            throw std::logic_error(_ERR_INCORRECT_TAGTYPE);

        if (data_.is) {
            *data_.is = value;
            return;
        }

        data_.is = new std::vector<int32>(value);
    }

    void setLongArray(const std::vector<int64>& value)
    {
        if (!isLongArray())
            throw std::logic_error(_ERR_INCORRECT_TAGTYPE);

        if (data_.ls) {
            *data_.ls = value;
            return;
        }

        data_.ls = new std::vector<int64>(value);
    }

    void addByte(byte value)
    {
        if (!isByteArray())
            throw std::logic_error(_ERR_INCORRECT_TAGTYPE);

        if (data_.bs == nullptr)
            data_.bs = new std::vector<byte>();

        data_.bs->push_back(value);
    }

    void addInt(int32 value)
    {
        if (!isIntArray())
            throw std::logic_error(_ERR_INCORRECT_TAGTYPE);

        if (data_.is == nullptr)
            data_.is = new std::vector<int32>();

        data_.is->push_back(value);
    }

    void addLong(int64 value)
    {
        if (!isLongArray())
            throw std::logic_error(_ERR_INCORRECT_TAGTYPE);

        if (data_.ls == nullptr)
            data_.ls = new std::vector<int64>();

        data_.ls->push_back(value);
    }

    void addMember(Tag& tag)
    {
        if (!isComplex() || (isList() && tag.type() != dtype()))
            throw std::logic_error(_ERR_INCORRECT_TAGTYPE);

        if (data_.d == nullptr)
            data_.d = new std::vector<Tag>();

        data_.d->emplace_back(std::move(tag));

        if (isList())
            data_.d->back().pureData_ = true;
        else
            data_.d->back().pureData_ = false;
    }

    // @overload
    // The rigth value overloaded version of the function addMember()
    void addMember(Tag&& tag)
    {
        if (!isComplex() || (isList() && tag.type() != dtype()))
            throw std::logic_error(_ERR_INCORRECT_TAGTYPE);

        if (data_.d == nullptr)
            data_.d = new std::vector<Tag>();

        data_.d->emplace_back(std::move(tag));

        if (isList())
            data_.d->back().pureData_ = true;
        else
            data_.d->back().pureData_ = false;
    }

    void removeByte(size_t pos)
    {
        if (!isByteArray())
            throw std::logic_error(_ERR_INCORRECT_TAGTYPE);

        if (data_.bs == nullptr || pos >= data_.bs->size())
            throw std::range_error(_ERR_OVER_RANGE);

        data_.bs->erase(data_.bs->begin() + pos);
    }

    void removeInt(size_t pos)
    {
        if (!isIntArray())
            throw std::logic_error(_ERR_INCORRECT_TAGTYPE);

        if (data_.is == nullptr || pos >= data_.is->size())
            throw std::range_error(_ERR_OVER_RANGE);

        data_.is->erase(data_.is->begin() + pos);
    }

    void removeLong(size_t pos)
    {
        if (!isLongArray())
            throw std::logic_error(_ERR_INCORRECT_TAGTYPE);

        if (data_.ls == nullptr || pos >= data_.ls->size())
            throw std::range_error(_ERR_OVER_RANGE);

        data_.ls->erase(data_.ls->begin() + pos);
    }

    void removeMember(size_t pos)
    {
        if (!isComplex())
            throw std::logic_error(_ERR_INCORRECT_TAGTYPE);

        if (data_.d == nullptr || pos >= data_.d->size())
            throw std::range_error(_ERR_OVER_RANGE);

        data_.d->erase(data_.d->begin() + pos);
    }

    void removeMember(const std::string& name)
    {
        if (!isCompound())
            throw std::logic_error(_ERR_INCORRECT_TAGTYPE);

        if (data_.d == nullptr)
            return;

        for (auto it = data_.d->begin(); it != data_.d->end(); ++it) {
            if (it->name_ == nullptr)
                continue;

            if (*(it->name_) == name) {
                data_.d->erase(it);
                return;
            }
        }
    }

#ifdef MCNBT_USE_GZIP
    // @brief Output the binay NBT tag to output stream.
    void write(std::ostream& os, bool isBigEndian = false, bool isCompressed = false) const
    {
        if (isCompressed) {
            std::stringstream ss;
            write_(ss, isBigEndian);
            os << gzip::compress(ss.str().c_str(), ss.str().size());
        } else {
            write_(os, isBigEndian);
        }
    }

    void write(const std::string& filename,
               bool isBigEndian = false, bool isCompressed = false) const
    {
        std::ofstream ofs(filename, std::ios_base::binary);

        if (ofs.is_open()) {
            write(ofs, isBigEndian, isCompressed);
        } else {
            throw std::runtime_error("Failed to open file: " + filename);
        }
    }
#else
    // @brief Output the binay NBT tag to output stream.
    void write(std::ostream& os, bool isBigEndian = false) const
    {
        write_(os, isBigEndian);
    }

    void write(const std::string& filename, bool isBigEndian = false) const
    {
        std::ofstream ofs(filename, std::ios_base::binary);

        if (ofs.is_open()) {
            write(ofs, isBigEndian);
        } else {
            throw std::runtime_error("Failed to open file: " + filename);
        }
    }
#endif // MCNBT_USE_GZIP

    // @brief Get the SNBT. (The string representation of NBT)
    std::string toSnbt(bool isIndented = true) const
    {
        const int indentStep = 4;
        static int indentSize = 0;
        std::string result;

        if (!pureData_ && name_ && !name_->empty())
            result += *name_ + ':';

        if (isIndented)
            result += ' ';

        auto getNumString = [&] () ->std::string {
            if (type_ == BYTE)
                return std::to_string(static_cast<int>(data_.n.i8)) + 'b';
            else if (type_ == SHORT)
                return std::to_string(static_cast<int>(data_.n.i16)) + 's';
            else if (type_ == INT)
                return std::to_string(data_.n.i32);
            else if (type_ == LONG)
                return std::to_string(data_.n.i64) + 'l';
            else if (type_ == FLOAT)
                return std::to_string(data_.n.f32) + 'f';
            else if (type_ == DOUBLE)
                return std::to_string(data_.n.f64) + 'd';
            else
                return "";
        };

        if (isEnd())
            return "";

        if (isNum())
            return result += getNumString();

        if (isString()) {
            result += '"';

            if (data_.s)
                result += *data_.s;

            result += '"';

            return result;
        }

        if (isArray()) {
            result += "[";

            indentSize += indentStep;

            if (isIndented)
                result += '\n' + std::string(indentSize, ' ');

            if (isByteArray())
                result += "B;";
            else if (isIntArray())
                result += "I;";
            else if (isLongArray())
                result += "L;";

            if (isByteArray() && data_.bs) {
                for (size_t i = 0; i < data_.bs->size(); i++) {
                    if (isIndented)
                        result += '\n' + std::string(indentSize, ' ');

                    result += std::to_string(static_cast<int>((*data_.bs)[i])) + 'b';

                    if (i != data_.bs->size() - 1)
                        result += ',';
                }
            } else if (isIntArray() && data_.is) {
                for (size_t i = 0; i < data_.is->size(); i++) {
                    if (isIndented)
                        result += '\n' + std::string(indentSize, ' ');

                    result += std::to_string(static_cast<int>((*data_.is)[i]));

                    if (i != data_.is->size() - 1)
                        result += ',';
                }
            } else if (isLongArray() && data_.ls) {
                for (size_t i = 0; i < data_.ls->size(); i++) {
                    if (isIndented)
                        result += '\n' + std::string(indentSize, ' ');

                    result += std::to_string(static_cast<int>((*data_.ls)[i])) + 'l';

                    if (i != data_.ls->size() - 1)
                        result += ',';
                }
            }

            indentSize = indentSize -= indentStep;

            if (indentSize < 0)
                indentSize = 0;

            if (isIndented)
                result += '\n' + std::string(indentSize, ' ');

            result += "]";

            return result;
        }

        if (isList()) {
            if (data_.d == nullptr || data_.d->empty())
                return result + "[]";

            result += "[";

            indentSize += indentStep;

            for (size_t i = 0; i < data_.d->size(); i++) {
                if (isIndented)
                    result += '\n' + std::string(indentSize, ' ');

                result += (*data_.d)[i].toSnbt(isIndented);

                if (i != data_.d->size() - 1)
                    result += ',';
            }
            indentSize = indentSize -= indentStep;
            if (indentSize < 0)
                indentSize = 0;

            if (isIndented)
                result += '\n' + std::string(indentSize, ' ');

            result += "]";

            return result;
        }

        if (isCompound()) {
            if (data_.d == nullptr || data_.d->empty())
                return result + "{}";

            result += "{";

            indentSize += indentStep;

            for (size_t i = 0; i < data_.d->size(); i++) {
                if (isIndented)
                    result += '\n' + std::string(indentSize, ' ');

                result += (*data_.d)[i].toSnbt(isIndented);

                if (i != data_.d->size() - 1)
                    result += ',';
            }

            indentSize = indentSize -= indentStep;

            if (indentSize < 0)
                indentSize = 0;

            if (isIndented)
                result += '\n' + std::string(indentSize, ' ');

            result += "}";

            return result;
        }

        return "";
    }

    Tag& operator=(const Tag& other)
    {
        this->~Tag();

        pureData_ = other.pureData_;
        type_ = other.type_;
        dtype_ = other.dtype_;

        if (other.type_ == END)
            return *this;

        if (!other.pureData_ && other.name_)
            name_ = new std::string(*other.name_);

        if (other.isNum())
            data_.n = other.data_.n;
        else if (other.isString() && other.data_.s)
            data_.s = new std::string(*other.data_.s);
        else if (other.isByteArray() && other.data_.bs)
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

        pureData_ = other.pureData_;
        type_ = other.type_;
        dtype_ = other.dtype_;

        name_ = other.name_;
        other.name_ = nullptr;

        data_ = other.data_;
        other.data_.s = nullptr;

        return *this;
    }

    Tag& operator[](size_t pos) { return getMember(pos); }

    Tag& operator[](const std::string& name) { return getMember(name); }

    Tag& operator<<(Tag& tag)
    {
        addMember(tag);
        return *this;
    }

    Tag& operator<<(Tag&& tag)
    {
        addMember(std::move(tag));
        return *this;
    }

    friend Tag gList(const std::string& name, TagType dtype);
    friend Tag gpList(TagType dtype);

private:
    Tag(TagType type, std::istream& is, bool isBigEndian, bool isPuredata) :
        type_(type), pureData_(isPuredata)
    {
        loadFromStream_(is, isBigEndian);
    }

    // @brief Get a NBT tag from binary input stream.
    void loadFromStream_(std::istream& is, bool isBigEndian)
    {
        construcPrework_(is, isBigEndian);

        if (isEnd())
            return;

        if (isNum())
            numConstruct_(is, isBigEndian);
        else if (isString())
            stringConstruct_(is, isBigEndian);
        else if (isArray())
            arrayConstruct_(is, isBigEndian);
        else if (isList())
            listConstruct_(is, isBigEndian);
        else if (isCompound())
            compoundConstruct_(is, isBigEndian);
        else
            throw std::runtime_error(_ERR_UNDEFINED_TAGTYPE);
    }

    // TODO
    // @brief Get a NBT tag from SNBT.
    void loadFromSnbt_(const std::string& snbt) {};

    void construcPrework_(std::istream& is, bool isBigEndian)
    {
        if (pureData_)
            return;

        TagType type = static_cast<TagType>(is.get());
        if (type != type_)
            throw std::runtime_error(_ERR_OTHER);

        int16 nameLen = _bytes2num<int16>(is, isBigEndian);

        if (name_) {
            delete name_;
            name_ = nullptr;
        }

        if (nameLen == 0)
            return;

        byte* bytes = new byte[nameLen];

        is.read(bytes, nameLen);
        name_ = new std::string();
        name_->assign(bytes, static_cast<size_t>(is.gcount()));

        delete[] bytes;
    }

    void numConstruct_(std::istream& is, bool isBigEndian)
    {
        switch (type_) {
            case BYTE:
                data_.n.i8 = _bytes2num<byte>(is, isBigEndian);
                break;
            case SHORT:
                data_.n.i16 = _bytes2num<int16>(is, isBigEndian);
                break;
            case INT:
                data_.n.i32 = _bytes2num<int32>(is, isBigEndian);
                break;
            case LONG:
                data_.n.i64 = _bytes2num<int64>(is, isBigEndian);
                break;
            case FLOAT:
                data_.n.f32 = _bytes2num<fp32>(is, isBigEndian);
                break;
            case DOUBLE:
                data_.n.f64 = _bytes2num<fp64>(is, isBigEndian);
                break;
            default:
                break;
        }
    }

    void stringConstruct_(std::istream& is, bool isBigEndian)
    {
        int16 strlen = _bytes2num<int16>(is, isBigEndian);

        if (data_.s) {
            delete data_.s;
            data_.s = nullptr;
        }

        if (strlen == 0)
            return;

        byte* bytes = new byte[strlen];

        is.read(bytes, strlen);
        data_.s = new std::string();
        data_.s->assign(bytes, static_cast<size_t>(is.gcount()));

        delete[] bytes;
    }

    void arrayConstruct_(std::istream& is, bool isBigEndian)
    {
        int32 dsize = _bytes2num<int32>(is, isBigEndian);

        if (isByteArray()) {
            if (data_.bs) {
                delete data_.bs;
                data_.bs = nullptr;
            }

            if (dsize == 0)
                return;

            data_.bs = new std::vector<byte>();
            data_.bs->reserve(dsize);
        }

        if (isIntArray()) {
            if (data_.is) {
                delete data_.is;
                data_.ls = nullptr;
            }

            if (dsize == 0)
                return;

            data_.is = new std::vector<int32>();
            data_.is->reserve(dsize);
        }

        if (isLongArray()) {
            if (data_.ls) {
                delete data_.ls;
                data_.ls = nullptr;
            }

            if (dsize == 0)
                return;

            data_.ls = new std::vector<int64>();
            data_.ls->reserve(dsize);
        }

        int32 size = 0;
        while (!is.eof() && size++ < dsize) {
            if (isByteArray())
                data_.bs->emplace_back(_bytes2num<byte>(is, isBigEndian));

            if (isIntArray())
                data_.is->emplace_back(_bytes2num<int32>(is, isBigEndian));

            if (isLongArray())
                data_.ls->emplace_back(_bytes2num<int64>(is, isBigEndian));
        }
    }

    void listConstruct_(std::istream& is, bool isBigEndian)
    {
        dtype_ = static_cast<TagType>(is.get());
        int32 dsize = _bytes2num<int32>(is, isBigEndian);

        if (data_.d) {
            delete data_.d;
            data_.d = nullptr;
        }

        if (dsize == 0)
            return;

        data_.d = new std::vector<Tag>();
        data_.d->reserve(dsize);

        int32 size = 0;
        while (!is.eof() && size++ < dsize)
            data_.d->emplace_back(Tag(dtype_, is, isBigEndian, true));
    }

    void compoundConstruct_(std::istream& is, bool isBigEndian)
    {
        if (data_.d)
            delete data_.d;

        data_.d = new std::vector<Tag>();

        while (!is.eof()) {
            TagType type = static_cast<TagType>(is.peek());

            if (type == END) {
                // Give up End tag and move stream point to next byte.
                is.get();
                break;
            }

            data_.d->emplace_back(Tag(type, is, isBigEndian, false));
        }
    }

    void write_(std::ostream& os, bool isBigEndian = false) const
    {
        if (!pureData_) {
            os.put(static_cast<byte>(type_));

            if (name_ == nullptr || name_->empty()) {
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
            if (data_.s == nullptr || data_.s->empty()) {
                _num2bytes<int16>(static_cast<int16>(0), os, isBigEndian);
                return;
            }

            _num2bytes<int16>(static_cast<int16>(data_.s->size()), os, isBigEndian);
            os.write(data_.s->c_str(), data_.s->size());

            return;
        }

        if (isByteArray()) {
            if (data_.bs == nullptr || data_.bs->empty()) {
                _num2bytes<int32>(static_cast<int32>(0), os, isBigEndian);
                return;
            }

            _num2bytes<int32>(static_cast<int32>(data_.bs->size()), os, isBigEndian);

            for (const auto& var : *data_.bs)
                os.put(var);

            return;
        }

        if (isIntArray()) {
            if (data_.is == nullptr || data_.is->empty()) {
                _num2bytes<int32>(static_cast<int32>(0), os, isBigEndian);
                return;
            }

            _num2bytes<int32>(static_cast<int32>(data_.is->size()), os, isBigEndian);

            for (const auto& var : *data_.is)
                _num2bytes<int32>(var, os, isBigEndian);

            return;
        }

        if (isLongArray()) {
            if (data_.ls == nullptr || data_.ls->empty()) {
                _num2bytes<int32>(static_cast<int32>(0), os, isBigEndian);
                return;
            }

            _num2bytes<int32>(static_cast<int32>(data_.ls->size()), os, isBigEndian);

            for (const auto& var : *data_.ls)
                _num2bytes<int64>(var, os, isBigEndian);

            return;
        }

        if (isList()) {
            if (data_.d == nullptr || data_.d->empty()) {
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
            if (data_.d == nullptr || data_.d->empty()) {
                os.put(END);
                return;
            }

            for (const auto& var : *data_.d)
                var.write_(os, isBigEndian);

            os.put(END);

            return;
        }
    }

private:
    // Whether to the object  is a "Base Tag", and the Base Tag has not description prefix.
    // (e.g. name, name length)
    // The all members of List is a "Base Tag".
    bool pureData_ = false;
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
namespace mcnbt
{

inline Tag gByte(const std::string& name, byte value)
{
    Tag tag(BYTE, name);
    tag.setByte(value);

    return tag;
}

inline Tag gpByte(byte value)
{
    Tag tag(BYTE, true);
    tag.setByte(value);

    return tag;
}

inline Tag gShort(const std::string& name, int16 value)
{
    Tag tag(SHORT, name);
    tag.setShort(value);

    return tag;
}

inline Tag gpShort(int16 value)
{
    Tag tag(SHORT, true);
    tag.setShort(value);

    return tag;
}

inline Tag gInt(const std::string& name, int32 value)
{
    Tag tag(INT, name);
    tag.setInt(value);

    return tag;
}

inline Tag gpInt(int32 value)
{
    Tag tag(INT, true);
    tag.setInt(value);

    return tag;
}

inline Tag gLong(const std::string& name, int64 value)
{
    Tag tag(LONG, name);
    tag.setLong(value);

    return tag;
}

inline Tag gpLong(int64 value)
{
    Tag tag(LONG, true);
    tag.setLong(value);

    return tag;
}

inline Tag gFloat(const std::string& name, fp32 value)
{
    Tag tag(FLOAT, name);
    tag.setFloat(value);

    return tag;
}

inline Tag gpFloat(fp32 value)
{
    Tag tag(FLOAT, true);
    tag.setFloat(value);

    return tag;
}

inline Tag gDouble(const std::string& name, fp64 value)
{
    Tag tag(DOUBLE, name);
    tag.setDouble(value);

    return tag;
}

inline Tag gpDouble(fp64 value)
{
    Tag tag(DOUBLE, true);
    tag.setDouble(value);

    return tag;
}

inline Tag gString(const std::string& name, const std::string& value)
{
    Tag tag(STRING, name);
    tag.setString(value);

    return tag;
}

inline Tag gpString(const std::string& value)
{
    Tag tag(STRING, true);
    tag.setString(value);

    return tag;
}

inline Tag gByteArray(const std::string& name = "")
{
    return Tag(BYTE_ARRAY, name);
}

inline Tag gpByteArray()
{
    return Tag(BYTE_ARRAY, true);
}

inline Tag gIntArray(const std::string& name = "")
{
    return Tag(INT_ARRAY, name);
}

inline Tag gpIntArray()
{
    return Tag(INT_ARRAY, true);
}

inline Tag gLongArray(const std::string& name = "")
{
    return Tag(LONG_ARRAY, name);
}

inline Tag gpLongArray()
{
    return Tag(LONG_ARRAY, true);
}

inline Tag gList(const std::string& name, TagType dtype)
{
    Tag tag(TagType::LIST, name);
    tag.dtype_ = dtype;

    return tag;
}

inline Tag gpList(TagType dtype)
{
    Tag tag(TagType::LIST, true);
    tag.dtype_ = dtype;

    return tag;
}

inline Tag gCompound(const std::string& name = "")
{
    return Tag(COMPOUND, name);
}

inline Tag gpCompound()
{
    return Tag(COMPOUND, true);
}

}

#endif // !MCNBT_HPP
