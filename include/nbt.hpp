// The "mcnbt" library written in c++.
//
// Webs: https://github.com/JaderoChan/mcnbt
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

#ifndef NBT_HPP
#define NBT_HPP

#include <cstring> // std::memcpy()
#include <cstdint> // std::size_t

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
#include <gzip/utils.h>
#include <gzip/compress.h>
#include <gzip/decompress.h>
#endif // !NBT_NOGZIP

#if __cplusplus >= 201703L
#define NBT_STATIC static inline
#else
#define NBT_STATIC static
#endif

#ifndef NBT_MACRO
#define NBT_MACRO

#define NBT_WARNHINT "[NBT Warning] "
#define NBT_ERRHINT "[NBT Error] "
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
constexpr const int _BufferSize = 8;

NBT_STATIC char _buffer[_BufferSize];

// @brief Reverse a C string.
// @param size The size of range that need reversed, and reverser all if the size is 0.
inline void _reverse(char *str, std::size_t size = 0) {
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
inline bool _isBigEndian() {
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

}

namespace Nbt
{

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
        int8 i8;
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
        mType(type), mPureData(isPuredata),
        mDataType(End), mName(nullptr), mData(Data()) {}
    Tag(TagTypes type, const std::string &name) :
        mType(type), mPureData(false),
        mDataType(End), mName(new std::string(name)), mData(Data()) {}
    Tag(TagTypes type, std::istream &is, bool isBigEndian = false, std::size_t headerSize = 0) :
        mType(type), mPureData(false),
        mDataType(End), mName(nullptr), mData(Data())
    {
        std::stringstream ss;
        is.seekg(0, is.end);
        std::size_t size = is.tellg();
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

        loadFromStream(ss, isBigEndian);
    }
    Tag(const Tag &rhs) :
        mType(rhs.mType), mPureData(rhs.mPureData),
        mDataType(rhs.mDataType), mName(nullptr), mData(Data())
    {
        if (rhs.mType == End)
            return;
        if (!rhs.mPureData && rhs.mName != nullptr)
            mName = new std::string(*rhs.mName);
        if (rhs.isNum())
            mData.n = rhs.mData.n;
        else if (rhs.isString() && rhs.mData.s != nullptr)
            mData.s = new std::string(*rhs.mData.s);
        else if (rhs.isArray() && rhs.mData.bs != nullptr)
            mData.bs = new std::vector<byte>(*rhs.mData.bs);
        else if (rhs.isIntArray() && rhs.mData.is != nullptr)
            mData.is = new std::vector<int32>(*rhs.mData.is);
        else if (rhs.isLongArray() && rhs.mData.ls != nullptr)
            mData.ls = new std::vector<int64>(*rhs.mData.ls);
        else if (rhs.isComplex() && rhs.mData.d != nullptr)
            mData.d = new std::vector<Tag>(*rhs.mData.d);
    }
    Tag(Tag &&rhs) noexcept :
        mType(rhs.mType), mPureData(rhs.mPureData),
        mDataType(rhs.mDataType), mName(rhs.mName), mData(rhs.mData)
    {
        rhs.mName = nullptr;
        rhs.mData.s = nullptr;
    }
    ~Tag() {
        if (mName != nullptr) {
            delete mName;
            mName = nullptr;
        }
        if (isString() && mData.s != nullptr)
            delete mData.s;
        else if (isByteArray() && mData.bs != nullptr)
            delete mData.bs;
        else if (isIntArray() && mData.is != nullptr)
            delete mData.is;
        else if (isLongArray() && mData.ls != nullptr)
            delete mData.ls;
        else if (isComplex() && mData.d != nullptr)
            delete mData.d;
        mData.s = nullptr;
    }

    Tag copy() {
        return Tag(*this);
    }

    bool isEnd() const {
        return mType == End;
    }
    bool isByte() const {
        return mType == Byte;
    }
    bool isShort() const {
        return mType == Short;
    }
    bool isInt() const {
        return mType == Int;
    }
    bool isLong() const {
        return mType == Long;
    }
    bool isFloat() const {
        return mType == Float;
    }
    bool isDouble() const {
        return mType == Double;
    }
    bool isString() const {
        return mType == String;
    }
    bool isByteArray() const {
        return mType == ByteArray;
    }
    bool isIntArray() const {
        return mType == IntArray;
    }
    bool isLongArray() const {
        return mType == LongArray;
    }
    bool isList() const {
        return mType == List;
    }
    bool isCompound() const {
        return mType == Compound;
    }
    bool isNum() const {
        return mType == Byte || mType == Short || mType == Int ||
            mType == Long || mType == Float || mType == Double;
    }
    bool isInteger() const {
        return mType == Byte || mType == Short || mType == Int || mType == Long;
    }
    bool isFloatPoint() const {
        return mType == Float || mType == Double;
    }
    bool isArray() const {
        return mType == ByteArray || mType == IntArray || mType == LongArray;
    }
    // @brief Whether to the object is a List and Compound.
    // @return Return true if the object is a List or Compound if not return false.
    bool isComplex() const {
        return mType == Compound || mType == List;
    }

    // @brief Whether to the Compound has member with special name.
    // @return Return true if the object is a Compound and conatins a special member if not return false.
    bool hasMember(const std::string &name) const {
        if (!isCompound() || mData.d == nullptr)
            return false;

        for (auto &var : *mData.d) {
            if (var.mName == nullptr)
                continue;
            if (*var.mName == name)
                return true;
        }
        return false;
    }

    // @return Return a empty string if the object is not named.
    std::string name() const {
        if (mPureData || mName == nullptr)
            return std::string();
        return *mName;
    }
    TagTypes type() const {
        return mType;
    }
    // @brief Get the member type of List.
    // @return Return End when object not a List.
    TagTypes dtype() const {
        if (!isList())
            return End;
        return mDataType;
    }
    // @return Return 0 if the object is not named.
    int16 nameLen() const {
        if (mPureData || mName == nullptr)
            return 0;
        return static_cast<int16>(mName->size());
    }
    // @return Return 0 if the object is not a String.
    int32 stringLen() const {
        if (!isString() || mData.s == nullptr)
            return 0;
        return static_cast<int32>(mData.s->size());
    }
    // @brief Get the size of the container.
    // @return Return 0 if the object is a number or End.
    std::size_t size() const {
        if (isNum())
            return 0;
        else if (isString() && mData.s != nullptr)
            return mData.s->size();
        else if (isArray() && mData.bs != nullptr)
            return mData.bs->size();
        else if (isIntArray() && mData.is != nullptr)
            return mData.is->size();
        else if (isLongArray() && mData.ls != nullptr)
            return mData.ls->size();
        else if (isComplex() && mData.d != nullptr)
            return mData.d->size();
        return 0;
    }

    byte getByte() const {
        NBT_ASSERT(isByte());
        return mData.n.i8;
    }
    int16 getShort() const {
        NBT_ASSERT(isShort());
        return mData.n.i16;
    }
    int32 getInt() const {
        NBT_ASSERT(isInt());
        return mData.n.i32;
    }
    int64 getLong() const {
        NBT_ASSERT(isLong());
        return mData.n.i64;
    }
    fp32 getFloat() const {
        NBT_ASSERT(isFloat());
        return mData.n.f32;
    }
    fp64 getDouble() const {
        NBT_ASSERT(isDouble());
        return mData.n.f64;
    }
    std::string getString() const {
        NBT_ASSERT(isString());
        if (mData.s == nullptr)
            return std::string();
        return *mData.s;
    }
    std::vector<byte> *getByteArray() const {
        NBT_ASSERT(isByteArray());
        return mData.bs;
    }
    std::vector<int32> *getIntArray() const {
        NBT_ASSERT(isIntArray());
        return mData.is;
    }
    std::vector<int64> *getLongArray() const {
        NBT_ASSERT(isLongArray());
        return mData.ls;
    }
    // @return Return Failed Tag if the object is not a List or Compound.
    // @note If pos is out index range will be cause error.
    Tag &getMember(std::size_t pos) {
        NBT_ASSERT(isComplex() && mData.d != nullptr);
        NBT_ASSERT(pos < mData.d->size());

        NBT_IFERR(!isComplex() || mData.d == nullptr, "");
        NBT_IFERR(pos >= mData.d->size(), "The index is out range.");

        return (*mData.d)[pos];
    }
    // @return Return Failed Tag if the object is not a Compound or does not contains a special member
    Tag &getMember(const std::string &name) {
        NBT_ASSERT(isCompound() && mData.d != nullptr);

        NBT_IFERR(!isComplex() || mData.d == nullptr, "");

        for (auto &var : *mData.d) {
            if (var.mName == nullptr)
                continue;
            if (*var.mName == name)
                return var;
        }

        NBT_ERR("Not special name member.");
    }
    Tag &front() {
        NBT_ASSERT(isComplex() && mData.d != nullptr);

        NBT_IFERR(!isComplex() || mData.d == nullptr, "");

        return mData.d->front();
    }
    Tag &back() {
        NBT_ASSERT(isComplex() && mData.d != nullptr);

        NBT_IFERR(!isComplex() || mData.d == nullptr, "");

        return mData.d->back();
    }

    void setName(const std::string &name) {
        if (mPureData)
            return;
        if (mName != nullptr) {
            *mName = name;
            return;
        }
        mName = new std::string(name);
    }
    void setByte(byte value) {
        if (!isByte())
            return;
        mData.n.i8 = value;
    }
    void setShort(int16 value) {
        if (!isShort())
            return;
        mData.n.i16 = value;
    }
    void setInt(int32 value) {
        if (!isInt())
            return;
        mData.n.i32 = value;
    }
    void setLong(int64 value) {
        if (!isLong())
            return;
        mData.n.i64 = value;
    }
    void setFloat(fp32 value) {
        if (!isFloat())
            return;
        mData.n.f32 = value;
    }
    void setDouble(fp64 value) {
        if (!isDouble())
            return;
        mData.n.f64 = value;
    }
    void setString(const std::string &value) {
        if (!isString())
            return;
        if (mData.s != nullptr) {
            *mData.s = value;
            return;
        }
        mData.s = new std::string(value);
    }
    void setByteArray(const std::vector<byte> &value) {
        if (!isByteArray())
            return;
        if (mData.bs != nullptr) {
            *mData.bs = value;
            return;
        }
        mData.bs = new std::vector<byte>(value);
    }
    void setIntArray(const std::vector<int32> &value) {
        if (!isIntArray())
            return;
        if (mData.is != nullptr) {
            *mData.is = value;
            return;
        }
        mData.is = new std::vector<int32>(value);
    }
    void setLongArray(const std::vector<int64> &value) {
        if (!isLongArray())
            return;
        if (mData.ls != nullptr) {
            *mData.ls = value;
            return;
        }
        mData.ls = new std::vector<int64>(value);
    }

    // @note Do nothing if the object is not a Byte Array.
    void addByte(byte value) {
        if (!isByteArray())
            return;
        if (mData.bs == nullptr)
            mData.bs = new std::vector<byte>();
        mData.bs->push_back(value);
    }
    // @note Do nothing if the object is not a Int Array.
    void addInt(int32 value) {
        if (!isIntArray())
            return;
        if (mData.is == nullptr)
            mData.is = new std::vector<int32>();
        mData.is->push_back(value);
    }
    // @note Do nothing if the object is not a Long Array.
    void addLong(int64 value) {
        if (!isLongArray())
            return;
        if (mData.ls == nullptr)
            mData.ls = new std::vector<int64>();
        mData.ls->push_back(value);
    }
    // @note Do nothing if the object is neither a List nor a Compound.
    void addMember(Tag &tag) {
        if (!isComplex() || tag.mType == End)
            return;
        if (mData.d == nullptr)
            mData.d = new std::vector<Tag>();
        mData.d->emplace_back(std::move(tag));
        if (isList())
            mData.d->back().mPureData = true;
        else
            mData.d->back().mPureData = false;
    }
    // @overload An rigth value overloaded version of the function addMember()
    void addMember(Tag &&tag) {
        if (!isComplex() || tag.mType == End)
            return;
        if (mData.d == nullptr)
            mData.d = new std::vector<Tag>();
        mData.d->emplace_back(std::move(tag));
        if (isList())
            mData.d->back().mPureData = true;
        else
            mData.d->back().mPureData = false;
    }

    // @note Do nothing if the object is not a Byte Array.
    void removeByte(std::size_t pos) {
        if (!isByteArray() || mData.bs == nullptr || pos >= mData.bs->size())
            return;
        mData.bs->erase(mData.bs->begin() + pos);
    }
    // @note Do nothing if the object is not a Int Array.
    void removeInt(std::size_t pos) {
        if (!isIntArray() || mData.is == nullptr || pos >= mData.is->size())
            return;
        mData.is->erase(mData.is->begin() + pos);
    }
    // @note Do nothing if the object is not a Long Array.
    void removeLong(std::size_t pos) {
        if (!isLongArray() || mData.ls == nullptr || pos >= mData.ls->size())
            return;
        mData.ls->erase(mData.ls->begin() + pos);
    }
    // @note Do nothing if the object is neither a List nor a Compound.
    void removeMember(std::size_t pos) {
        if (!isComplex() || mData.d == nullptr)
            return;

        NBT_ASSERT(pos < mData.d->size());
        NBT_IFERR(pos >= mData.d->size(), "The index is out range.");

        mData.d->erase(mData.d->begin() + pos);
    }
    // @note Do nothing if the object not is a Compound.
    void removeMember(const std::string &name) {
        if (!isCompound() || mData.d == nullptr)
            return;

        for (auto it = mData.d->begin(); it != mData.d->end(); ++it) {
            if (it->mName == nullptr)
                continue;
            if (*(it->mName) == name) {
                mData.d->erase(it);
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

        if (!mPureData && mName != nullptr && !mName->empty())
            result += *mName + ":";
        if (isIndented)
            result += " ";

        auto getNumString = [&] () ->std::string {
            if (mType == Byte)
                return std::to_string(static_cast<int>(mData.n.i8)) + "b";
            else if (mType == Short)
                return std::to_string(static_cast<int>(mData.n.i16)) + "s";
            else if (mType == Int)
                return std::to_string(mData.n.i32);
            else if (mType == Long)
                return std::to_string(mData.n.i64) + "l";
            else if (mType == Float)
                return std::to_string(mData.n.f32) + "f";
            else if (mType == Double)
                return std::to_string(mData.n.f64) + "d";
            else
                return "";
        };

        if (isEnd())
            return "";
        if (isNum())
            return result += getNumString();
        if (isString()) {
            result += "\"";
            if (mData.s != nullptr)
                result += *mData.s;
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
            if (isByteArray() && mData.bs != nullptr) {
                for (int i = 0; i < mData.bs->size(); ++i) {
                    if (isIndented)
                        result += "\n" + std::string(indentSize, ' ');
                    result += std::to_string(static_cast<int>((*mData.bs)[i])) + "b";
                    if (i != mData.bs->size() - 1)
                        result += ",";
                }
            } else if (isIntArray() && mData.is != nullptr) {
                for (int i = 0; i < mData.is->size(); ++i) {
                    if (isIndented)
                        result += "\n" + std::string(indentSize, ' ');
                    result += std::to_string(static_cast<int>((*mData.is)[i]));
                    if (i != mData.is->size() - 1)
                        result += ",";
                }
            } else if (isLongArray() && mData.ls != nullptr) {
                for (int i = 0; i < mData.ls->size(); ++i) {
                    if (isIndented)
                        result += "\n" + std::string(indentSize, ' ');
                    result += std::to_string(static_cast<int>((*mData.ls)[i])) + "l";
                    if (i != mData.ls->size() - 1)
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
            if (mData.d == nullptr || mData.d->empty()) {
                result += "[]";
                return result;
            }
            result += "[";
            indentSize += indentStep;
            for (int i = 0; i < mData.d->size(); ++i) {
                if (isIndented)
                    result += "\n" + std::string(indentSize, ' ');
                result += (*mData.d)[i].toSnbt(isIndented);
                if (i != mData.d->size() - 1)
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
            if (mData.d == nullptr || mData.d->empty()) {
                result += "{}";
                return result;
            }
            result += "{";
            indentSize += indentStep;
            for (int i = 0; i < mData.d->size(); ++i) {
                if (isIndented)
                    result += "\n" + std::string(indentSize, ' ');
                result += (*mData.d)[i].toSnbt(isIndented);
                if (i != mData.d->size() - 1)
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

        mPureData = rhs.mPureData;
        mType = rhs.mType;
        mDataType = rhs.mDataType;

        if (rhs.mType == End)
            return *this;
        if (!rhs.mPureData && rhs.mName != nullptr)
            mName = new std::string(*rhs.mName);
        if (rhs.isNum())
            mData.n = rhs.mData.n;
        else if (rhs.isString() && rhs.mData.s != nullptr)
            mData.s = new std::string(*rhs.mData.s);
        else if (rhs.isByteArray() && rhs.mData.bs != nullptr)
            mData.bs = new std::vector<byte>(*rhs.mData.bs);
        else if (rhs.isIntArray() && rhs.mData.is != nullptr)
            mData.is = new std::vector<int32>(*rhs.mData.is);
        else if (rhs.isLongArray() && rhs.mData.ls != nullptr)
            mData.ls = new std::vector<int64>(*rhs.mData.ls);
        else if (rhs.isComplex() && rhs.mData.d != nullptr)
            mData.d = new std::vector<Tag>(*rhs.mData.d);

        return *this;
    }
    Tag &operator=(Tag &&rhs) noexcept {
        this->~Tag();

        mPureData = rhs.mPureData;
        mType = rhs.mType;
        mDataType = rhs.mDataType;
        mName = rhs.mName;
        rhs.mName = nullptr;
        mData = rhs.mData;
        rhs.mData.s = nullptr;

        return *this;
    }
    Tag &operator[](std::size_t pos) {
        return getMember(pos);
    }
    Tag &operator[](const std::string &name) {
        return getMember(name);
    }
    Tag &operator<<(Tag &tag) {
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
        mType(type), mPureData(isPuredata),
        mDataType(End), mName(nullptr), mData(Data())
    {
        loadFromStream(is, isBigEndian);
    }
    // @brief Get a NBT tag from binary input stream.
    void loadFromStream(std::istream &is, bool isBigEndian) {
        construcPrework(is, isBigEndian);
        if (isEnd())
            return;
        if (isNum())
            numConstruct(is, isBigEndian);
        else if (isString())
            stringConstruct(is, isBigEndian);
        else if (isArray())
            arrayConstruct(is, isBigEndian);
        else if (isList())
            listConstruct(is, isBigEndian);
        else if (isCompound())
            compoundConstruct(is, isBigEndian);
        else
            NBT_ERR(std::to_string(static_cast<int>(mType)) + " Tag type is undefined!");
    }
    // @brief Get a NBT tag from SNBT.
    void loadFromSnbt(const std::string &snbt) {};
    void construcPrework(std::istream &is, bool isBigEndian) {
        if (mPureData)
            return;

        TagTypes type = static_cast<TagTypes>(is.get());
        NBT_ASSERT(type == mType);

        int16 nameLen = _bytesToNum<int16>(is, isBigEndian);

        if (mName != nullptr) {
            delete mName;
            mName = nullptr;
        }
        if (nameLen == 0)
            return;

        mName = new std::string();
        char *bytes = new char[nameLen];
        is.read(bytes, nameLen);
        mName->assign(bytes, static_cast<std::size_t>(is.gcount()));
        delete[] bytes;
    }
    void numConstruct(std::istream &is, bool isBigEndian) {
        switch (mType) {
            case Byte:
                mData.n.i8 = _bytesToNum<int8>(is, isBigEndian);
                break;
            case Short:
                mData.n.i16 = _bytesToNum<int16>(is, isBigEndian);
                break;
            case Int:
                mData.n.i32 = _bytesToNum<int32>(is, isBigEndian);
                break;
            case Long:
                mData.n.i64 = _bytesToNum<int64>(is, isBigEndian);
                break;
            case Float:
                mData.n.f32 = _bytesToNum<fp32>(is, isBigEndian);
                break;
            case Double:
                mData.n.f64 = _bytesToNum<fp64>(is, isBigEndian);
                break;
            default:
                break;
        }
    }
    void stringConstruct(std::istream &is, bool isBigEndian) {
        int16 strlen = _bytesToNum<int16>(is, isBigEndian);

        if (mData.s != nullptr) {
            delete mData.s;
            mData.s = nullptr;
        }
        if (strlen == 0)
            return;

        mData.s = new std::string();
        char *bytes = new char[strlen];
        is.read(bytes, strlen);
        mData.s->assign(bytes, static_cast<std::size_t>(is.gcount()));
        delete[] bytes;
    }
    void arrayConstruct(std::istream &is, bool isBigEndian) {
        int32 dsize = _bytesToNum<int32>(is, isBigEndian);

        if (isByteArray()) {
            if (mData.bs != nullptr) {
                delete mData.bs;
                mData.bs = nullptr;
            }
            if (dsize == 0)
                return;
            mData.bs = new std::vector<int8>();
            mData.bs->reserve(dsize);
        }
        if (isIntArray()) {
            if (mData.is != nullptr) {
                delete mData.is;
                mData.ls = nullptr;
            }
            if (dsize == 0)
                return;
            mData.is = new std::vector<int32>();
            mData.is->reserve(dsize);
        }
        if (isLongArray()) {
            if (mData.ls != nullptr) {
                delete mData.ls;
                mData.ls = nullptr;
            }
            if (dsize == 0)
                return;
            mData.ls = new std::vector<int64>();
            mData.ls->reserve(dsize);
        }

        int32 size = 0;
        while (!is.eof() && size++ < dsize) {
            if (isByteArray())
                mData.bs->emplace_back(_bytesToNum<int8>(is, isBigEndian));
            if (isIntArray())
                mData.is->emplace_back(_bytesToNum<int32>(is, isBigEndian));
            if (isLongArray())
                mData.ls->emplace_back(_bytesToNum<int64>(is, isBigEndian));
        }
    }
    void listConstruct(std::istream &is, bool isBigEndian) {
        mDataType = static_cast<TagTypes>(is.get());
        int32 dsize = _bytesToNum<int32>(is, isBigEndian);

        if (mData.d != nullptr) {
            delete mData.d;
            mData.d = nullptr;
        }
        if (dsize == 0)
            return;

        mData.d = new std::vector<Tag>();
        mData.d->reserve(dsize);

        int32 size = 0;
        while (!is.eof() && size++ < dsize)
            mData.d->emplace_back(Tag(mDataType, is, isBigEndian, true));
    }
    void compoundConstruct(std::istream &is, bool isBigEndian) {
        if (mData.d != nullptr)
            delete mData.d;
        mData.d = new std::vector<Tag>();

        while (!is.eof()) {
            TagTypes type = static_cast<TagTypes>(is.peek());
            if (type == End) {
                is.get();         // Give up End tag and move stream point to next byte.
                break;
            }
            mData.d->emplace_back(Tag(type, is, isBigEndian, false));
        }
    }
    void _write(std::ostream &os, bool isBigEndian = false) const {
        if (!mPureData) {
            os.put(static_cast<int8>(mType));
            if (mName == nullptr || mName->empty()) {
                _numToBytes<int16>(static_cast<int16>(0), os, isBigEndian);
            } else {
                _numToBytes<int16>(static_cast<int16>(mName->size()), os, isBigEndian);
                os.write(mName->c_str(), mName->size());
            }
        }

        if (isEnd()) {
            os.put(End);
            return;
        }
        if (isByte()) {
            os.put(mData.n.i8);
            return;
        }
        if (isShort()) {
            _numToBytes<int16>(mData.n.i16, os, isBigEndian);
            return;
        }
        if (isInt()) {
            _numToBytes<int32>(mData.n.i32, os, isBigEndian);
            return;
        }
        if (isLong()) {
            _numToBytes<int64>(mData.n.i64, os, isBigEndian);
            return;
        }
        if (isFloat()) {
            _numToBytes<fp32>(mData.n.f32, os, isBigEndian);
            return;
        }
        if (isDouble()) {
            _numToBytes<fp64>(mData.n.f64, os, isBigEndian);
            return;
        }
        if (isString()) {
            if (mData.s == nullptr || mData.s->empty()) {
                _numToBytes<int16>(static_cast<int16>(0), os, isBigEndian);
                return;
            }
            _numToBytes<int16>(static_cast<int16>(mData.s->size()), os, isBigEndian);
            os.write(mData.s->c_str(), mData.s->size());
            return;
        }
        if (isByteArray()) {
            if (mData.bs == nullptr || mData.bs->empty()) {
                _numToBytes<int32>(static_cast<int32>(0), os, isBigEndian);
                return;
            }
            _numToBytes<int32>(static_cast<int32>(mData.bs->size()), os, isBigEndian);
            for (auto &var : *mData.bs)
                os.put(var);
            return;
        }
        if (isIntArray()) {
            if (mData.is == nullptr || mData.is->empty()) {
                _numToBytes<int32>(static_cast<int32>(0), os, isBigEndian);
                return;
            }
            _numToBytes<int32>(static_cast<int32>(mData.is->size()), os, isBigEndian);
            for (auto &var : *mData.is)
                _numToBytes<int32>(var, os, isBigEndian);
            return;
        }
        if (isLongArray()) {
            if (mData.ls == nullptr || mData.ls->empty()) {
                _numToBytes<int32>(static_cast<int32>(0), os, isBigEndian);
                return;
            }
            _numToBytes<int32>(static_cast<int32>(mData.ls->size()), os, isBigEndian);
            for (auto &var : *mData.ls)
                _numToBytes<int64>(var, os, isBigEndian);
            return;
        }
        if (isList()) {
            if (mData.d == nullptr || mData.d->empty()) {
                os.put(static_cast<int8>(End));
                _numToBytes<int32>(static_cast<int32>(0), os, isBigEndian);
                return;
            }
            os.put(static_cast<int8>(mDataType));
            _numToBytes<int32>(static_cast<int32>(mData.d->size()), os, isBigEndian);
            for (auto &var : *mData.d)
                var._write(os, isBigEndian);
            return;
        }
        if (isCompound()) {
            if (mData.d == nullptr || mData.d->empty()) {
                os.put(End);
                return;
            }
            for (auto &var : *mData.d)
                var._write(os, isBigEndian);
            os.put(End);
            return;
        }
    }

private:
    // Whether to the object  is a "Base Tag", and the Base Tag has not description prefix. (e.g. name, name length)
    // The all members of List is a "Base Tag".
    bool mPureData;
    TagTypes mType;
    TagTypes mDataType;
    std::string *mName;
    Data mData;
};

}

namespace Nbt
{

inline Tag gByte(const std::string &name, char value) {
    Tag tag(TagTypes::Byte, name);
    tag.setByte(value);
    return tag;
}

inline Tag gpByte(char value) {
    Tag tag(TagTypes::Byte, true);
    tag.setByte(value);
    return tag;
}

inline Tag gShort(const std::string &name, short value) {
    Tag tag(TagTypes::Short, name);
    tag.setShort(value);
    return tag;
}

inline Tag gpShort(short value) {
    Tag tag(TagTypes::Short, true);
    tag.setShort(value);
    return tag;
}

inline Tag gInt(const std::string &name, int value) {
    Tag tag(TagTypes::Int, name);
    tag.setInt(value);
    return tag;
}

inline Tag gpInt(int value) {
    Tag tag(TagTypes::Int, true);
    tag.setInt(value);
    return tag;
}

inline Tag gLong(const std::string &name, long long value) {
    Tag tag(TagTypes::Long, name);
    tag.setLong(value);
    return tag;
}

inline Tag gpLong(long long value) {
    Tag tag(TagTypes::Long, true);
    tag.setLong(value);
    return tag;
}

inline Tag gFloat(const std::string &name, float value) {
    Tag tag(TagTypes::Float, name);
    tag.setFloat(value);
    return tag;
}

inline Tag gpFloat(float value) {
    Tag tag(TagTypes::Float, true);
    tag.setFloat(value);
    return tag;
}

inline Tag gDouble(const std::string &name, double value) {
    Tag tag(TagTypes::Double, name);
    tag.setDouble(value);
    return tag;
}

inline Tag gpDouble(double value) {
    Tag tag(TagTypes::Double, true);
    tag.setDouble(value);
    return tag;
}

inline Tag gString(const std::string &name, const std::string &value) {
    Tag tag(TagTypes::String, name);
    tag.setString(value);
    return tag;
}

inline Tag gpString(const std::string &value) {
    Tag tag(TagTypes::String, true);
    tag.setString(value);
    return tag;
}

inline Tag gByteArray(const std::string &name = std::string()) {
    return Tag(TagTypes::ByteArray, name);
}

inline Tag gpByteArray() {
    return Tag(TagTypes::ByteArray, true);
}

inline Tag gIntArray(const std::string &name = std::string()) {
    return Tag(TagTypes::IntArray, name);
}

inline Tag gpIntArray() {
    return Tag(TagTypes::IntArray, true);
}

inline Tag gLongArray(const std::string &name = std::string()) {
    return Tag(TagTypes::LongArray, name);
}

inline Tag gpLongArray() {
    return Tag(TagTypes::LongArray, true);
}

inline Tag gList(const std::string &name, TagTypes dtype) {
    Tag tag(TagTypes::List, name);
    tag.mDataType = dtype;
    return tag;
}

inline Tag gpList(TagTypes dtype) {
    Tag tag(TagTypes::List, true);
    tag.mDataType = dtype;
    return tag;
}

inline Tag gCompound(const std::string &name = std::string()) {
    return Tag(Compound, name);
}

inline Tag gpCompound() {
    return Tag(Compound, true);
}

}

#endif // !NBT_HPP
