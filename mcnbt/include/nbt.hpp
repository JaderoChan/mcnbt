// Project "mcnbt" library by 頔珞JaderoChan
//
// MIT License
//
// Copyright (c) 2024 頔珞JaderoChan
//
// https://github.com/JaderoChan/mcnbt
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

#ifndef NBT_HPP
#define NBT_HPP

// C Header
#include <cstring> // std::memcpy()
#include <cstdint> // std::size_t

// Cpp Header
#include <vector>   // std::vector
#include <string>   // std::string
#include <fstream>  // std::istream, std::ostream, fstream
#include <sstream> // std::stringstream
#include <algorithm> // std::_reverser()

#ifndef NBT_ASSERT
#include <cassert>
#define NBT_ASSERT(x) assert(x)
#endif // !NBT_ASSERT

// Whether to use GZip to operation NBT.
#ifndef NBT_NOGZIP
#include <gzip/utils.hpp>
#include <gzip/compress.hpp>
#include <gzip/decompress.hpp>
#endif // !NBT_NOGZIP

#ifndef NBT_MACRO
#define NBT_MACRO

constexpr auto NBT_WARNHINT = "[NBT Warning] ";
constexpr auto NBT_ERRHINT = "[NBT Error] ";
#define NBT_TERMINATE std::terminate()

#include <iostream>
#define NBT_WARN(x) std::cerr << NBT_WARNHINT << x << std::endl
#define NBT_ERR(x) {std::cerr << NBT_WARNHINT << x << std::endl; NBT_TERMINATE;}
#define NBT_IFWARN(x,y) if(x) std::cerr << NBT_WARNHINT << y << std::endl
#define NBT_IFERR(x,y) if(x) {std::cerr << NBT_ERRHINT << y << std::endl; NBT_TERMINATE;}

#endif // NBT_MACRO

namespace Nbt
{

// Defines the size of the  buffer used by the _bytesToNum and _numToBytes functions.
static const int _BufferSize = 8;

static char _buffer[_BufferSize];

// @brief Reverse a C string.
// @param size The size of range that need reversed, and reverser all if the size is 0.
static void _reverse(char *str, std::size_t size = 0) {
    std::size_t _size = size;
    if (size == 0)
        _size = std::strlen(str);
    std::size_t i = 0;
    while (i < _size / 2) {
        char ch = str[i];
        str[i] = str[_size - 1 - i];
        str[_size - 1 - i] = ch;
        ++i;
    }
}

// @brief Check if system is big endian.
static inline bool _isBigEndian() {
    static bool inited = false;
    static bool isBig = false;
    if (inited)
        return isBig;
    int num = 1;
    char *numPtr = reinterpret_cast<char *>(&num);
    isBig = numPtr[0] == 0 ? true : false;
    inited = true;
    return isBig;
}

static const bool IsBigEndian = _isBigEndian();

// @brief Obtain bytes from input stream, and convert it to number.
// @param restoreCursor Whether to restore the input stream cursor after read.
// @return A number.
template<typename T>
T _bytesToNum(std::istream &is, bool isBigEndian = false, bool restoreCursor = false) {
    std::size_t size = sizeof(T);
    T result = T();
    auto begpos = is.tellg();
    is.read(_buffer, size);
    size = static_cast<std::size_t>(is.gcount());
    if (isBigEndian != IsBigEndian)
        Nbt::_reverse(_buffer, size);
    std::memcpy(&result, _buffer, size);
    if (restoreCursor)
        is.seekg(begpos);
    return result;
}

// @brief Convert the number to bytes, and output it to output stream.
template<typename T>
void _numToBytes(T num, std::ostream &os, bool isBigEndian = false) {
    std::size_t size = sizeof(T);
    std::memcpy(_buffer, &num, size);
    if (isBigEndian != IsBigEndian)
        Nbt::_reverse(_buffer, size);
    os.write(_buffer, size);
}

// NBT tag types
enum TagTypes : char
{
    End = 0,
    Byte = 1,
    Short = 2,
    Int = 3,
    Long = 4,
    Float = 5,
    Double = 6,
    ByteArray = 7,
    String = 8,
    List = 9,
    Compound = 10,
    IntArray = 11,
    LongArray = 12
};

// @brief Get the NBT tag string representation. (Usually used to debug)
static std::string getTagString(TagTypes type) {
    switch (type) {
        case End:
            return "End";
        case Byte:
            return "Byte";
        case Short:
            return "Short";
        case Int:
            return "Int";
        case Long:
            return "Long";
        case Float:
            return "Float";
        case Double:
            return "Double";
        case String:
            return "String";
        case ByteArray:
            return "Byte Array";
        case IntArray:
            return "Int Array";
        case LongArray:
            return "Long Array";
        case List:
            return "List";
        case Compound:
            return "Compound";
        default:
            return "Undefined";
    }
}

class Tag
{
public:
    typedef unsigned char uchar;
    typedef char byte;
    typedef char int8;
    typedef short int16;
    typedef int int32;
    typedef long long int64;
    typedef float fp32;
    typedef double fp64;

    // Num union contain interger and float point number.
    union Num
    {
        Num() : i64(0) {}
        int8 i8;    // char
        int16 i16;    // short
        int32 i32;    // int
        int64 i64;    // long long
        fp32 f32;    // float
        fp64 f64;    // double
    };

    // The data of NBT tag.
    union Data
    {
        Data() : s(nullptr) {}
        // Number data
        Num n;
        // String data
        std::string *s;
        // Byte Array data
        std::vector<byte> *bs;
        // Int Array data
        std::vector<int32> *is;
        // Long Array data
        std::vector<int64> *ls;
        // List and Compound data
        std::vector<Tag> *d;
    };

public:
    Tag(TagTypes type, bool isPuredata = false) :
        type_(type), isPuredata_(isPuredata),
        dtype_(End), name_(nullptr), data_(Data()) {}
    Tag(TagTypes type, const std::string &name) :
        type_(type), isPuredata_(false),
        dtype_(End), name_(new std::string(name)), data_(Data()) {}
    Tag(TagTypes type, std::istream &is, bool isBigEndian = false, std::size_t headerSize = 0) :
        type_(type), isPuredata_(false),
        dtype_(End), name_(nullptr), data_(Data())
    {
        std::stringstream ss;
        is.seekg(0, is.end);
        long long size = is.tellg();
        is.seekg(0, is.beg);
        char *buffer = new char[size];
        is.read(buffer, size);
        std::string content = std::string(buffer, size);
        delete[]buffer;
#ifndef NBT_NOGZIP
        if (gzip::is_compressed(content.c_str(), size))
            content = gzip::decompress(content.c_str(), content.size());
#endif // !NBT_NOGZIP
        ss << content;

        if (headerSize != 0) {
            char *buffer = new char[headerSize];
            is.read(buffer, headerSize);
            delete[] buffer;
        }

        _loadFromStream(ss, isBigEndian);
    }
    Tag(const Tag &rhs) :
        type_(rhs.type_), isPuredata_(rhs.isPuredata_),
        dtype_(rhs.dtype_), name_(nullptr), data_(Data())
    {
        if (rhs.type_ == End)
            return;
        if (!rhs.isPuredata_ && rhs.name_ != nullptr)
            name_ = new std::string(*rhs.name_);
        if (rhs.isNum())
            data_.n = rhs.data_.n;
        else if (rhs.isString() && rhs.data_.s != nullptr)
            data_.s = new std::string(*rhs.data_.s);
        else if (rhs.isArray() && rhs.data_.bs != nullptr)
            data_.bs = new std::vector<byte>(*rhs.data_.bs);
        else if (rhs.isIntArray() && rhs.data_.is != nullptr)
            data_.is = new std::vector<int32>(*rhs.data_.is);
        else if (rhs.isLongArray() && rhs.data_.ls != nullptr)
            data_.ls = new std::vector<int64>(*rhs.data_.ls);
        else if (rhs.isComplex() && rhs.data_.d != nullptr)
            data_.d = new std::vector<Tag>(*rhs.data_.d);
    }
    Tag(Tag &&rhs) noexcept :
        type_(rhs.type_), isPuredata_(rhs.isPuredata_),
        dtype_(rhs.dtype_), name_(rhs.name_), data_(rhs.data_)
    {
        rhs.name_ = nullptr;
        rhs.data_.s = nullptr;
    }
    ~Tag() {
        if (name_ != nullptr) {
            delete name_;
            name_ = nullptr;
        }
        if (isString() && data_.s != nullptr)
            delete data_.s;
        if (isByteArray() && data_.bs != nullptr)
            delete data_.bs;
        if (isIntArray() && data_.is != nullptr)
            delete data_.is;
        if (isLongArray() && data_.ls != nullptr)
            delete data_.ls;
        if (isComplex() && data_.d != nullptr)
            delete data_.d;
        data_.s = nullptr;
    }

    // @brief The default assignment case is copy(), and no explicit call is required.
    Tag copy() {
        Tag tag(*this);
        return tag;
    }
    // @brief Move an object containing a lot of data, and original object will be destroyed.
    Tag move() {
        return std::move(*this);
    }

    bool isEnd() const {
        return type_ == End;
    }
    bool isByte() const {
        return type_ == Byte;
    }
    bool isShort() const {
        return type_ == Short;
    }
    bool isInt() const {
        return type_ == Int;
    }
    bool isLong() const {
        return type_ == Long;
    }
    bool isFloat() const {
        return type_ == Float;
    }
    bool isDouble() const {
        return type_ == Double;
    }
    bool isString() const {
        return type_ == String;
    }
    bool isByteArray() const {
        return type_ == ByteArray;
    }
    bool isIntArray() const {
        return type_ == IntArray;
    }
    bool isLongArray() const {
        return type_ == LongArray;
    }
    bool isList() const {
        return type_ == List;
    }
    bool isCompound() const {
        return type_ == Compound;
    }
    bool isNum() const {
        return type_ == Byte || type_ == Short || type_ == Int ||
            type_ == Long || type_ == Float || type_ == Double;
    }
    bool isInteger() const {
        return type_ == Byte || type_ == Short || type_ == Int || type_ == Long;
    }
    bool isFloatPoint() const {
        return type_ == Float || type_ == Double;
    }
    bool isArray() const {
        return type_ == ByteArray || type_ == IntArray || type_ == LongArray;
    }
    // @brief Whether to the object is a List and Compound.
    // @return Return true if the object is a List or Compound if not return false.
    bool isComplex() const {
        return type_ == Compound || type_ == List;
    }

    // @brief Whether to the Compound has member with special name.
    // @return Return true if the object is a Compound and conatins a special member if not return false.
    bool hasMember(const std::string &name) const {
        if (!isCompound() || data_.d == nullptr)
            return false;

        for (auto &var : *data_.d) {
            if (var.name_ == nullptr)
                continue;
            if (*var.name_ == name)
                return true;
        }
        return false;
    }

    TagTypes type() const {
        return type_;
    }
    // @brief Get the member type of List.
    // @return Return End when object not a List.
    TagTypes dtype() const {
        if (!isList())
            return End;
        return dtype_;
    }
    // @return Return a empty string if the object is not named.
    std::string name() const {
        if (isPuredata_ || name_ == nullptr)
            return std::string();
        return *name_;
    }
    // @return Return 0 if the object is not named.
    int16 nameLen() const {
        if (isPuredata_ || name_ == nullptr)
            return 0;
        return static_cast<int16>(name_->size());
    }
    // @return Return 0 if the object is not a String.
    int32 stringLen() const {
        if (!isString() || data_.s == nullptr)
            return 0;
        return static_cast<int32>(data_.s->size());
    }
    // @brief Get the size of the container.
    // @return Return 0 if the object is a number or End.
    std::size_t size() const {
        if (isNum())
            return 0;
        else if (isString() && data_.s != nullptr)
            return data_.s->size();
        else if (isArray() && data_.bs != nullptr)
            return data_.bs->size();
        else if (isIntArray() && data_.is != nullptr)
            return data_.is->size();
        else if (isLongArray() && data_.ls != nullptr)
            return data_.ls->size();
        else if (isComplex() && data_.d != nullptr)
            return data_.d->size();
        return 0;
    }

    byte getByte() const {
        NBT_ASSERT(isByte());
        return data_.n.i8;
    }
    int16 getShort() const {
        NBT_ASSERT(isShort());
        return data_.n.i16;
    }
    int32 getInt() const {
        NBT_ASSERT(isInt());
        return data_.n.i32;
    }
    int64 getLong() const {
        NBT_ASSERT(isLong());
        return data_.n.i64;
    }
    fp32 getFloat() const {
        NBT_ASSERT(isFloat());
        return data_.n.f32;
    }
    fp64 getDouble() const {
        NBT_ASSERT(isDouble());
        return data_.n.f64;
    }
    std::string getString() const {
        NBT_ASSERT(isString());
        if (data_.s == nullptr)
            return std::string();
        return *data_.s;
    }
    std::vector<byte> *getByteArray() const {
        NBT_ASSERT(isByteArray());
        return data_.bs;
    }
    std::vector<int32> *getIntArray() const {
        NBT_ASSERT(isIntArray());
        return data_.is;
    }
    std::vector<int64> *getLongArray() const {
        NBT_ASSERT(isLongArray());
        return data_.ls;
    }
    // @return Return Failed Tag if the object is not a List or Compound.
    // @note If pos is out index range will be cause error.
    Tag &getMember(std::size_t pos) {
        NBT_ASSERT(isComplex() && data_.d != nullptr);
        NBT_ASSERT(pos < data_.d->size());

        NBT_IFERR(!isComplex() || data_.d == nullptr, "");
        NBT_IFERR(pos >= data_.d->size(), "The index is out range.");

        return (*data_.d)[pos];
    }
    // @return Return Failed Tag if the object is not a Compound or does not contains a special member
    Tag &getMember(const std::string &name) {
        NBT_ASSERT(isCompound() && data_.d != nullptr);

        NBT_IFERR(!isComplex() || data_.d == nullptr, "");

        for (auto &var : *data_.d) {
            if (var.name_ == nullptr)
                continue;
            if (*var.name_ == name)
                return var;
        }

        NBT_ERR("Not special name member.");
    }
    Tag &front() {
        NBT_ASSERT(isComplex() && data_.d != nullptr);

        NBT_IFERR(!isComplex() || data_.d == nullptr, "");

        return data_.d->front();
    }
    Tag &back() {
        NBT_ASSERT(isComplex() && data_.d != nullptr);

        NBT_IFERR(!isComplex() || data_.d == nullptr, "");

        return data_.d->back();
    }

    void setName(const std::string &name) {
        if (isPuredata_)
            return;
        if (name_ != nullptr) {
            *name_ = name;
            return;
        }
        name_ = new std::string(name);
    }
    void setByte(byte value) {
        if (!isByte())
            return;
        data_.n.i8 = value;
    }
    void setShort(int16 value) {
        if (!isShort())
            return;
        data_.n.i16 = value;
    }
    void setInt(int32 value) {
        if (!isInt())
            return;
        data_.n.i32 = value;
    }
    void setLong(int64 value) {
        if (!isLong())
            return;
        data_.n.i64 = value;
    }
    void setFloat(fp32 value) {
        if (!isFloat())
            return;
        data_.n.f32 = value;
    }
    void setDouble(fp64 value) {
        if (!isDouble())
            return;
        data_.n.f64 = value;
    }
    void setString(const std::string &value) {
        if (!isString())
            return;
        if (data_.s != nullptr) {
            *data_.s = value;
            return;
        }
        data_.s = new std::string(value);
    }
    void setByteArray(const std::vector<byte> &value) {
        if (!isByteArray())
            return;
        if (data_.bs != nullptr) {
            *data_.bs = value;
            return;
        }
        data_.bs = new std::vector<byte>(value);
    }
    void setIntArray(const std::vector<int32> &value) {
        if (!isIntArray())
            return;
        if (data_.is != nullptr) {
            *data_.is = value;
            return;
        }
        data_.is = new std::vector<int32>(value);
    }
    void setLongArray(const std::vector<int64> &value) {
        if (!isLongArray())
            return;
        if (data_.ls != nullptr) {
            *data_.ls = value;
            return;
        }
        data_.ls = new std::vector<int64>(value);
    }

    // @note Do nothing if the object is not a Byte Array.
    void addByte(byte value) {
        if (!isByteArray())
            return;
        if (data_.bs == nullptr)
            data_.bs = new std::vector<byte>();
        data_.bs->push_back(value);
    }
    // @note Do nothing if the object is not a Int Array.
    void addInt(int32 value) {
        if (!isIntArray())
            return;
        if (data_.is == nullptr)
            data_.is = new std::vector<int32>();
        data_.is->push_back(value);
    }
    // @note Do nothing if the object is not a Long Array.
    void addLong(int64 value) {
        if (!isLongArray())
            return;
        if (data_.ls == nullptr)
            data_.ls = new std::vector<int64>();
        data_.ls->push_back(value);
    }
    // @note Do nothing if the object is neither a List nor a Compound.
    void addMember(const Tag &tag) {
        if (!isComplex() || tag.type_ == End)
            return;
        if (data_.d == nullptr)
            data_.d = new std::vector<Tag>();
        data_.d->emplace_back(tag);
        if (isList())
            data_.d->back().isPuredata_ = true;
        else
            data_.d->back().isPuredata_ = false;
    }
    // @overload An rigth value overloaded version of the function addMember()
    void addMember(Tag &&tag) {
        if (!isComplex() || tag.type_ == End)
            return;
        if (data_.d == nullptr)
            data_.d = new std::vector<Tag>();
        data_.d->emplace_back(std::move(tag));
        if (isList())
            data_.d->back().isPuredata_ = true;
        else
            data_.d->back().isPuredata_ = false;
    }
    // @note Do nothing if the object is not a Byte Array.
    void removeByte(std::size_t pos) {
        if (!isByteArray() || data_.bs == nullptr || pos >= data_.bs->size())
            return;
        data_.bs->erase(data_.bs->begin() + pos);
    }
    // @note Do nothing if the object is not a Int Array.
    void removeInt(std::size_t pos) {
        if (!isIntArray() || data_.is == nullptr || pos >= data_.is->size())
            return;
        data_.is->erase(data_.is->begin() + pos);
    }
    // @note Do nothing if the object is not a Long Array.
    void removeLong(std::size_t pos) {
        if (!isLongArray() || data_.ls == nullptr || pos >= data_.ls->size())
            return;
        data_.ls->erase(data_.ls->begin() + pos);
    }
    // @note Do nothing if the object is neither a List nor a Compound.
    void removeMember(std::size_t pos) {
        if (!isComplex() || data_.d == nullptr)
            return;

        NBT_ASSERT(pos < data_.d->size());
        NBT_IFERR(pos >= data_.d->size(), "The index is out range.");

        data_.d->erase(data_.d->begin() + pos);
    }
    // @note Do nothing if the object not is a Compound.
    void removeMember(const std::string &name) {
        if (!isCompound() || data_.d == nullptr)
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

#ifndef NBT_NOGZIP
    // @brief Output the binay NBT tag to output stream.
    void write(std::ostream &os, bool isBigEndian = false, bool isCompressed = false) const {
        if (isCompressed) {
            std::stringstream ss;
            _write(ss, isBigEndian);
            os << gzip::compress(ss.str().c_str(), ss.str().size());
            return;
        }
        _write(os, isBigEndian);
    }
#else
    // @brief Output the binay NBT tag to output stream.
    void write(std::ostream &os, bool isBigEndian = false) const {
        _write(os, isBigEndian);
    }
#endif // !NBT_NOGZIP

    // @brief Get the SNBT. (The string representation of NBT)
    std::string toSnbt(bool isIndented = true) const {
        const int indentStep = 4;
        static int indentSize = 0;
        std::string result;

        if (!isPuredata_ && name_ != nullptr && !name_->empty())
            result += *name_ + ":";
        if (isIndented)
            result += " ";

        auto getNumString = [&] () ->std::string {
            if (type_ == Byte)
                return std::to_string(static_cast<int>(data_.n.i8)) + "b";
            else if (type_ == Short)
                return std::to_string(static_cast<int>(data_.n.i16)) + "s";
            else if (type_ == Int)
                return std::to_string(data_.n.i32);
            else if (type_ == Long)
                return std::to_string(data_.n.i64) + "l";
            else if (type_ == Float)
                return std::to_string(data_.n.f32) + "f";
            else if (type_ == Double)
                return std::to_string(data_.n.f64) + "d";
            else
                return "";
        };

        if (isEnd())
            return "";
        if (isNum())
            return result += getNumString();
        if (isString()) {
            result += "\"";
            if (data_.s != nullptr)
                result += *data_.s;
            result += "\"";
            return result;
        }
        if (isArray()) {
            result += "[";
            indentSize += indentStep;
            if (isIndented)
                result += "\n" + std::string(indentSize, ' ');
            if (isByteArray())
                result += "B;";
            else if (isIntArray())
                result += "I;";
            else if (isLongArray())
                result += "L;";
            else
                result;
            if (isByteArray() && data_.bs != nullptr) {
                for (int i = 0; i < data_.bs->size(); ++i) {
                    if (isIndented)
                        result += "\n" + std::string(indentSize, ' ');
                    result += std::to_string(static_cast<int>((*data_.bs)[i])) + "b";
                    if (i != data_.bs->size() - 1)
                        result += ",";
                }
            } else if (isIntArray() && data_.is != nullptr) {
                for (int i = 0; i < data_.is->size(); ++i) {
                    if (isIndented)
                        result += "\n" + std::string(indentSize, ' ');
                    result += std::to_string(static_cast<int>((*data_.is)[i]));
                    if (i != data_.is->size() - 1)
                        result += ",";
                }
            } else if (isLongArray() && data_.ls != nullptr) {
                for (int i = 0; i < data_.ls->size(); ++i) {
                    if (isIndented)
                        result += "\n" + std::string(indentSize, ' ');
                    result += std::to_string(static_cast<int>((*data_.ls)[i])) + "l";
                    if (i != data_.ls->size() - 1)
                        result += ",";
                }
            }
            indentSize = indentSize -= indentStep;
            if (indentSize < 0)
                indentSize = 0;
            if (isIndented)
                result += "\n" + std::string(indentSize, ' ');
            result += "]";
            return result;
        }
        if (isList()) {
            if (data_.d == nullptr || data_.d->empty()) {
                result += "[]";
                return result;
            }
            result += "[";
            indentSize += indentStep;
            for (int i = 0; i < data_.d->size(); ++i) {
                if (isIndented)
                    result += "\n" + std::string(indentSize, ' ');
                result += (*data_.d)[i].toSnbt(isIndented);
                if (i != data_.d->size() - 1)
                    result += ",";
            }
            indentSize = indentSize -= indentStep;
            if (indentSize < 0)
                indentSize = 0;
            if (isIndented)
                result += "\n" + std::string(indentSize, ' ');
            result += "]";
            return result;
        }
        if (isCompound()) {
            if (data_.d == nullptr || data_.d->empty()) {
                result += "{}";
                return result;
            }
            result += "{";
            indentSize += indentStep;
            for (int i = 0; i < data_.d->size(); ++i) {
                if (isIndented)
                    result += "\n" + std::string(indentSize, ' ');
                result += (*data_.d)[i].toSnbt(isIndented);
                if (i != data_.d->size() - 1)
                    result += ",";
            }
            indentSize = indentSize -= indentStep;
            if (indentSize < 0)
                indentSize = 0;
            if (isIndented)
                result += "\n" + std::string(indentSize, ' ');
            result += "}";
            return result;
        }
        return "";
    }

    Tag &operator=(const Tag &rhs) {
        this->~Tag();

        isPuredata_ = rhs.isPuredata_;
        type_ = rhs.type_;
        dtype_ = rhs.dtype_;

        if (rhs.type_ == End)
            return *this;
        if (!rhs.isPuredata_ && rhs.name_ != nullptr)
            name_ = new std::string(*rhs.name_);
        if (rhs.isNum())
            data_.n = rhs.data_.n;
        else if (rhs.isString() && rhs.data_.s != nullptr)
            data_.s = new std::string(*rhs.data_.s);
        else if (rhs.isByteArray() && rhs.data_.bs != nullptr)
            data_.bs = new std::vector<byte>(*rhs.data_.bs);
        else if (rhs.isIntArray() && rhs.data_.is != nullptr)
            data_.is = new std::vector<int32>(*rhs.data_.is);
        else if (rhs.isLongArray() && rhs.data_.ls != nullptr)
            data_.ls = new std::vector<int64>(*rhs.data_.ls);
        else if (rhs.isComplex() && rhs.data_.d != nullptr)
            data_.d = new std::vector<Tag>(*rhs.data_.d);

        return *this;
    }
    Tag &operator=(Tag &&rhs) noexcept {
        this->~Tag();

        isPuredata_ = rhs.isPuredata_;
        type_ = rhs.type_;
        dtype_ = rhs.dtype_;
        name_ = rhs.name_;
        rhs.name_ = nullptr;
        data_ = rhs.data_;
        rhs.data_.s = nullptr;

        return *this;
    }
    Tag &operator[](std::size_t pos) {
        return getMember(pos);
    }
    Tag &operator[](const std::string &name) {
        return getMember(name);
    }
    Tag &operator<<(const Tag &tag) {
        addMember(tag);
        return *this;
    }
    Tag &operator<<(Tag &&tag) {
        addMember(std::move(tag));
        return *this;
    }

    friend Tag gList(const std::string &name, TagTypes dtype);
    friend Tag gpList(TagTypes dtype);

private:
    Tag(TagTypes type, std::istream &is, bool isBigEndian, bool isPuredata) :
        type_(type), isPuredata_(isPuredata),
        dtype_(End), name_(nullptr), data_(Data())
    {
        _loadFromStream(is, isBigEndian);
    }
    // @brief Get a NBT tag from binary input stream.
    void _loadFromStream(std::istream &is, bool isBigEndian) {
        _construcPrework(is, isBigEndian);
        if (isEnd())
            return;
        if (isNum())
            _numConstruct(is, isBigEndian);
        else if (isString())
            _stringConstruct(is, isBigEndian);
        else if (isArray())
            _arrayConstruct(is, isBigEndian);
        else if (isList())
            _listConstruct(is, isBigEndian);
        else if (isCompound())
            _compoundConstruct(is, isBigEndian);
        else
            NBT_ERR(std::to_string(static_cast<int>(type_)) + " Tag type is undefined!");
    }
    // @brief Get a NBT tag from SNBT.
    void _loadFromSnbt(const std::string &snbt) {};
    void _write(std::ostream &os, bool isBigEndian = false) const {
        if (!isPuredata_) {
            os.put(static_cast<int8>(type_));
            if (name_ == nullptr || name_->empty()) {
                _numToBytes<int16>(static_cast<int16>(0), os, isBigEndian);
            } else {
                _numToBytes<int16>(static_cast<int16>(name_->size()), os, isBigEndian);
                os.write(name_->c_str(), name_->size());
            }
        }

        if (isEnd()) {
            os.put(End);
            return;
        }
        if (isByte()) {
            os.put(data_.n.i8);
            return;
        }
        if (isShort()) {
            _numToBytes<int16>(data_.n.i16, os, isBigEndian);
            return;
        }
        if (isInt()) {
            _numToBytes<int32>(data_.n.i32, os, isBigEndian);
            return;
        }
        if (isLong()) {
            _numToBytes<int64>(data_.n.i64, os, isBigEndian);
            return;
        }
        if (isFloat()) {
            _numToBytes<fp32>(data_.n.f32, os, isBigEndian);
            return;
        }
        if (isDouble()) {
            _numToBytes<fp64>(data_.n.f64, os, isBigEndian);
            return;
        }
        if (isString()) {
            if (data_.s == nullptr || data_.s->empty()) {
                _numToBytes<int16>(static_cast<int16>(0), os, isBigEndian);
                return;
            }
            _numToBytes<int16>(static_cast<int16>(data_.s->size()), os, isBigEndian);
            os.write(data_.s->c_str(), data_.s->size());
            return;
        }
        if (isByteArray()) {
            if (data_.bs == nullptr || data_.bs->empty()) {
                _numToBytes<int32>(static_cast<int32>(0), os, isBigEndian);
                return;
            }
            _numToBytes<int32>(static_cast<int32>(data_.bs->size()), os, isBigEndian);
            for (auto &var : *data_.bs)
                os.put(var);
            return;
        }
        if (isIntArray()) {
            if (data_.is == nullptr || data_.is->empty()) {
                _numToBytes<int32>(static_cast<int32>(0), os, isBigEndian);
                return;
            }
            _numToBytes<int32>(static_cast<int32>(data_.is->size()), os, isBigEndian);
            for (auto &var : *data_.is)
                _numToBytes<int32>(var, os, isBigEndian);
            return;
        }
        if (isLongArray()) {
            if (data_.ls == nullptr || data_.ls->empty()) {
                _numToBytes<int32>(static_cast<int32>(0), os, isBigEndian);
                return;
            }
            _numToBytes<int32>(static_cast<int32>(data_.ls->size()), os, isBigEndian);
            for (auto &var : *data_.ls)
                _numToBytes<int64>(var, os, isBigEndian);
            return;
        }
        if (isList()) {
            if (data_.d == nullptr || data_.d->empty()) {
                os.put(static_cast<int8>(End));
                _numToBytes<int32>(static_cast<int32>(0), os, isBigEndian);
                return;
            }
            os.put(static_cast<int8>(dtype_));
            _numToBytes<int32>(static_cast<int32>(data_.d->size()), os, isBigEndian);
            for (auto &var : *data_.d)
                var._write(os, isBigEndian);
            return;
        }
        if (isCompound()) {
            if (data_.d == nullptr || data_.d->empty()) {
                os.put(End);
                return;
            }
            for (auto &var : *data_.d)
                var._write(os, isBigEndian);
            os.put(End);
            return;
        }
    }
    void _construcPrework(std::istream &is, bool isBigEndian) {
        if (isPuredata_)
            return;

        TagTypes type = static_cast<TagTypes>(is.get());
        NBT_ASSERT(type == type_);

        int16 nameLen = _bytesToNum<int16>(is, isBigEndian);

        if (name_ != nullptr) {
            delete name_;
            name_ = nullptr;
        }
        if (nameLen == 0)
            return;

        name_ = new std::string();
        char *bytes = new char[nameLen];
        is.read(bytes, nameLen);
        name_->assign(bytes, static_cast<std::size_t>(is.gcount()));
        delete[] bytes;
    }
    void _numConstruct(std::istream &is, bool isBigEndian) {
        switch (type_) {
            case Byte:
                data_.n.i8 = _bytesToNum<int8>(is, isBigEndian);
                break;
            case Short:
                data_.n.i16 = _bytesToNum<int16>(is, isBigEndian);
                break;
            case Int:
                data_.n.i32 = _bytesToNum<int32>(is, isBigEndian);
                break;
            case Long:
                data_.n.i64 = _bytesToNum<int64>(is, isBigEndian);
                break;
            case Float:
                data_.n.f32 = _bytesToNum<fp32>(is, isBigEndian);
                break;
            case Double:
                data_.n.f64 = _bytesToNum<fp64>(is, isBigEndian);
                break;
            default:
                break;
        }
    }
    void _stringConstruct(std::istream &is, bool isBigEndian) {
        int16 strlen = _bytesToNum<int16>(is, isBigEndian);

        if (data_.s != nullptr) {
            delete data_.s;
            data_.s = nullptr;
        }
        if (strlen == 0)
            return;

        data_.s = new std::string();
        char *bytes = new char[strlen];
        is.read(bytes, strlen);
        data_.s->assign(bytes, static_cast<std::size_t>(is.gcount()));
        delete[] bytes;
    }
    void _arrayConstruct(std::istream &is, bool isBigEndian) {
        int32 dsize = _bytesToNum<int32>(is, isBigEndian);

        if (isByteArray()) {
            if (data_.bs != nullptr) {
                delete data_.bs;
                data_.bs = nullptr;
            }
            if (dsize == 0)
                return;
            data_.bs = new std::vector<int8>();
            data_.bs->reserve(dsize);
        }
        if (isIntArray()) {
            if (data_.is != nullptr) {
                delete data_.is;
                data_.ls = nullptr;
            }
            if (dsize == 0)
                return;
            data_.is = new std::vector<int32>();
            data_.is->reserve(dsize);
        }
        if (isLongArray()) {
            if (data_.ls != nullptr) {
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
                data_.bs->emplace_back(_bytesToNum<int8>(is, isBigEndian));
            if (isIntArray())
                data_.is->emplace_back(_bytesToNum<int32>(is, isBigEndian));
            if (isLongArray())
                data_.ls->emplace_back(_bytesToNum<int64>(is, isBigEndian));
        }
    }
    void _listConstruct(std::istream &is, bool isBigEndian) {
        dtype_ = static_cast<TagTypes>(is.get());
        int32 dsize = _bytesToNum<int32>(is, isBigEndian);

        if (data_.d != nullptr) {
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
    void _compoundConstruct(std::istream &is, bool isBigEndian) {
        if (data_.d != nullptr)
            delete data_.d;
        data_.d = new std::vector<Tag>();

        while (!is.eof()) {
            TagTypes type = static_cast<TagTypes>(is.peek());
            if (type == End) {
                is.get();         // Give up End tag and move stream point to next byte.
                break;
            }
            data_.d->emplace_back(Tag(type, is, isBigEndian, false));
        }
    }

private:
    // Whether to the object  is a "Base Tag", and the Base Tag has not description prefix. (e.g. name, name length)
    // The all members of List is a "Base Tag".
    bool isPuredata_;
    TagTypes type_;
    TagTypes dtype_;
    std::string *name_;
    Data data_;
};

inline static Tag gByte(const std::string &name, char value) {
    Tag tag(TagTypes::Byte, name);
    tag.setByte(value);
    return tag;
}

inline static Tag gpByte(char value) {
    Tag tag(TagTypes::Byte, true);
    tag.setByte(value);
    return tag;
}

inline static Tag gShort(const std::string &name, short value) {
    Tag tag(TagTypes::Short, name);
    tag.setShort(value);
    return tag;
}

inline static Tag gpShort(short value) {
    Tag tag(TagTypes::Short, true);
    tag.setShort(value);
    return tag;
}

inline static Tag gInt(const std::string &name, int value) {
    Tag tag(TagTypes::Int, name);
    tag.setInt(value);
    return tag;
}

inline static Tag gpInt(int value) {
    Tag tag(TagTypes::Int, true);
    tag.setInt(value);
    return tag;
}

inline static Tag gLong(const std::string &name, long long value) {
    Tag tag(TagTypes::Long, name);
    tag.setLong(value);
    return tag;
}

inline static Tag gpLong(long long value) {
    Tag tag(TagTypes::Long, true);
    tag.setLong(value);
    return tag;
}

inline static Tag gFloat(const std::string &name, float value) {
    Tag tag(TagTypes::Float, name);
    tag.setFloat(value);
    return tag;
}

inline static Tag gpFloat(float value) {
    Tag tag(TagTypes::Float, true);
    tag.setFloat(value);
    return tag;
}

inline static Tag gDouble(const std::string &name, double value) {
    Tag tag(TagTypes::Double, name);
    tag.setDouble(value);
    return tag;
}

inline static Tag gpDouble(double value) {
    Tag tag(TagTypes::Double, true);
    tag.setDouble(value);
    return tag;
}

inline static Tag gString(const std::string &name, const std::string &value) {
    Tag tag(TagTypes::String, name);
    tag.setString(value);
    return tag;
}

inline static Tag gpString(const std::string &value) {
    Tag tag(TagTypes::String, true);
    tag.setString(value);
    return tag;
}

inline static Tag gByteArray(const std::string &name = std::string()) {
    return Tag(TagTypes::ByteArray, name);
}

inline static Tag gpByteArray() {
    return Tag(TagTypes::ByteArray, true);
}

inline static Tag gIntArray(const std::string &name = std::string()) {
    return Tag(TagTypes::IntArray, name);
}

inline static Tag gpIntArray() {
    return Tag(TagTypes::IntArray, true);
}

inline static Tag gLongArray(const std::string &name = std::string()) {
    return Tag(TagTypes::LongArray, name);
}

inline static Tag gpLongArray() {
    return Tag(TagTypes::LongArray, true);
}

inline static Tag gList(const std::string &name, TagTypes dtype) {
    Tag tag(TagTypes::List, name);
    tag.dtype_ = dtype;
    return tag;
}

inline static Tag gpList(TagTypes dtype) {
    Tag tag(TagTypes::List, true);
    tag.dtype_ = dtype;
    return tag;
}

inline static Tag gCompound(const std::string &name = std::string()) {
    return Tag(Compound, name);
}

inline static Tag gpCompound() {
    return Tag(Compound, true);
}

}

#endif // !NBT_HPP
