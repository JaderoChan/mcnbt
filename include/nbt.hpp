#ifndef NBT_HPP
#define NBT_HPP

#include <cstring> // std::memcpy()
#include <cstdint> // std::size_t
#include <vector>   // std::vector
#include <string>   // std::string
#include <fstream>  // std::istream, std::ostream, fstream
#include <algorithm> // std::reverser()

#ifndef NBT_ASSERT
#include <cassert>
#define NBT_ASSERT(x) assert(x)
#endif

constexpr const char *NBT_ERRHINT = "[NBT Error] ";
constexpr const char *NBT_WARNHINT = "[NBT Warning] ";

#ifndef NBT_TERMINATE
#define NBT_TERMINATE std::terminate()
#endif

#include <iostream>
#ifndef NBT_WARN
#define NBT_WARN(x) std::cerr << NBT_WARNHINT << x << std::endl
#endif

#ifndef NBT_ERR
#define NBT_ERR(x) {std::cerr << NBT_WARNHINT << x << std::endl; NBT_TERMINATE;}
#endif

#ifndef NBT_IFWARN
#define NBT_IFWARN(x,y) if(x) std::cerr << NBT_WARNHINT << y << std::endl
#endif

#ifndef NBT_IFERR
#define NBT_IFERR(x,y) if(x) {std::cerr << NBT_ERRHINT << y << std::endl; NBT_TERMINATE;}
#endif

namespace Nbt
{

// 定义bytes2num和num2bytes函数所使用的缓冲区_buffer的大小。
constexpr const int BufferSize = 8;

/* @brief 从输入流中获取字节并转换为数字
* @param resumeSeek: 决定函数读取完数据后，流指针是否置于原位。
*/
template<typename T>
T bytes2num(std::istream &is, bool isBigEndian = false, bool resumeCursor = false);

// @brief 将数字转换为字节形式输出到流中
template<typename T>
void num2bytes(T num, std::ostream &os, bool isBigEndian = false);

// Tag-type enum
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

/*@brief get a Tag-types string via TagTypes value*/
static std::string type2str(TagTypes type) {
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

    // Num union contain interger and float point num, and use union for saving sapce.
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

    union Data
    {
        Data() : s(nullptr) {}
        Num n;
        // String Data
        std::string *s;
        // Array Data
        std::vector<byte> *bs;
        // IntArray Data
        std::vector<int32> *is;
        // LongArray Data
        std::vector<int64> *ls;
        // List & Compound Data
        std::vector<Tag> *d;
    };

public:
    Tag(TagTypes type, bool isBigEndian = false, bool isPuredata = false) :
        type_(type), isBigEndian_(isBigEndian), isPuredata_(isPuredata), dtype_(End),
        name_(nullptr), data_(Data()) {}
    Tag(TagTypes type, const std::string &name, bool isBigEndian = false) :
        type_(type), isBigEndian_(isBigEndian), isPuredata_(false), dtype_(End),
        name_(new std::string(name)), data_(Data()) {}
    Tag(TagTypes type, std::istream &is, bool isBigEndian = false,
        std::size_t headerSize = 0, bool isPuredata = false);
    Tag(const Tag &rhs);
    Tag(Tag &&rhs) noexcept;
    ~Tag();

public:
    Tag copy() {
        Tag tag(*this);
        return tag;
    }
    /* @brief 返回自身的右值引用，通常用于移交大量数据（例如Compound中add一个List）
    *   当一个对象使用move加入List或Compound中时，原数据将被销毁
    */
    Tag move() { return std::move(*this); }
    // read tag from binary istream, be usually used for read root-tag (that is whole nbt).
    void loadFromStream(std::istream &is);
    // read whole snbt from text.
    void loadFromSnbt(const std::string &snbt);

    bool isBigEndian() const { return isBigEndian_; }
    bool isEnd() const { return type_ == End; }
    bool isByte() const { return type_ == Byte; }
    bool isShort() const { return type_ == Short; }
    bool isInt() const { return type_ == Int; }
    bool isLong() const { return type_ == Long; }
    bool isFloat() const { return type_ == Float; }
    bool isDouble() const { return type_ == Double; }
    bool isString() const { return type_ == String; }
    bool isByteArray() const { return type_ == ByteArray; }
    bool isIntArray() const { return type_ == IntArray; }
    bool isLongArray() const { return type_ == LongArray; }
    bool isList() const { return type_ == List; }
    bool isCompound() const { return type_ == Compound; }
    bool isNum() const {
        return type_ == Byte || type_ == Short || type_ == Int ||
            type_ == Long || type_ == Float || type_ == Double;
    }
    bool isInteger() const {
        return type_ == Byte || type_ == Short || type_ == Int ||
            type_ == Long;
    }
    bool isFloaToint() const { return type_ == Float || type_ == Double; }
    bool isArray() const {
        return type_ == ByteArray || type_ == IntArray || type_ == LongArray;
    }
    bool isContainer() const { return type_ == Compound || type_ == List; }

    // @brief 检查Compound中是否具有指定名称的Tag，若调用者不是Compound将返回false
    bool hasMember(const std::string &name) const;

    TagTypes type() const { return type_; }
    // @brief 返回List中的元素类型，若调用者不是List将返回End
    TagTypes dtype() const {
        NBT_ASSERT(isList());
        if (!isList())
            return End;
        return dtype_;
    }
    std::string name() const {
        NBT_ASSERT(!isPuredata_);
        if (isPuredata_ || name_ == nullptr)
            return std::string();
        return *name_;
    }
    int16 nameLen() const {
        NBT_ASSERT(!isPuredata_);
        if (isPuredata_ || name_ == nullptr)
            return 0;
        return static_cast<int16>(name_->size());
    }
    int32 strlen() const {
        NBT_ASSERT(isString());
        if (!isString() || data_.s == nullptr)
            return 0;
        return static_cast<int32>(data_.s->size());
    }
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
        else if (isContainer() && data_.d != nullptr)
            return data_.d->size();
        return 0;
    }

    byte getByte() const {
        NBT_ASSERT(isByte());
        if (!isByte()) return 0;
        return data_.n.i8;
    }
    int16 getShort() const {
        NBT_ASSERT(isShort());
        if (!isShort()) return 0;
        return data_.n.i16;
    }
    int32 getInt() const {
        NBT_ASSERT(isInt());
        if (!isInt()) return 0;
        return data_.n.i32;
    }
    int64 getLong() const {
        NBT_ASSERT(isLong());
        if (!isLong()) return 0;
        return data_.n.i64;
    }
    fp32 getFloat() const {
        NBT_ASSERT(isFloat());
        if (!isFloat()) return 0;
        return data_.n.f32;
    }
    fp64 getDouble() const {
        NBT_ASSERT(isDouble());
        if (!isDouble()) return 0;
        return data_.n.f64;
    }
    std::string getString() const {
        NBT_ASSERT(isString());
        if (!isString() || data_.s == nullptr)
            return std::string();
        return *data_.s;
    }
    std::vector<byte> getArray() const {
        NBT_ASSERT(isArray());
        if (!isArray() || data_.bs == nullptr)
            return std::vector<byte>();
        return *data_.bs;
    }
    std::vector<int32> getIntArray() const {
        NBT_ASSERT(isIntArray());
        if (!isIntArray() || data_.is == nullptr)
            return std::vector<int32>();
        return *data_.is;
    }
    std::vector<int64> getLongArray() const {
        NBT_ASSERT(isLongArray());
        if (!isLongArray() || data_.ls == nullptr)
            return std::vector<int64>();
        return *data_.ls;
    }
    Tag &get(int32 pos) {
        NBT_ASSERT(isContainer());
        if (!isContainer() || data_.d == nullptr)
            return FailedTag;
        NBT_ASSERT(pos < static_cast<int32>(data_.d->size()));
        NBT_IFERR(pos >= static_cast<int32>(data_.d->size()), "The index is out range.");
        return (*data_.d)[pos];
    }
    Tag &get(const std::string &name);

    void setName(const std::string &name) {
        NBT_ASSERT(!isPuredata_);
        if (isPuredata_)
            return;
        if (name_ != nullptr) {
            *name_ = name;
            return;
        }
        name_ = new std::string(name);
    }
    void setByte(byte value) {
        NBT_ASSERT(isByte());
        if (!isByte())
            return;
        data_.n.i8 = value;
    }
    void setShort(int16 value) {
        NBT_ASSERT(isShort());
        if (!isShort())
            return;
        data_.n.i16 = value;
    }
    void setInt(int32 value) {
        NBT_ASSERT(isInt());
        if (!isInt())
            return;
        data_.n.i32 = value;
    }
    void setLong(int64 value) {
        NBT_ASSERT(isLong());
        if (!isLong())
            return;
        data_.n.i64 = value;
    }
    void setFloat(fp32 value) {
        NBT_ASSERT(isFloat());
        if (!isFloat())
            return;
        data_.n.f32 = value;
    }
    void setDouble(fp64 value) {
        NBT_ASSERT(isDouble());
        if (!isDouble())
            return;
        data_.n.f64 = value;
    }
    void setString(const std::string &value) {
        NBT_ASSERT(isString());
        if (!isString())
            return;
        if (data_.s != nullptr) {
            *data_.s = value;
            return;
        }
        data_.s = new std::string(value);
    }
    void setArray(const std::vector<byte> &value) {
        NBT_ASSERT(isArray());
        if (!isArray())
            return;
        if (data_.bs != nullptr) {
            *data_.bs = value;
            return;
        }
        data_.bs = new std::vector<byte>(value);
    }
    void setIntArray(const std::vector<int32> &value) {
        NBT_ASSERT(isIntArray());
        if (!isIntArray())
            return;
        if (data_.is != nullptr) {
            *data_.is = value;
            return;
        }
        data_.is = new std::vector<int32>(value);
    }
    void setLongArray(const std::vector<int64> &value) {
        NBT_ASSERT(isLongArray());
        if (!isLongArray())
            return;
        if (data_.ls != nullptr) {
            *data_.ls = value;
            return;
        }
        data_.ls = new std::vector<int64>(value);
    }

    // only for tag with Array type.
    void addByte(byte value) {
        NBT_ASSERT(isByte());
        if (!isByte())
            return;
        if (data_.bs == nullptr)
            data_.bs = new std::vector<byte>();
        data_.bs->push_back(value);
    }
    // only for tag with IntArray type.
    void addInt(int32 value) {
        NBT_ASSERT(isInt());
        if (!isInt())
            return;
        if (data_.is == nullptr)
            data_.is = new std::vector<int32>();
        data_.is->push_back(value);
    }
    // only for tag with LongArray type.
    void addLong(int64 value) {
        NBT_ASSERT(isLong());
        if (!isLong())
            return;
        if (data_.ls == nullptr)
            data_.ls = new std::vector<int64>();
        data_.ls->push_back(value);
    }
    // only for tag with Compound and List type.
    void add(const Tag &tag) {
        NBT_ASSERT(isContainer());
        if (!isContainer() || tag.type_ == End)
            return;
        if (data_.d == nullptr)
            data_.d = new std::vector<Tag>();
        data_.d->emplace_back(tag);
        if (isList())
            data_.d->back().isPuredata_ = true;
        else
            data_.d->back().isPuredata_ = false;
    }
    void add(Tag &&tag) {
        NBT_ASSERT(isContainer());
        if (!isContainer() || tag.type_ == End)
            return;
        if (data_.d == nullptr)
            data_.d = new std::vector<Tag>();
        data_.d->emplace_back(std::move(tag));
        if (isList())
            data_.d->back().isPuredata_ = true;
        else
            data_.d->back().isPuredata_ = false;
    }
    // only for tag with Compound and List type.
    void remove(int32 pos) {
        NBT_ASSERT(isContainer());
        if (!isContainer() || data_.d == nullptr)
            return;
        NBT_ASSERT(pos < static_cast<int32>(data_.d->size()));
        NBT_IFERR(pos >= static_cast<int32>(data_.d->size()), "The index is out range.");
        data_.d->erase(data_.d->begin() + pos);
    }
    // only for tag with Compound type.
    void remove(const std::string &name);

    // print the unshaped tag content to ostream(std::cout or std::ofstream and so on) for test.
    void print(std::ostream &os) const;
    // save the binary tag to ostream.
    void write(std::ostream &os, bool isBigEndian = false) const;
    // get a snbt form tag.
    std::string toSnbt() const;

    Tag &operator=(const Tag &rhs);
    Tag &operator=(Tag &&rhs) noexcept;
    Tag &operator[](int32 pos) { return get(pos); }
    Tag &operator[](const std::string &name);
    // crude operator overload for ues simply.
    Tag &operator<<(const Tag &tag) {
        add(tag);
        return *this;
    }
    Tag &operator<<(Tag &&tag) {
        add(std::move(tag));
        return *this;
    }

    // the genList() and genListBase() function need modify private member variable dtype_.
    friend Tag gList(const std::string &name, TagTypes dtype, bool isBigEndian);
    friend Tag gpList(TagTypes dtype, bool isBigEndian);

private:
    void construcTrework(std::istream &is);
    void numConstruct(std::istream &is);
    void stringConstruct(std::istream &is);
    void arrayConstruct(std::istream &is);
    void listConstruct(std::istream &is);
    void compoundConstruct(std::istream &is);

private:
    // whether is "Base-Tag",and the tag that not is Base-Tag not has description prefix. (e.g. name, name length)
    bool isPuredata_;
    bool isBigEndian_;
    TagTypes type_;
    TagTypes dtype_;
    std::string *name_;
    Data data_;
    // a unuseful object, the function is reply function that returns a Tag reference can have a return value in case of an error.
    static Tag FailedTag;
};

inline static Tag gByte(const std::string &name, char value, bool isBigEndian = false) {
    Tag tag(TagTypes::Byte, name, isBigEndian);
    tag.setByte(value);
    return tag;
}

inline static Tag gpByte(char value, bool isBigEndian = false) {
    Tag tag(TagTypes::Byte, isBigEndian, true);
    tag.setByte(value);
    return tag;
}

inline static Tag gShort(const std::string &name, short value, bool isBigEndian = false) {
    Tag tag(TagTypes::Short, name, isBigEndian);
    tag.setShort(value);
    return tag;
}

inline static Tag gpShort(short value, bool isBigEndian = false) {
    Tag tag(TagTypes::Short, isBigEndian, true);
    tag.setShort(value);
    return tag;
}

inline static Tag gInt(const std::string &name, int value, bool isBigEndian = false) {
    Tag tag(TagTypes::Int, name, isBigEndian);
    tag.setInt(value);
    return tag;
}

inline static Tag gpInt(int value, bool isBigEndian = false) {
    Tag tag(TagTypes::Int, isBigEndian, true);
    tag.setInt(value);
    return tag;
}

inline static Tag gLong(const std::string &name, long long value, bool isBigEndian = false) {
    Tag tag(TagTypes::Long, name, isBigEndian);
    tag.setLong(value);
    return tag;
}

inline static Tag gpLong(long long value, bool isBigEndian = false) {
    Tag tag(TagTypes::Long, isBigEndian, true);
    tag.setLong(value);
    return tag;
}

inline static Tag gFloat(const std::string &name, float value, bool isBigEndian = false) {
    Tag tag(TagTypes::Float, name, isBigEndian);
    tag.setFloat(value);
    return tag;
}

inline static Tag gpFloat(float value, bool isBigEndian = false) {
    Tag tag(TagTypes::Float, isBigEndian, true);
    tag.setFloat(value);
    return tag;
}

inline static Tag gDouble(const std::string &name, double value, bool isBigEndian = false) {
    Tag tag(TagTypes::Double, name, isBigEndian);
    tag.setDouble(value);
    return tag;
}

inline static Tag gpDouble(double value, bool isBigEndian = false) {
    Tag tag(TagTypes::Double, isBigEndian, true);
    tag.setDouble(value);
    return tag;
}

inline static Tag gString(const std::string &name, const std::string &value, bool isBigEndian = false) {
    Tag tag(TagTypes::String, name, isBigEndian);
    tag.setString(value);
    return tag;
}

inline static Tag gpString(const std::string &value, bool isBigEndian = false) {
    Tag tag(TagTypes::String, isBigEndian, true);
    tag.setString(value);
    return tag;
}

inline static Tag gArray(const std::string &name = std::string(), bool isBigEndian = false) {
    return Tag(TagTypes::ByteArray, name, isBigEndian);
}

inline static Tag gpArray(bool isBigEndian = false) {
    return Tag(TagTypes::ByteArray, isBigEndian, true);
}

inline static Tag gIntArray(const std::string &name = std::string(), bool isBigEndian = false) {
    return Tag(TagTypes::IntArray, name, isBigEndian);
}

inline static Tag gpIntArray(bool isBigEndian = false) {
    return Tag(TagTypes::IntArray, isBigEndian, true);
}

inline static Tag gLongArray(const std::string &name = std::string(), bool isBigEndian = false) {
    return Tag(TagTypes::LongArray, name, isBigEndian);
}

inline static Tag gpLongArray(bool isBigEndian = false) {
    return Tag(TagTypes::LongArray, isBigEndian, true);
}

inline static Tag gList(const std::string &name, TagTypes dtype, bool isBigEndian = false) {
    Tag tag(TagTypes::List, name, isBigEndian);
    tag.dtype_ = dtype;
    return tag;
}

inline static Tag gpList(TagTypes dtype, bool isBigEndian = false) {
    Tag tag(TagTypes::List, isBigEndian, true);
    tag.dtype_ = dtype;
    return tag;
}

inline static Tag gCompound(const std::string &name = std::string(), bool isBigEndian = false) {
    return Tag(Compound, name, isBigEndian);
}

inline static Tag gpCompound(bool isBigEndian = false) {
    return Tag(Compound, isBigEndian, true);
}

}

#endif // !NBT_HPP
