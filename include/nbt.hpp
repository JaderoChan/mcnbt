#ifndef NBT_HPP
#define NBT_HPP

//#define NBT_NOGZIP

// C Header
#include <cstring> // std::memcpy()
#include <cstdint> // std::size_t

// Cpp Header
#include <vector>   // std::vector
#include <string>   // std::string
#include <fstream>  // std::istream, std::ostream, fstream
#include <algorithm> // std::reverser()

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
constexpr const int _BufferSize = 8;

// @brief Obtain bytes from input stream, and convert it to number.
// @param restoreCursor Whether to restore the input stream cursor after read.
// @return A number.
template<typename T>
T _bytesToNum(std::istream &is, bool isBigEndian = false, bool restoreCursor = false);

// @brief Convert the number to bytes, and output it to output stream.
template<typename T>
void _numToBytes(T num, std::ostream &os, bool isBigEndian = false);

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
        type_(type), isPuredata_(isPuredata),
        dtype_(End), name_(nullptr), data_(Data()) {}
    Tag(TagTypes type, const std::string &name) :
        type_(type), isPuredata_(false),
        dtype_(End), name_(new std::string(name)), data_(Data()) {}
    Tag(TagTypes type, std::istream &is, bool isBigEndian = false, std::size_t headerSize = 0);
    Tag(const Tag &rhs);
    Tag(Tag &&rhs) noexcept;
    ~Tag();

    // @brief The default assignment case is copy(), and no explicit call is required.
    Tag copy() {
        Tag tag(*this);
        return tag;
    }
    // @brief Move an object containing a lot of data, and original object will be destroyed.
    Tag move() {
        return std::move(*this);
    }

    // @brief Whther to is a Failed Tag.
    bool isFailedTag() const {
        return this == &FailedTag;
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
    bool hasMember(const std::string &name) const;

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
        NBT_ASSERT(isComplex());

        if (!isComplex() || data_.d == nullptr)
            return FailedTag;

        NBT_ASSERT(pos < data_.d->size());
        NBT_IFERR(pos >= data_.d->size(), "The index is out range.");

        return (*data_.d)[pos];
    }
    // @return Return Failed Tag if the object is not a Compound or does not contains a special member
    Tag &getMember(const std::string &name);
    Tag &front() {
        NBT_ASSERT(isComplex());

        if (!isComplex() || data_.d == nullptr)
            return FailedTag;

        return data_.d->front();
    }
    Tag &back() {
        NBT_ASSERT(isComplex());

        if (!isComplex() || data_.d == nullptr)
            return FailedTag;

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
    void removeMember(const std::string &name);

#ifndef NBT_NOGZIP
    // @brief Output the binay NBT tag to output stream.
    void write(std::ostream &os, bool isBigEndian = false, bool isCompressed = false) const;
#else
    // @brief Output the binay NBT tag to output stream.
    void write(std::ostream &os, bool isBigEndian = false) const;
#endif // !NBT_NOGZIP

    // @brief Get the SNBT. (The string representation of NBT)
    std::string toSnbt(bool isIndented = true) const;

    Tag &operator=(const Tag &rhs);
    Tag &operator=(Tag &&rhs) noexcept;
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
    Tag(TagTypes type, std::istream &is, bool isBigEndian, bool isPuredata);
    // @brief Get a NBT tag from binary input stream.
    void _loadFromStream(std::istream &is, bool isBigEndian);
    // @brief Get a NBT tag from SNBT.
    void _loadFromSnbt(const std::string &snbt);
    void _write(std::ostream &os, bool isBigEndian = false) const;
    void _construcPrework(std::istream &is, bool isBigEndian);
    void _numConstruct(std::istream &is, bool isBigEndian);
    void _stringConstruct(std::istream &is, bool isBigEndian);
    void _arrayConstruct(std::istream &is, bool isBigEndian);
    void _listConstruct(std::istream &is, bool isBigEndian);
    void _compoundConstruct(std::istream &is, bool isBigEndian);

private:
    // Whether to the object  is a "Base Tag", and the Base Tag has not description prefix. (e.g. name, name length)
    // The all members of List is a "Base Tag".
    bool isPuredata_;
    TagTypes type_;
    TagTypes dtype_;
    std::string *name_;
    Data data_;
    // A unuseful object, for the function that returns a Tag reference can have a return value in case of an error.
    static Tag FailedTag;
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
