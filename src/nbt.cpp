#include "nbt.hpp"

#include <sstream>

namespace Nbt
{

char _buffer[_BufferSize];

// @brief Reverse a C string.
// @param size The size of range that need reversed, and reverser all if the size is 0.
static void reverse(char *str, std::size_t size = 0) {
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
inline static bool isBigEndian() {
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

const bool IsBigEndian = isBigEndian();

template<typename T>
T _bytesToNum(std::istream &is, bool isBigEndian, bool restoreCursor) {
    std::size_t size = sizeof(T);
    T result = T();
    auto begpos = is.tellg();
    is.read(_buffer, size);
    size = static_cast<std::size_t>(is.gcount());
    if (isBigEndian != IsBigEndian)
        Nbt::reverse(_buffer, size);
    std::memcpy(&result, _buffer, size);
    if (restoreCursor)
        is.seekg(begpos);
    return result;
}

template<typename T>
void _numToBytes(T num, std::ostream &os, bool isBigEndian) {
    std::size_t size = sizeof(T);
    std::memcpy(_buffer, &num, size);
    if (isBigEndian != IsBigEndian)
        Nbt::reverse(_buffer, size);
    os.write(_buffer, size);
}

Tag Tag::FailedTag = Tag(End);

Tag::Tag(TagTypes type, std::istream &is, bool isBigEndian, std::size_t headerSize) :
    type_(type), isPuredata_(false),
    dtype_(End), name_(nullptr), data_(Data())
{
#ifndef NBT_NOGZIP
    std::stringstream ss;
    is.seekg(0, is.end);
    long long size = is.tellg();
    is.seekg(0, is.beg);
    char *buffer = new char[size];
    is.read(buffer, size);
    std::string content = std::string(buffer, size);
    delete[]buffer;
    if (gzip::is_compressed(content.c_str(), size))
        content = gzip::decompress(content.c_str(), content.size());
    ss << content;
#endif // !NBT_NOGZIP

    if (headerSize != 0) {
        char *buffer = new char[headerSize];
        is.read(buffer, headerSize);
        delete[] buffer;
    }

    _loadFromStream(ss, isBigEndian);
}

Tag::Tag(const Tag &other) :
    type_(other.type_), isPuredata_(other.isPuredata_),
    dtype_(other.dtype_), name_(nullptr), data_(Data())
{
    if (other.type_ == End)
        return;
    if (!other.isPuredata_ && other.name_ != nullptr)
        name_ = new std::string(*other.name_);
    if (other.isNum())
        data_.n = other.data_.n;
    else if (other.isString() && other.data_.s != nullptr)
        data_.s = new std::string(*other.data_.s);
    else if (other.isArray() && other.data_.bs != nullptr)
        data_.bs = new std::vector<byte>(*other.data_.bs);
    else if (other.isIntArray() && other.data_.is != nullptr)
        data_.is = new std::vector<int32>(*other.data_.is);
    else if (other.isLongArray() && other.data_.ls != nullptr)
        data_.ls = new std::vector<int64>(*other.data_.ls);
    else if (other.isComplex() && other.data_.d != nullptr)
        data_.d = new std::vector<Tag>(*other.data_.d);
}

Tag::Tag(Tag &&other) noexcept :
    type_(other.type_), isPuredata_(other.isPuredata_),
    dtype_(other.dtype_), name_(other.name_), data_(other.data_)
{
    other.name_ = nullptr;
    other.data_.s = nullptr;
}

Tag::~Tag() {
    if (name_ != nullptr) {
        delete name_;
        name_ = nullptr;
    }
    if (data_.s != nullptr) {
        delete data_.s;
        data_.s = nullptr;
    }
    if (data_.bs != nullptr) {
        delete data_.bs;
        data_.bs = nullptr;
    }
    if (data_.is != nullptr) {
        delete data_.is;
        data_.is = nullptr;
    }
    if (data_.ls != nullptr) {
        delete data_.ls;
        data_.ls = nullptr;
    }
    if (data_.d != nullptr) {
        delete data_.d;
        data_.d = nullptr;
    }
}

bool Tag::hasMember(const std::string &name) const {
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

Tag &Tag::getMember(const std::string &name) {
    NBT_ASSERT(isCompound());
    if (!isCompound() || data_.d == nullptr)
        return FailedTag;

    for (auto &var : *data_.d) {
        if (var.name_ == nullptr)
            continue;
        if (*var.name_ == name)
            return var;
    }
    return FailedTag;
}

void Tag::removeMember(const std::string &name) {
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
void Tag::write(std::ostream &os, bool isBigEndian, bool isCompressed) const
{
    if (isCompressed) {
        std::stringstream ss;
        _write(ss, isBigEndian);
        os << gzip::compress(ss.str().c_str(), ss.str().size());
        return;
    }
    _write(os, isBigEndian);
}
#else
void Tag::write(std::ostream &os, bool isBigEndian) const
{
    _write(os, isBigEndian);
}
#endif // !NBT_NOGZIP

std::string Tag::toSnbt(bool isIndented) const {
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

Tag &Tag::operator=(const Tag &other) {
    this->~Tag();

    isPuredata_ = other.isPuredata_;
    type_ = other.type_;
    dtype_ = other.dtype_;

    if (other.type_ == End)
        return *this;
    if (!other.isPuredata_ && other.name_ != nullptr)
        name_ = new std::string(*other.name_);
    if (other.isNum())
        data_.n = other.data_.n;
    else if (other.isString() && other.data_.s != nullptr)
        data_.s = new std::string(*other.data_.s);
    else if (other.isByteArray() && other.data_.bs != nullptr)
        data_.bs = new std::vector<byte>(*other.data_.bs);
    else if (other.isIntArray() && other.data_.is != nullptr)
        data_.is = new std::vector<int32>(*other.data_.is);
    else if (other.isLongArray() && other.data_.ls != nullptr)
        data_.ls = new std::vector<int64>(*other.data_.ls);
    else if (other.isComplex() && other.data_.d != nullptr)
        data_.d = new std::vector<Tag>(*other.data_.d);

    return *this;
}

Tag &Tag::operator=(Tag &&other) noexcept {
    this->~Tag();

    isPuredata_ = other.isPuredata_;
    type_ = other.type_;
    dtype_ = other.dtype_;
    name_ = other.name_;
    other.name_ = nullptr;
    data_ = other.data_;
    other.data_.s = nullptr;

    return *this;
}

Tag::Tag(TagTypes type, std::istream &is, bool isBigEndian, bool isPuredata) :
    type_(type), isPuredata_(isPuredata),
    dtype_(End), name_(nullptr), data_(Data())
{
    _loadFromStream(is, isBigEndian);
}

void Tag::_loadFromStream(std::istream &is, bool isBigEndian)
{
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

// TODO
void Tag::_loadFromSnbt(const std::string &snbt) {

}

void Tag::_write(std::ostream &os, bool isBigEndian) const {
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

void Tag::_construcPrework(std::istream &is, bool isBigEndian) {
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

void Tag::_numConstruct(std::istream &is, bool isBigEndian) {
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

void Tag::_stringConstruct(std::istream &is, bool isBigEndian) {
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

void Tag::_arrayConstruct(std::istream &is, bool isBigEndian) {
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

void Tag::_listConstruct(std::istream &is, bool isBigEndian) {
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

void Tag::_compoundConstruct(std::istream &is, bool isBigEndian) {
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

}
