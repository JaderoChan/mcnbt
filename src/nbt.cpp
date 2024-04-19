#include "nbt.hpp"

namespace Nbt
{

char _buffer[BufferSize];

// 倒转C风格字符串，若参数size为0则倒转\0符号之前的字符
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

// 判断系统字节序是否为大端
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
T bytes2num(std::istream &is, bool isBigEndian, bool resumeCursor) {
    std::size_t size = sizeof(T);
    T result = T();
    auto begpos = is.tellg();
    is.read(_buffer, size);
    size = static_cast<std::size_t>(is.gcount());
    if (isBigEndian != IsBigEndian)
        Nbt::reverse(_buffer, size);
    std::memcpy(&result, _buffer, size);
    if (resumeCursor)
        is.seekg(begpos);
    return result;
}

template<typename T>
void num2bytes(T num, std::ostream &os, bool isBigEndian) {
    std::size_t size = sizeof(T);
    std::memcpy(_buffer, &num, size);
    if (isBigEndian != IsBigEndian)
        Nbt::reverse(_buffer, size);
    os.write(_buffer, size);
}

Tag Tag::FailedTag = Tag(End);

Tag::Tag(TagTypes type, std::istream &is, bool isBigEndian,
         std::size_t headerSize, bool isPuredata) :
    type_(type), isBigEndian_(isBigEndian), isPuredata_(isPuredata), dtype_(End),
    name_(nullptr), data_(Data()) {
    if (headerSize != 0) {
        char *buffer = new char[headerSize];
        is.read(buffer, headerSize);
        delete[] buffer;
    }
    loadFromStream(is);
}

Tag::Tag(const Tag &other) :
    isPuredata_(other.isPuredata_), isBigEndian_(other.isBigEndian_),
    type_(other.type_), dtype_(other.dtype_), name_(nullptr), data_(Data()) {
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
    else if (other.isContainer() && other.data_.d != nullptr)
        data_.d = new std::vector<Tag>(*other.data_.d);
}

Tag::Tag(Tag &&other) noexcept :
    isPuredata_(other.isPuredata_), isBigEndian_(other.isBigEndian_),
    type_(other.type_), dtype_(other.dtype_), name_(other.name_), data_(other.data_) {
    other.name_ = nullptr;
    other.data_.s = nullptr;
}

Tag::~Tag() {
    if (name_ != nullptr) {
        delete name_;
        name_ = nullptr;
    }
    if (isNum())
        return;
    if (isString() && data_.s != nullptr) {
        delete data_.s;
        data_.s = nullptr;
        return;
    }
    if (isArray() && data_.bs != nullptr) {
        delete data_.bs;
        data_.bs = nullptr;
        return;
    }
    if (isIntArray() && data_.is != nullptr) {
        delete data_.is;
        data_.is = nullptr;
        return;
    }
    if (isLongArray() && data_.ls != nullptr) {
        delete data_.ls;
        data_.ls = nullptr;
        return;
    }
    if (isContainer() && data_.d != nullptr) {
        delete data_.d;
        data_.d = nullptr;
        return;
    }
}

void Tag::loadFromStream(std::istream &is) {
    construcTrework(is);
    if (isEnd())
        return;
    if (isNum())
        numConstruct(is);
    else if (isString())
        stringConstruct(is);
    else if (isArray())
        arrayConstruct(is);
    else if (isList())
        listConstruct(is);
    else if (isCompound())
        compoundConstruct(is);
    else
        NBT_ERR("The Tag-type is undefined!");
}

void Tag::loadFromSnbt(const std::string &snbt) {}

bool Tag::hasMember(const std::string &name) const {
    NBT_ASSERT(isCompound());
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

Tag &Tag::get(const std::string &name) {
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

void Tag::remove(const std::string &name) {
    NBT_ASSERT(isCompound());
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

void Tag::print(std::ostream &os) const {
    if (!isPuredata_) {
        if (name_ == nullptr)
            os << "\"" << "\"" << " [" << 0 << "] ";
        else
            os << "\"" << *name_ << "\"" << " [" << name_->size() << "] ";
    }
    os << " [" << type2str(type_) << "]  :  ";
    switch (type_) {
        case End:
            return;
        case Byte:
            os << static_cast<int32>(data_.n.i8);
            break;
        case Short:
            os << data_.n.i16;
            break;
        case Int:
            os << data_.n.i32;
            break;
        case Long:
            os << data_.n.i64;
            break;
        case Float:
            os << data_.n.f32;
            break;
        case Double:
            os << data_.n.f64;
            break;
        case String:
            if (data_.s == nullptr)
                os << "\"" << "\"";
            else
                os << "\"" << *data_.s << "\"";
            break;
        case ByteArray:
            if (data_.bs == nullptr)
                break;
            for (auto &var : *data_.bs)
                os << static_cast<int32>(var) << "\n";
            break;
        case IntArray:
            if (data_.is == nullptr)
                break;
            for (auto &var : *data_.is)
                os << var << "\n";
            break;
        case LongArray:
            if (data_.ls == nullptr)
                break;
            for (auto &var : *data_.ls)
                os << var << "\n";
            break;
        case List:
            os << "(Element type : " << type2str(dtype_) << ") (Size : ";
            if (data_.d == nullptr) {
                os << 0 << ")\n";
                break;
            }
            os << data_.d->size() << ")\n";
            for (auto &var : *data_.d)
                var.print(os);
            break;
        case Compound:
            if (data_.d == nullptr) {
                os << "(Size : " << 0 << ")\n";
                break;
            }
            os << "(Size : " << data_.d->size() << ")\n";
            for (auto &var : *data_.d)
                var.print(os);
            break;
        default:
            break;
    }
    os << "\n";
    os.flush();
}

void Tag::write(std::ostream &os, bool isBigEndian) const {
    if (!isPuredata_) {
        os.put(static_cast<int8>(type_));
        if (name_ == nullptr)
            num2bytes<int16>(static_cast<int16>(0), os, isBigEndian);
        else {
            num2bytes<int16>(static_cast<int16>(name_->size()), os, isBigEndian);
            os.write(name_->c_str(), name_->size());
        }
    }
    switch (type_) {
        case End:
            os.put(End);
            break;
        case Byte:
            os.put(data_.n.i8);
            break;
        case Short:
            num2bytes<int16>(data_.n.i16, os, isBigEndian);
            break;
        case Int:
            num2bytes<int32>(data_.n.i32, os, isBigEndian);
            break;
        case Long:
            num2bytes<int64>(data_.n.i64, os, isBigEndian);
            break;
        case Float:
            num2bytes<fp32>(data_.n.f32, os, isBigEndian);
            break;
        case Double:
            num2bytes<fp64>(data_.n.f64, os, isBigEndian);
            break;
        case String:
            if (data_.s == nullptr)
                num2bytes<int16>(static_cast<int16>(0), os, isBigEndian);
            else {
                num2bytes<int16>(static_cast<int16>(data_.s->size()), os, isBigEndian);
                os.write(data_.s->c_str(), data_.s->size());
            }
            break;
        case ByteArray:
            if (data_.bs == nullptr)
                num2bytes<int32>(static_cast<int32>(0), os, isBigEndian);
            else {
                num2bytes<int32>(static_cast<int32>(data_.bs->size()), os, isBigEndian);
                for (auto &var : *data_.bs)
                    os.put(var);
            }
            break;
        case IntArray:
            if (data_.is == nullptr)
                num2bytes<int32>(static_cast<int32>(0), os, isBigEndian);
            else {
                num2bytes<int32>(static_cast<int32>(data_.is->size()), os, isBigEndian);
                for (auto &var : *data_.is)
                    num2bytes<int32>(var, os, isBigEndian);
            }
            break;
        case LongArray:
            if (data_.ls == nullptr)
                num2bytes<int32>(static_cast<int32>(0), os, isBigEndian);
            else {
                num2bytes<int32>(static_cast<int32>(data_.ls->size()), os, isBigEndian);
                for (auto &var : *data_.ls)
                    num2bytes<int64>(var, os, isBigEndian);
            }
            break;
        case List:
            if (data_.d == nullptr) {
                os.put(static_cast<byte>(End));
                num2bytes<int32>(static_cast<int32>(0), os, isBigEndian);
            } else {
                os.put(static_cast<byte>(dtype_));
                num2bytes<int32>(static_cast<int32>(data_.d->size()), os, isBigEndian);
                for (auto &var : *data_.d)
                    var.write(os, isBigEndian);
            }
            break;
        case Compound:
            if (data_.d == nullptr) {
                os.put(End);
                break;
            }
            for (auto &var : *data_.d)
                var.write(os, isBigEndian);
            os.put(End);
            break;
        default:
            break;
    }
}

std::string Tag::toSnbt() const {
    return std::string();
}

Tag &Tag::operator=(const Tag &other) {
    if (name_ != nullptr) {
        delete name_;
        name_ = nullptr;
    }
    if (isString() && data_.s != nullptr) {
        delete data_.s;
        data_.s = nullptr;
    } else if (isArray() && data_.bs != nullptr) {
        delete data_.bs;
        data_.bs = nullptr;
    } else if (isIntArray() && data_.is != nullptr) {
        delete data_.is;
        data_.is = nullptr;
    } else if (isLongArray() && data_.ls != nullptr) {
        delete data_.ls;
        data_.ls = nullptr;
    } else if (isContainer() && data_.d != nullptr) {
        delete data_.d;
        data_.d = nullptr;
    }
    isPuredata_ = other.isPuredata_;
    isBigEndian_ = other.isBigEndian_;
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
    else if (other.isArray() && other.data_.bs != nullptr)
        data_.bs = new std::vector<byte>(*other.data_.bs);
    else if (other.isIntArray() && other.data_.is != nullptr)
        data_.is = new std::vector<int32>(*other.data_.is);
    else if (other.isLongArray() && other.data_.ls != nullptr)
        data_.ls = new std::vector<int64>(*other.data_.ls);
    else if (other.isContainer() && other.data_.d != nullptr)
        data_.d = new std::vector<Tag>(*other.data_.d);
    return *this;
}

Tag &Tag::operator=(Tag &&other) noexcept {
    if (name_ != nullptr) {
        delete name_;
        name_ = nullptr;
    }
    if (isString() && data_.s != nullptr) {
        delete data_.s;
        data_.s = nullptr;
    } else if (isArray() && data_.bs != nullptr) {
        delete data_.bs;
        data_.bs = nullptr;
    } else if (isIntArray() && data_.is != nullptr) {
        delete data_.is;
        data_.is = nullptr;
    } else if (isLongArray() && data_.ls != nullptr) {
        delete data_.ls;
        data_.ls = nullptr;
    } else if (isContainer() && data_.d != nullptr) {
        delete data_.d;
        data_.d = nullptr;
    }
    isPuredata_ = other.isPuredata_;
    isBigEndian_ = other.isBigEndian_;
    type_ = other.type_;
    dtype_ = other.dtype_;
    name_ = other.name_;
    other.name_ = nullptr;
    data_ = other.data_;
    other.data_.s = nullptr;
    return *this;
}

Tag &Tag::operator[](const std::string &name) {
    return get(name);
}

void Tag::construcTrework(std::istream &is) {
    if (!isPuredata_) {
        TagTypes type = static_cast<TagTypes>(is.get());
        NBT_ASSERT(type == type_);
        if (type != type_)
            return;
        int16 nameLen = bytes2num<int16>(is, isBigEndian_);
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
}

void Tag::numConstruct(std::istream &is) {
    switch (type_) {
        case Byte:
            data_.n.i8 = bytes2num<int8>(is, isBigEndian_);
            break;
        case Short:
            data_.n.i16 = bytes2num<int16>(is, isBigEndian_);
            break;
        case Int:
            data_.n.i32 = bytes2num<int32>(is, isBigEndian_);
            break;
        case Long:
            data_.n.i64 = bytes2num<int64>(is, isBigEndian_);
            break;
        case Float:
            data_.n.f32 = bytes2num<fp32>(is, isBigEndian_);
            break;
        case Double:
            data_.n.f64 = bytes2num<fp64>(is, isBigEndian_);
            break;
        default:
            break;
    }
}

void Tag::stringConstruct(std::istream &is) {
    int16 strlen = bytes2num<int16>(is, isBigEndian_);
    if (isString() && data_.s != nullptr) {
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

void Tag::arrayConstruct(std::istream &is) {
    int32 dsize = bytes2num<int32>(is, isBigEndian_);
    int32 size = 0;
    if (isArray()) {
        if (data_.bs != nullptr)
            delete data_.bs;
        data_.bs = new std::vector<byte>();
        data_.bs->reserve(dsize);
    } else if (isIntArray()) {
        if (data_.is != nullptr)
            delete data_.is;
        data_.is = new std::vector<int32>();
        data_.is->reserve(dsize);
    } else if (isLongArray()) {
        if (data_.ls != nullptr)
            delete data_.ls;
        data_.ls = new std::vector<int64>();
        data_.ls->reserve(dsize);
    } else
        return;
    while (!is.eof() && size < dsize) {
        if (isArray()) {
            data_.bs->emplace_back(bytes2num<byte>(is, isBigEndian_));
        } else if (isIntArray()) {
            data_.is->emplace_back(bytes2num<int32>(is, isBigEndian_));
        } else if (isLongArray()) {
            data_.ls->emplace_back(bytes2num<int64>(is, isBigEndian_));
        } else
            break;
        ++size;
    }
}

void Tag::listConstruct(std::istream &is) {
    dtype_ = static_cast<TagTypes>(is.get());
    int32 dsize = bytes2num<int32>(is, isBigEndian_);
    int32 size = 0;
    if (data_.d != nullptr)
        delete data_.d;
    data_.d = new std::vector<Tag>();
    data_.d->reserve(dsize);
    while (!is.eof() && size++ < dsize)
        data_.d->emplace_back(Tag(dtype_, is, isBigEndian_, 0, true));
}

void Tag::compoundConstruct(std::istream &is) {
    if (data_.d != nullptr)
        delete data_.d;
    data_.d = new std::vector<Tag>();
    while (!is.eof()) {
        TagTypes type = static_cast<TagTypes>(is.peek());
        if (type == End) {
            is.get();         // give up end-value and move stream point to next byte.
            break;
        }
        data_.d->emplace_back(Tag(type, is, isBigEndian_, 0, false));
    }
}

}
