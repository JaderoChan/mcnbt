// The "mcnbt" library, in C++11.
//
// Webs: https://github.com/JaderoChan/mcnbt
// You can contact me by email: c_dl_cn@outlook.com

// MIT License
//
// Copyright (c) 2024-2026 頔珞JaderoChan
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

/**
 * @file mcnbt.hpp
 * @brief A header-only C++ library for reading and writing Minecraft NBT format.
 * @author 頔珞 JaderoChan
 * @version 2.1.0
 */

#ifndef MCNBT_MCNBT_HPP
#define MCNBT_MCNBT_HPP

#include <cstdint>
#include <cstddef>
#include <cstring>
#include <string>
#include <vector>
#include <map>
#include <utility>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <type_traits>
#include <iterator>
#include <functional>

#include <mcnbt/config.hpp>
#ifdef MCNBT_HAS_ZLIB
    #include <mcnbt/gzip.hpp>
#endif

namespace nbt
{

/**
 * @brief Insertion-order-preserving map backed by std::vector with a std::map-compatible key interface.
 * @details Intended as a drop-in replacement for the CompoundType template parameter
 *          when the insertion order of compound tag entries must be preserved.
 */
template<
    class Key,
    class T,
    class IgnoredLess = std::less<Key>,
    class Allocator   = std::allocator<std::pair<const Key, T>>
>
class OrderedMap
{
public:
    using key_type        = Key;
    using mapped_type     = T;
    using value_type      = std::pair<const Key, T>;
    using size_type       = size_t;
    using difference_type = std::ptrdiff_t;
    using allocator_type  = Allocator;
    using key_compare     = IgnoredLess;

private:
    using StorageAlloc = typename std::allocator_traits<Allocator>::template rebind_alloc<value_type>;
    using Container    = std::vector<value_type, StorageAlloc>;

public:
    using reference              = value_type&;
    using const_reference        = const value_type&;
    using pointer                = typename std::allocator_traits<StorageAlloc>::pointer;
    using const_pointer          = typename std::allocator_traits<StorageAlloc>::const_pointer;
    using iterator               = typename Container::iterator;
    using const_iterator         = typename Container::const_iterator;
    using reverse_iterator       = typename Container::reverse_iterator;
    using const_reverse_iterator = typename Container::const_reverse_iterator;

    OrderedMap() = default;

    OrderedMap(std::initializer_list<value_type> ilist)
    {
        for (const auto& v : ilist)
            insert(v);
    }

    // ============
    // > Iterators
    // ============

    iterator               begin()  noexcept       { return data_.begin();  }
    iterator               end()    noexcept       { return data_.end();    }
    const_iterator         begin()  const noexcept { return data_.begin();  }
    const_iterator         end()    const noexcept { return data_.end();    }
    const_iterator         cbegin() const noexcept { return data_.cbegin(); }
    const_iterator         cend()   const noexcept { return data_.cend();   }
    reverse_iterator       rbegin() noexcept       { return data_.rbegin(); }
    reverse_iterator       rend()   noexcept       { return data_.rend();   }
    const_reverse_iterator rbegin() const noexcept { return data_.rbegin(); }
    const_reverse_iterator rend()   const noexcept { return data_.rend();   }

    // ===========
    // > Capacity
    // ===========

    bool      empty() const noexcept { return data_.empty(); }
    size_type size()  const noexcept { return data_.size();  }
    void      clear() noexcept       { data_.clear();        }
    void      reserve(size_type n)   { data_.reserve(n);     }

    // =================
    // > Element access
    // =================

    T& operator[](const Key& key)
    {
        auto it = find(key);
        if (it != end())
            return it->second;
        data_.push_back(value_type(key, T{}));
        return data_.back().second;
    }

    T& operator[](Key&& key)
    {
        auto it = find(key);
        if (it != end())
            return it->second;
        data_.push_back(value_type(std::move(key), T{}));
        return data_.back().second;
    }

    T& at(const Key& key)
    {
        auto it = find(key);
        if (it == end())
            throw std::out_of_range("OrderedMap::at(): key not found");
        return it->second;
    }

    const T& at(const Key& key) const
    {
        auto it = find(key);
        if (it == end())
            throw std::out_of_range("OrderedMap::at(): key not found");
        return it->second;
    }

    // =========
    // > Lookup
    // =========

    iterator find(const Key& key)
    {
        for (auto it = data_.begin(); it != data_.end(); ++it)
            if (it->first == key) return it;
        return data_.end();
    }

    const_iterator find(const Key& key) const
    {
        for (auto it = data_.cbegin(); it != data_.cend(); ++it)
            if (it->first == key) return it;
        return data_.cend();
    }

    size_type count(const Key& key) const
    {
        return find(key) != data_.cend() ? 1 : 0;
    }

    // ============
    // > Modifiers
    // ============

    std::pair<iterator, bool> insert(const value_type& val)
    {
        auto it = find(val.first);
        if (it != end())
            return {it, false};
        data_.push_back(val);
        return {std::prev(data_.end()), true};
    }

    template<typename K, typename... Args>
    std::pair<iterator, bool> emplace(K&& key, Args&&... args)
    {
        auto it = find(key);
        if (it != end())
            return {it, false};
        data_.emplace_back(
            std::piecewise_construct,
            std::forward_as_tuple(std::forward<K>(key)),
            std::forward_as_tuple(std::forward<Args>(args)...));
        return {std::prev(data_.end()), true};
    }

    size_type erase(const Key& key)
    {
        auto it = find(key);
        if (it == end()) return 0;
        data_.erase(it);
        return 1;
    }

    iterator erase(const_iterator pos)
    {
        return data_.erase(pos);
    }

private:
    Container data_;
};

// TagGetter forward declaration
namespace detail
{

template<typename T, typename BasicTagType>
struct TagGetter;

} // namespace detail

/**
 * @brief A type-safe NBT (Named Binary Tag) value.
 * @tparam ListType     Sequence container template for TT_LIST payloads (default: std::vector).
 * @tparam CompoundType Associative container template for TT_COMPOUND payloads (default: std::map).
 *                      Use OrderedMap to preserve entry insertion order.
 * @tparam AllocatorType Allocator template applied to all internal containers.
 */
template<
    template<typename U, typename... Args> class ListType = std::vector,
    template<typename K, typename V, typename... Args> class CompoundType = std::map,
    template<typename U> class AllocatorType = std::allocator
>
class BasicTag
{
private:
    using BasicTagType = BasicTag<ListType, CompoundType, AllocatorType>;

public:
    using value_type      = BasicTagType;
    using reference       = BasicTagType&;
    using const_reference = const BasicTagType&;
    using size_type       = size_t;

    using StringT         = std::string;
    using ByteArrayT      = std::vector<int8_t,  AllocatorType<int8_t>>;
    using IntArrayT       = std::vector<int32_t, AllocatorType<int32_t>>;
    using LongArrayT      = std::vector<int64_t, AllocatorType<int64_t>>;
    using ListT           = ListType<BasicTagType, AllocatorType<BasicTagType>>;
    using CompoundT       = CompoundType<
        StringT,
        BasicTagType,
        std::less<StringT>,
        AllocatorType<std::pair<const StringT, BasicTagType>>>;

    /** @brief NBT tag type identifier. */
    enum TagType : uint8_t
    {
        TT_END        = 0,
        TT_BYTE       = 1,
        TT_SHORT      = 2,
        TT_INT        = 3,
        TT_LONG       = 4,
        TT_FLOAT      = 5,
        TT_DOUBLE     = 6,
        TT_BYTE_ARRAY = 7,
        TT_STRING     = 8,
        TT_LIST       = 9,
        TT_COMPOUND   = 10,
        TT_INT_ARRAY  = 11,
        TT_LONG_ARRAY = 12
    };

    // ===========
    // > Iterator
    // ===========

    /**
     * @brief Unified forward iterator over TT_LIST and TT_COMPOUND tags.
     * @details Dereference yields the element value (BasicTagType&).
     *          key() returns the compound entry key and throws std::domain_error
     *          when the iterator belongs to a list tag.
     */
    template<bool IsConst>
    class iter_impl
    {
        friend class BasicTag;

        using TagPtr = typename std::conditional<IsConst, const BasicTag*, BasicTag*>::type;
        using TagRef = typename std::conditional<IsConst, const BasicTag&, BasicTag&>::type;
        using ListIt = typename std::conditional<IsConst,
            typename ListT::const_iterator, typename ListT::iterator>::type;
        using CmpIt  = typename std::conditional<IsConst,
            typename CompoundT::const_iterator, typename CompoundT::iterator>::type;

        bool   isCompound_;
        ListIt listIt_;
        CmpIt  cmpIt_;

        explicit iter_impl(ListIt it) : isCompound_(false), listIt_(it), cmpIt_()   {}
        explicit iter_impl(CmpIt  it) : isCompound_(true),  listIt_(),   cmpIt_(it) {}

    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type        = BasicTag;
        using reference         = TagRef;
        using pointer           = TagPtr;
        using difference_type   = std::ptrdiff_t;

        reference operator*()  const { return isCompound_ ? cmpIt_->second : *listIt_; }
        pointer   operator->() const { return &(**this); }

        iter_impl& operator++()    { isCompound_ ? (void)++cmpIt_ : (void)++listIt_; return *this; }
        iter_impl  operator++(int) { auto t = *this; ++(*this); return t; }

        bool operator==(const iter_impl& o) const
        {
            if (isCompound_ != o.isCompound_) return false;
            return isCompound_ ? cmpIt_ == o.cmpIt_ : listIt_ == o.listIt_;
        }
        bool operator!=(const iter_impl& o) const { return !(*this == o); }

        /**
         * @brief Returns the current compound entry key.
         * @throws std::domain_error when iterating a list.
         */
        const StringT& key() const
        {
            if (!isCompound_)
                throw std::domain_error("nbt::iterator::key(): not a compound iterator");
            return cmpIt_->first;
        }
    };

    using iterator       = iter_impl<false>;
    using const_iterator = iter_impl<true>;

    // ============================
    // > Construct and deconstruct
    // ============================

    /** @brief Constructs a TT_END (null) tag. */
    BasicTag() noexcept : type_(TT_END), listItemType_(TT_END)
    { value_.i64 = 0; }

    /** @brief Constructs an empty tag of the specified type, allocating heap storage when required. */
    explicit BasicTag(TagType t) : type_(t), listItemType_(TT_END)
    {
        value_.i64 = 0;
        switch (t)
        {
            case TT_STRING:     value_.ptr = new StringT();     break;
            case TT_BYTE_ARRAY: value_.ptr = new ByteArrayT();  break;
            case TT_INT_ARRAY:  value_.ptr = new IntArrayT();   break;
            case TT_LONG_ARRAY: value_.ptr = new LongArrayT();  break;
            case TT_LIST:       value_.ptr = new ListT();       break;
            case TT_COMPOUND:   value_.ptr = new CompoundT();   break;
            default: break;
        }
    }

    /** @brief Constructs a TT_BYTE tag. */
    BasicTag(int8_t v) noexcept : type_(TT_BYTE), listItemType_(TT_END)
    { value_.i64 = 0; value_.i8 = v; }

    /** @brief Constructs a TT_BYTE tag with boolean value. */
    BasicTag(bool v) noexcept : type_(TT_BYTE), listItemType_(TT_END)
    { value_.i64 = 0; value_.i8 = static_cast<int8_t>(v); }

    /** @brief Constructs a TT_SHORT tag. */
    BasicTag(int16_t v) noexcept : type_(TT_SHORT), listItemType_(TT_END)
    { value_.i64 = 0; value_.i16 = v; }

    /** @brief Constructs a TT_INT tag. */
    BasicTag(int32_t v) noexcept : type_(TT_INT), listItemType_(TT_END)
    { value_.i64 = 0; value_.i32 = v; }

    /** @brief Constructs a TT_LONG tag. */
    BasicTag(int64_t v) noexcept : type_(TT_LONG), listItemType_(TT_END)
    { value_.i64 = v; }

    /** @brief Constructs a TT_FLOAT tag. */
    BasicTag(float v) noexcept : type_(TT_FLOAT), listItemType_(TT_END)
    { value_.i64 = 0; value_.f32 = v; }

    /** @brief Constructs a TT_DOUBLE tag. */
    BasicTag(double v) noexcept : type_(TT_DOUBLE), listItemType_(TT_END)
    { value_.f64 = v; }

    /** @brief Constructs a TT_STRING tag from an lvalue string. */
    BasicTag(const StringT& v) : type_(TT_STRING), listItemType_(TT_END)
    { value_.ptr = new StringT(v); }

    /** @brief Constructs a TT_STRING tag from an rvalue string. */
    BasicTag(StringT&& v) : type_(TT_STRING), listItemType_(TT_END)
    { value_.ptr = new StringT(std::move(v)); }

    /** @brief Constructs a TT_STRING tag from a null-terminated character array. */
    BasicTag(const typename StringT::value_type* v) : type_(TT_STRING), listItemType_(TT_END)
    { value_.ptr = new StringT(v); }

    /** @brief Constructs a TT_BYTE_ARRAY tag from an lvalue byte array. */
    BasicTag(const ByteArrayT& v) : type_(TT_BYTE_ARRAY), listItemType_(TT_END)
    { value_.ptr = new ByteArrayT(v); }

    /** @brief Constructs a TT_BYTE_ARRAY tag from an rvalue byte array. */
    BasicTag(ByteArrayT&& v) : type_(TT_BYTE_ARRAY), listItemType_(TT_END)
    { value_.ptr = new ByteArrayT(std::move(v)); }

    /** @brief Constructs a TT_INT_ARRAY tag from an lvalue int array. */
    BasicTag(const IntArrayT& v) : type_(TT_INT_ARRAY), listItemType_(TT_END)
    { value_.ptr = new IntArrayT(v); }

    /** @brief Constructs a TT_INT_ARRAY tag from an rvalue int array. */
    BasicTag(IntArrayT&& v) : type_(TT_INT_ARRAY), listItemType_(TT_END)
    { value_.ptr = new IntArrayT(std::move(v)); }

    /** @brief Constructs a TT_LONG_ARRAY tag from an lvalue long array. */
    BasicTag(const LongArrayT& v) : type_(TT_LONG_ARRAY), listItemType_(TT_END)
    { value_.ptr = new LongArrayT(v); }

    /** @brief Constructs a TT_LONG_ARRAY tag from an rvalue long array. */
    BasicTag(LongArrayT&& v) : type_(TT_LONG_ARRAY), listItemType_(TT_END)
    { value_.ptr = new LongArrayT(std::move(v)); }

    /** @brief Constructs a TT_LIST tag from an lvalue list; deduces the element type from the first element. */
    BasicTag(const ListT& v) : type_(TT_LIST), listItemType_(TT_END)
    {
        if (!v.empty())
            listItemType_ = v.front().type_;
        value_.ptr = new ListT(v);
    }

    /** @brief Constructs a TT_LIST tag from an rvalue list; deduces the element type from the first element. */
    BasicTag(ListT&& v) : type_(TT_LIST), listItemType_(TT_END)
    {
        if (!v.empty())
            listItemType_ = v.front().type_;
        value_.ptr = new ListT(std::move(v));
    }

    /** @brief Constructs a TT_COMPOUND tag from an lvalue compound map. */
    BasicTag(const CompoundT& v) : type_(TT_COMPOUND), listItemType_(TT_END)
    { value_.ptr = new CompoundT(v); }

    /** @brief Constructs a TT_COMPOUND tag from an rvalue compound map. */
    BasicTag(CompoundT&& v) : type_(TT_COMPOUND), listItemType_(TT_END)
    { value_.ptr = new CompoundT(std::move(v)); }

    /** @brief Copy constructor; performs a deep copy of the heap-allocated payload. */
    BasicTag(const BasicTag& other) : type_(other.type_), listItemType_(other.listItemType_)
    { copyValueFrom(other); }

    /** @brief Move constructor; transfers ownership of the heap-allocated payload. */
    BasicTag(BasicTag&& other) noexcept : type_(other.type_), listItemType_(other.listItemType_), value_(other.value_)
    {
        other.type_      = TT_END;
        other.value_.i64 = 0;
    }

    /** @brief Unified copy/move assignment operator via copy-and-swap idiom. */
    BasicTag& operator=(BasicTag other) noexcept
    {
        using std::swap;
        swap(type_,         other.type_);
        swap(listItemType_, other.listItemType_);
        swap(value_,        other.value_);
        return *this;
    }

    /** @brief Destructor; releases the heap-allocated payload if present. */
    ~BasicTag() { destroyValue(); }

    // ================================
    // > Convenience static constructs
    // ================================

    /** @brief Convenience functions for constructing List. */
    static BasicTag list()     { return BasicTag(TT_LIST); }

    /** @brief Convenience functions for constructing Compound. */
    static BasicTag compound() { return BasicTag(TT_COMPOUND); }

    // ===============
    // > Type queries
    // ===============

    /** @brief Returns the runtime tag type. */
    TagType type()      const noexcept { return type_; }

    bool isEnd()        const noexcept { return type_ == TT_END;        }
    bool isByte()       const noexcept { return type_ == TT_BYTE;       }
    bool isShort()      const noexcept { return type_ == TT_SHORT;      }
    bool isInt()        const noexcept { return type_ == TT_INT;        }
    bool isLong()       const noexcept { return type_ == TT_LONG;       }
    bool isFloat()      const noexcept { return type_ == TT_FLOAT;      }
    bool isDouble()     const noexcept { return type_ == TT_DOUBLE;     }
    bool isString()     const noexcept { return type_ == TT_STRING;     }
    bool isByteArray()  const noexcept { return type_ == TT_BYTE_ARRAY; }
    bool isIntArray()   const noexcept { return type_ == TT_INT_ARRAY;  }
    bool isLongArray()  const noexcept { return type_ == TT_LONG_ARRAY; }
    bool isList()       const noexcept { return type_ == TT_LIST;       }
    bool isCompound()   const noexcept { return type_ == TT_COMPOUND;   }

    /** @brief Returns true when the tag is any integer type (byte, short, int, or long). */
    bool isInteger()    const noexcept { return isByte() || isShort() || isInt() || isLong(); }
    /** @brief Returns true when the tag is TT_FLOAT or TT_DOUBLE. */
    bool isFloatPoint() const noexcept { return isFloat() || isDouble(); }
    /** @brief Returns true when the tag is any numeric type. */
    bool isNumber()     const noexcept { return isInteger() || isFloatPoint(); }
    /** @brief Returns true when the tag is a typed array (byte, int, or long array). */
    bool isArray()      const noexcept { return isByteArray() || isIntArray() || isLongArray(); }
    /** @brief Returns true when the tag is TT_LIST or TT_COMPOUND. */
    bool isContainer()  const noexcept { return isList() || isCompound(); }
    /** @brief Returns true when the tag is a number or a string. */
    bool isPrimitive()  const noexcept { return isNumber() || isString(); }

    // ===============
    // > Value access
    // ===============

    /**
     * @brief Returns the byte value.
     * @throws std::domain_error if the tag is not TT_BYTE.
     */
    int8_t   getByte()   const { checkType(TT_BYTE);   return value_.i8;  }
    /**
     * @brief Returns a mutable reference to the byte value.
     * @throws std::domain_error if the tag is not TT_BYTE.
     */
    int8_t&  getByte()         { checkType(TT_BYTE);   return value_.i8;  }
    /**
     * @brief Returns the short value.
     * @throws std::domain_error if the tag is not TT_SHORT.
     */

    int16_t  getShort()  const { checkType(TT_SHORT);  return value_.i16; }
    /**
     * @brief Returns a mutable reference to the short value.
     * @throws std::domain_error if the tag is not TT_SHORT.
     */
    int16_t& getShort()        { checkType(TT_SHORT);  return value_.i16; }
    /**
     * @brief Returns the int value.
     * @throws std::domain_error if the tag is not TT_INT.
     */

    int32_t  getInt()    const { checkType(TT_INT);    return value_.i32; }
    /**
     * @brief Returns a mutable reference to the int value.
     * @throws std::domain_error if the tag is not TT_INT.
     */
    int32_t& getInt()          { checkType(TT_INT);    return value_.i32; }
    /**
     * @brief Returns the long value.
     * @throws std::domain_error if the tag is not TT_LONG.
     */

    int64_t  getLong()   const { checkType(TT_LONG);   return value_.i64; }
    /**
     * @brief Returns a mutable reference to the long value.
     * @throws std::domain_error if the tag is not TT_LONG.
     */
    int64_t& getLong()         { checkType(TT_LONG);   return value_.i64; }
    /**
     * @brief Returns the float value.
     * @throws std::domain_error if the tag is not TT_FLOAT.
     */

    float    getFloat()  const { checkType(TT_FLOAT);  return value_.f32; }
    /**
     * @brief Returns a mutable reference to the float value.
     * @throws std::domain_error if the tag is not TT_FLOAT.
     */
    float&   getFloat()        { checkType(TT_FLOAT);  return value_.f32; }
    /**
     * @brief Returns the double value.
     * @throws std::domain_error if the tag is not TT_DOUBLE.
     */

    double   getDouble() const { checkType(TT_DOUBLE); return value_.f64; }
    /**
     * @brief Returns a mutable reference to the double value.
     * @throws std::domain_error if the tag is not TT_DOUBLE.
     */
    double&  getDouble()       { checkType(TT_DOUBLE); return value_.f64; }

    /**
     * @brief Returns a const reference to the string value.
     * @throws std::domain_error if not TT_STRING.
     */
    const StringT& getString() const
    { checkType(TT_STRING); return *static_cast<StringT*>(value_.ptr); }
    /**
     * @brief Returns a mutable reference to the string value.
     * @throws std::domain_error if not TT_STRING.
     */
    StringT& getString()
    { checkType(TT_STRING); return *static_cast<StringT*>(value_.ptr); }

    /**
     * @brief Returns a const reference to the byte array.
     * @throws std::domain_error if not TT_BYTE_ARRAY.
     */
    const ByteArrayT& getByteArray() const
    { checkType(TT_BYTE_ARRAY); return *static_cast<ByteArrayT*>(value_.ptr); }
    /**
     * @brief Returns a mutable reference to the byte array.
     * @throws std::domain_error if not TT_BYTE_ARRAY.
     */
    ByteArrayT& getByteArray()
    { checkType(TT_BYTE_ARRAY); return *static_cast<ByteArrayT*>(value_.ptr); }

    /**
     * @brief Returns a const reference to the int array.
     * @throws std::domain_error if not TT_INT_ARRAY.
     */
    const IntArrayT& getIntArray() const
    { checkType(TT_INT_ARRAY); return *static_cast<IntArrayT*>(value_.ptr); }
    /**
     * @brief Returns a mutable reference to the int array.
     * @throws std::domain_error if not TT_INT_ARRAY.
     */
    IntArrayT& getIntArray()
    { checkType(TT_INT_ARRAY); return *static_cast<IntArrayT*>(value_.ptr); }

    /**
     * @brief Returns a const reference to the long array.
     * @throws std::domain_error if not TT_LONG_ARRAY.
     */
    const LongArrayT& getLongArray() const
    { checkType(TT_LONG_ARRAY); return *static_cast<LongArrayT*>(value_.ptr); }
    /**
     * @brief Returns a mutable reference to the long array.
     * @throws std::domain_error if not TT_LONG_ARRAY.
     */
    LongArrayT& getLongArray()
    { checkType(TT_LONG_ARRAY); return *static_cast<LongArrayT*>(value_.ptr); }

    /**
     * @brief Returns a const reference to the list.
     * @throws std::domain_error if not TT_LIST.
     */
    const ListT& getList() const
    { checkType(TT_LIST); return *static_cast<ListT*>(value_.ptr); }
    /**
     * @brief Returns a mutable reference to the list.
     * @throws std::domain_error if not TT_LIST.
     */
    ListT& getList()
    { checkType(TT_LIST); return *static_cast<ListT*>(value_.ptr); }

    /**
     * @brief Returns a const reference to the compound map.
     * @throws std::domain_error if not TT_COMPOUND.
     */
    const CompoundT& getCompound() const
    { checkType(TT_COMPOUND); return *static_cast<CompoundT*>(value_.ptr); }
    /**
     * @brief Returns a mutable reference to the compound map.
     * @throws std::domain_error if not TT_COMPOUND.
     */
    CompoundT& getCompound()
    { checkType(TT_COMPOUND); return *static_cast<CompoundT*>(value_.ptr); }

    /**
     * @brief Returns the payload cast to type T.
     * @tparam T One of the supported native or container types.
     * @throws std::domain_error if the tag type does not match T.
     */
    template<typename T>
    T get() const
    {
        return detail::TagGetter<T, BasicTagType>::get(*this);
    }
    /**
     * @brief Returns a mutable reference to the payload cast to type T.
     * @tparam T One of the supported native or container types.
     * @throws std::domain_error if the tag type does not match T.
     */
    template<typename T>
    T& get()
    {
        return detail::TagGetter<T, BasicTagType>::getRef(*this);
    }

    // ===========================
    // > Implicit type conversion
    // ===========================

    // Explicit for arithmetic types prevents built-in operator candidates from being synthesized.
    explicit operator int8_t()  const { return getByte();      }
    explicit operator int16_t() const { return getShort();     }
    explicit operator int32_t() const { return getInt();       }
    explicit operator int64_t() const { return getLong();      }
    explicit operator float()   const { return getFloat();     }
    explicit operator double()  const { return getDouble();    }
    operator StringT()          const { return getString();    }
    operator ByteArrayT()       const { return getByteArray(); }
    operator IntArrayT()        const { return getIntArray();  }
    operator LongArrayT()       const { return getLongArray(); }
    operator CompoundT()        const { return getCompound();  }
    operator ListT()            const { return getList();      }

    // =================
    // > List item type
    // =================

    /**
     * @brief Returns the element type of a TT_LIST tag.
     * @throws std::domain_error if the tag is not TT_LIST.
     */
    TagType listItemType() const
    {
        checkType(TT_LIST);
        return listItemType_;
    }

    // ===========
    // > Capacity
    // ===========

    /**
     * @brief Returns the number of elements in the tag's container.
     * @details Returns 0 for non-container types (scalars, TT_END).
     */
    size_type size() const noexcept
    {
        switch (type_)
        {
            case TT_STRING:     return value_.ptr ? static_cast<StringT*>(value_.ptr)->size()    : 0;
            case TT_BYTE_ARRAY: return value_.ptr ? static_cast<ByteArrayT*>(value_.ptr)->size() : 0;
            case TT_INT_ARRAY:  return value_.ptr ? static_cast<IntArrayT*>(value_.ptr)->size()  : 0;
            case TT_LONG_ARRAY: return value_.ptr ? static_cast<LongArrayT*>(value_.ptr)->size() : 0;
            case TT_LIST:       return value_.ptr ? static_cast<ListT*>(value_.ptr)->size()      : 0;
            case TT_COMPOUND:   return value_.ptr ? static_cast<CompoundT*>(value_.ptr)->size()  : 0;
            default:            return 0;
        }
    }

    /** @brief Returns true when the container is empty or the tag is a scalar. */
    bool empty() const noexcept { return size() == 0; }

    // =================
    // > Element access
    // =================

    /** @brief Subscript access for TT_LIST tags; no bounds checking. */
    reference operator[](size_type idx)
    {
        checkType(TT_LIST);
        return (*static_cast<ListT*>(value_.ptr))[idx];
    }

    /** @brief Subscript access for TT_LIST tags (const overload); no bounds checking. */
    const_reference operator[](size_type idx) const
    {
        checkType(TT_LIST);
        return (*static_cast<const ListT*>(value_.ptr))[idx];
    }

    /** @brief Subscript access for TT_COMPOUND tags; inserts a default-constructed tag if the key is absent. */
    reference operator[](const StringT& key)
    {
        checkType(TT_COMPOUND);
        return (*static_cast<CompoundT*>(value_.ptr))[key];
    }

    /**
     * @brief Subscript access for TT_COMPOUND tags (const overload).
     * @throws std::out_of_range if the key is not found.
     */
    const_reference operator[](const StringT& key) const
    {
        checkType(TT_COMPOUND);
        auto& c = *static_cast<const CompoundT*>(value_.ptr);
        auto it = c.find(key);
        if (it == c.end())
            throw std::out_of_range("nbt::BasicTag::operator[](): key not found");
        return it->second;
    }

    /**
     * @brief Bounds-checked index access for TT_LIST tags.
     * @throws std::out_of_range if idx is out of range.
     */
    reference at(size_type idx)
    {
        checkType(TT_LIST);
        auto& l = *static_cast<ListT*>(value_.ptr);
        if (idx >= l.size())
            throw std::out_of_range("nbt::BasicTag::at(): index out of range");
        return l[idx];
    }

    /**
     * @brief Bounds-checked index access for TT_LIST tags (const overload).
     * @throws std::out_of_range if idx is out of range.
     */
    const_reference at(size_type idx) const
    {
        checkType(TT_LIST);
        const auto& l = *static_cast<const ListT*>(value_.ptr);
        if (idx >= l.size())
            throw std::out_of_range("nbt::BasicTag::at(): index out of range");
        return l[idx];
    }

    /**
     * @brief Bounds-checked key access for TT_COMPOUND tags.
     * @throws std::out_of_range if the key is not found.
     */
    reference at(const StringT& key)
    {
        checkType(TT_COMPOUND);
        auto& c = *static_cast<CompoundT*>(value_.ptr);
        auto it = c.find(key);
        if (it == c.end())
            throw std::out_of_range("nbt::BasicTag::at(): key not found");
        return it->second;
    }

    /**
     * @brief Bounds-checked key access for TT_COMPOUND tags (const overload).
     * @throws std::out_of_range if the key is not found.
     */
    const_reference at(const StringT& key) const
    {
        checkType(TT_COMPOUND);
        const auto& c = *static_cast<const CompoundT*>(value_.ptr);
        auto it = c.find(key);
        if (it == c.end())
            throw std::out_of_range("nbt::BasicTag::at(): key not found");
        return it->second;
    }

    /**
     * @brief Returns a reference to the first element of a TT_LIST tag.
     * @throws std::out_of_range if the list is empty.
     */
    reference front()
    {
        checkType(TT_LIST);
        auto& l = *static_cast<ListT*>(value_.ptr);
        if (l.empty())
            throw std::out_of_range("nbt::BasicTag::front(): list is empty");
        return l.front();
    }

    /**
     * @brief Returns a const reference to the first element of a TT_LIST tag.
     * @throws std::out_of_range if the list is empty.
     */
    const_reference front() const
    {
        checkType(TT_LIST);
        const auto& l = *static_cast<const ListT*>(value_.ptr);
        if (l.empty())
            throw std::out_of_range("nbt::BasicTag::front(): list is empty");
        return l.front();
    }

    /**
     * @brief Returns a reference to the last element of a TT_LIST tag.
     * @throws std::out_of_range if the list is empty.
     */
    reference back()
    {
        checkType(TT_LIST);
        auto& l = *static_cast<ListT*>(value_.ptr);
        if (l.empty())
            throw std::out_of_range("nbt::BasicTag::back(): list is empty");
        return l.back();
    }

    /**
     * @brief Returns a const reference to the last element of a TT_LIST tag.
     * @throws std::out_of_range if the list is empty.
     */
    const_reference back() const
    {
        checkType(TT_LIST);
        const auto& l = *static_cast<const ListT*>(value_.ptr);
        if (l.empty())
            throw std::out_of_range("nbt::BasicTag::back(): list is empty");
        return l.back();
    }

    // ======================
    // > Lookup for compound
    // ======================

    /** @brief Returns true when the tag is TT_COMPOUND and contains the given key. */
    bool contains(const StringT& key) const noexcept
    {
        if (!isCompound() || !value_.ptr) return false;
        return static_cast<const CompoundT*>(value_.ptr)->count(key) > 0;
    }

    // ============
    // > Iterators
    // ============

    /**
     * @brief Returns an iterator to the first element.
     * @throws std::domain_error if the tag is not TT_LIST or TT_COMPOUND.
     */
    iterator begin()
    {
        if (isList())     return iterator(static_cast<ListT*>(value_.ptr)->begin());
        if (isCompound()) return iterator(static_cast<CompoundT*>(value_.ptr)->begin());
        throw std::domain_error("nbt::BasicTag::begin(): tag is not a list or compound");
    }
    /**
     * @brief Returns an iterator past the last element.
     * @throws std::domain_error if the tag is not TT_LIST or TT_COMPOUND.
     */
    iterator end()
    {
        if (isList())     return iterator(static_cast<ListT*>(value_.ptr)->end());
        if (isCompound()) return iterator(static_cast<CompoundT*>(value_.ptr)->end());
        throw std::domain_error("nbt::BasicTag::end(): tag is not a list or compound");
    }
    /** @brief Returns a const iterator to the first element (calls cbegin()). */
    const_iterator begin() const { return cbegin(); }
    /** @brief Returns a const iterator past the last element (calls cend()). */
    const_iterator end()   const { return cend(); }
    /**
     * @brief Returns a const iterator to the first element.
     * @throws std::domain_error if the tag is not TT_LIST or TT_COMPOUND.
     */
    const_iterator cbegin() const
    {
        if (isList())     return const_iterator(static_cast<const ListT*>(value_.ptr)->cbegin());
        if (isCompound()) return const_iterator(static_cast<const CompoundT*>(value_.ptr)->cbegin());
        throw std::domain_error("nbt::BasicTag::cbegin(): tag is not a list or compound");
    }
    /**
     * @brief Returns a const iterator past the last element.
     * @throws std::domain_error if the tag is not TT_LIST or TT_COMPOUND.
     */
    const_iterator cend() const
    {
        if (isList())     return const_iterator(static_cast<const ListT*>(value_.ptr)->cend());
        if (isCompound()) return const_iterator(static_cast<const CompoundT*>(value_.ptr)->cend());
        throw std::domain_error("nbt::BasicTag::cend(): tag is not a list or compound");
    }

    /**
     * @brief Returns a reference to the underlying CompoundT for key-value iteration.
     * @throws std::domain_error if the tag is not TT_COMPOUND.
     * @example `for (const auto& kv : tag.items()) { ... }`
     */
    CompoundT& items()
    { checkType(TT_COMPOUND); return *static_cast<CompoundT*>(value_.ptr); }
    /**
     * @brief Returns a const reference to the underlying CompoundT for key-value iteration.
     * @throws std::domain_error if not TT_COMPOUND.
     */
    const CompoundT& items() const
    { checkType(TT_COMPOUND); return *static_cast<const CompoundT*>(value_.ptr); }

    // ============
    // > Modifiers
    // ============

    /**
     * @brief Appends a tag to a TT_LIST tag; infers the element type on the first call.
     * @throws std::domain_error on type mismatch.
     */
    void pushBack(BasicTag val)
    {
        checkType(TT_LIST);
        auto& l = *static_cast<ListT*>(value_.ptr);

        if (listItemType_ == TT_END)
            listItemType_ = val.type_;
        else if (val.type_ != listItemType_)
            throw std::domain_error("nbt::BasicTag::pushBack(): element type does not match list item type");

        l.push_back(std::move(val));
    }

    /** @brief Constructs a tag in-place at the back of a TT_LIST tag; infers the element type on the first call. */
    template<typename... Args>
    reference emplaceBack(Args&&... args)
    {
        checkType(TT_LIST);
        auto& l = *static_cast<ListT*>(value_.ptr);
        l.emplace_back(std::forward<Args>(args)...);
        TagType newType = l.back().type_;
        if (listItemType_ == TT_END)
            listItemType_ = newType;
        else if (newType != listItemType_)
        {
            l.pop_back();
            throw std::domain_error("nbt::BasicTag::emplaceBack(): element type does not match list item type");
        }
        return l.back();
    }

    /**
     * @brief Inserts a tag at the specified index in a TT_LIST tag.
     * @throws std::out_of_range if idx is out of range.
     * @throws std::domain_error if the tag type does not match the list element type.
     */
    void insert(size_type idx, BasicTag val)
    {
        checkType(TT_LIST);
        auto& l = *static_cast<ListT*>(value_.ptr);
        if (idx > l.size())
            throw std::out_of_range("nbt::BasicTag::insert(): index out of range");
        if (listItemType_ == TT_END)
            listItemType_ = val.type_;
        else if (val.type_ != listItemType_)
            throw std::domain_error("nbt::BasicTag::insert(): element type mismatch");
        l.insert(l.begin() + static_cast<typename ListT::difference_type>(idx), std::move(val));
    }

    /**
     * @brief Inserts a key-value entry into a TT_COMPOUND tag.
     * @return true if the entry was inserted; false if the key already existed.
     */
    bool insert(const StringT& key, BasicTag val)
    {
        checkType(TT_COMPOUND);
        return static_cast<CompoundT*>(value_.ptr)->emplace(key, std::move(val)).second;
    }

    /**
     * @brief Inserts a key-value pair into a TT_COMPOUND tag.
     * @return true if the entry was inserted; false if the key already existed.
     */
    bool insert(const std::pair<const StringT, BasicTag>& kv)
    {
        checkType(TT_COMPOUND);
        return static_cast<CompoundT*>(value_.ptr)->insert(kv).second;
    }

    /** @brief Shorthand for pushBack() on TT_LIST. */
    BasicTag& operator<<(BasicTag val)
    {
        pushBack(std::move(val));
        return *this;
    }

    /** @brief Shorthand for pushBack(key, val) on TT_COMPOUND. */
    BasicTag& operator<<(std::pair<StringT, BasicTag> kv)
    {
        insert(std::move(kv.first), std::move(kv.second));
        return *this;
    }

    /**
     * @brief Erases the element at the specified index from a TT_LIST or typed array tag.
     * @throws std::out_of_range if idx is out of range.
     * @throws std::domain_error if the tag is not a list or array.
     */
    void erase(size_type idx)
    {
        if (isList())
        {
            auto& l = *static_cast<ListT*>(value_.ptr);
            if (idx >= l.size()) throw std::out_of_range("nbt::BasicTag::erase(): index out of range");
            l.erase(l.begin() + static_cast<typename ListT::difference_type>(idx));
            return;
        }

        if (isByteArray())
        {
            auto& a = *static_cast<ByteArrayT*>(value_.ptr);
            if (idx >= a.size()) throw std::out_of_range("nbt::BasicTag::erase(): index out of range");
            a.erase(a.begin() + static_cast<typename ByteArrayT::difference_type>(idx));
            return;
        }

        if (isIntArray())
        {
            auto& a = *static_cast<IntArrayT*>(value_.ptr);
            if (idx >= a.size()) throw std::out_of_range("nbt::BasicTag::erase(): index out of range");
            a.erase(a.begin() + static_cast<typename IntArrayT::difference_type>(idx));
            return;
        }

        if (isLongArray())
        {
            auto& a = *static_cast<LongArrayT*>(value_.ptr);
            if (idx >= a.size()) throw std::out_of_range("nbt::BasicTag::erase(): index out of range");
            a.erase(a.begin() + static_cast<typename LongArrayT::difference_type>(idx));
            return;
        }

        throw std::domain_error("nbt::BasicTag::erase(size_t): tag is not a list or array");
    }

    /**
     * @brief Erases the entry with the specified key from a TT_COMPOUND tag.
     * @return Number of entries removed (0 or 1).
     */
    size_type erase(const StringT& key)
    {
        checkType(TT_COMPOUND);
        return static_cast<CompoundT*>(value_.ptr)->erase(key);
    }

    /** @brief Clears all elements in the tag's container; resets listItemType_ to TT_END for lists. */
    void clear() noexcept
    {
        switch (type_)
        {
            case TT_STRING:     static_cast<StringT*>(value_.ptr)->clear();    break;
            case TT_BYTE_ARRAY: static_cast<ByteArrayT*>(value_.ptr)->clear(); break;
            case TT_INT_ARRAY:  static_cast<IntArrayT*>(value_.ptr)->clear();  break;
            case TT_LONG_ARRAY: static_cast<LongArrayT*>(value_.ptr)->clear(); break;
            case TT_LIST:
                static_cast<ListT*>(value_.ptr)->clear();
                listItemType_ = TT_END;
                break;
            case TT_COMPOUND:   static_cast<CompoundT*>(value_.ptr)->clear();  break;
            default: break;
        }
    }

    // =======================
    // > Binary serialization
    // =======================

    /**
     * @brief Parses a named root tag from a binary stream.
     * @param is        Input stream positioned at the start of a named tag.
     * @param bigEndian True for big-endian (Java Edition); false for little-endian (Bedrock Edition).
     * @return Pair of {root_name, root_tag}.
     * @note When MCNBT_HAS_ZLIB is defined, gzip-compressed streams are decompressed automatically.
     */
    static std::pair<StringT, BasicTagType> parse(std::istream& is, bool bigEndian, size_t headerSkip = 0)
    {
    #ifdef MCNBT_HAS_ZLIB
        if (gzip::isCompressed(is))
        {
            std::istringstream ss;
            {
                StringT content((std::istreambuf_iterator<char>(is)), std::istreambuf_iterator<char>());
                content = gzip::decompress(content);
                ss = std::istringstream(content);
            }
            if (headerSkip > 0) ss.seekg(static_cast<std::streamoff>(headerSkip), std::ios::cur);
            return parseNamed(ss, bigEndian);
        }
    #endif
        if (headerSkip > 0) is.seekg(static_cast<std::streamoff>(headerSkip), std::ios::cur);
        return parseNamed(is, bigEndian);
    }

    /**
     * @brief Parses a named root tag from a file.
     * @param filepath   Path to the binary NBT file.
     * @param bigEndian  True for big-endian; false for little-endian.
     * @param headerSkip Number of bytes to skip at the start of the file (e.g., 8 for some Bedrock map files).
     * @return Pair of {root_name, root_tag}.
     */
    static std::pair<StringT, BasicTagType>
    parse(const StringT& filepath, bool bigEndian, size_t headerSkip = 0)
    {
        std::ifstream ifs(filepath, std::ios::binary);
        if (!ifs.is_open())
            throw std::runtime_error("nbt::BasicTag::parse(): failed to open file: " + filepath);
        return parse(ifs, bigEndian, headerSkip);
    }

    /**
     * @brief Serializes this tag as a named root tag to a binary stream.
     * @param os        Output stream.
     * @param bigEndian True for big-endian; false for little-endian.
     * @param name      Root tag name (empty string is valid).
     */
    void dump(std::ostream& os, bool bigEndian, const StringT& name = StringT{}) const
    {
        writeNamed(os, bigEndian, name);
    }

    /**
     * @brief Serializes this tag as a named root tag to a file.
     * @param filepath  Destination file path.
     * @param bigEndian True for big-endian; false for little-endian.
     * @param name      Root tag name (empty string is valid).
     */
    void dump(const StringT& filepath, bool bigEndian, const StringT& name = StringT{}) const
    {
        std::ofstream ofs(filepath, std::ios::binary);
        if (!ofs.is_open())
            throw std::runtime_error("nbt::BasicTag::dump(): failed to open file: " + filepath);
        dump(ofs, bigEndian, name);
    }

#ifdef MCNBT_HAS_ZLIB
    /**
     * @brief Serializes and gzip-compresses this tag to a binary stream.
     * @param os        Output stream.
     * @param bigEndian True for big-endian; false for little-endian.
     * @param name      Root tag name.
     */
    void dumpCompressed(std::ostream& os, bool bigEndian, const StringT& name = StringT{}) const
    {
        std::ostringstream ss;
        writeNamed(ss, bigEndian, name);
        os << gzip::compress(ss.str());
    }

    /**
     * @brief Serializes and gzip-compresses this tag to a file.
     * @param filepath  Destination file path.
     * @param bigEndian True for big-endian; false for little-endian.
     * @param name      Root tag name.
     */
    void dumpCompressed(const StringT& filepath, bool bigEndian, const StringT& name = StringT{}) const
    {
        std::ofstream ofs(filepath, std::ios::binary);
        if (!ofs.is_open())
            throw std::runtime_error("nbt::BasicTag::dumpCompressed(): failed to open file: " + filepath);
        dumpCompressed(ofs, bigEndian, name);
    }
#endif

    // =============================
    // > SNBT (text representation)
    // =============================

    /**
     * @brief Serializes this tag to an SNBT string.
     * @param indent Spaces per indentation level.
     *               Negative: fully compact (no newlines, no spaces).
     *               Zero: newlines but no indentation or extra spaces.
     *               Positive: newlines with indentation.
     */
    StringT toSnbt(int indent = -1) const
    {
        return toSnbtImpl(indent, 0);
    }

    /**
     * @brief Parses an SNBT string and returns the corresponding tag.
     * @param s SNBT-formatted string.
     * @throws std::runtime_error on malformed input.
     */
    static BasicTagType fromSnbt(const StringT& s)
    {
        size_t pos = 0;
        BasicTagType result = snbtParseValue(s, pos);
        snbtSkipWs(s, pos);
        if (pos != s.size())
            throw std::runtime_error("nbt::BasicTag::fromSnbt(): trailing content after value");
        return result;
    }

    // =======
    // > Swap
    // =======

    /** @brief Swaps the contents of this tag and @p other without allocating. */
    void swap(BasicTagType& other) noexcept
    {
        using std::swap;
        swap(type_,         other.type_);
        swap(listItemType_, other.listItemType_);
        swap(value_,        other.value_);
    }

private:
    // ===================
    // > Internal storage
    // ===================

    // All heap-allocated payload types are stored through a void pointer so that
    // the union remains trivially copyable and reading an inactive member is avoided.
    union Storage
    {
        int8_t  i8;
        int16_t i16;
        int32_t i32;
        int64_t i64;
        float   f32;
        double  f64;
        void*   ptr;
    };

    TagType type_;
    TagType listItemType_;
    Storage value_;

    // ===================
    // > Internal helpers
    // ===================

    void checkType(TagType expected) const
    {
        if (type_ != expected)
            throw std::domain_error("nbt::BasicTag: tag type mismatch");
    }

    void destroyValue() noexcept
    {
        switch (type_)
        {
            case TT_STRING:     delete static_cast<StringT*>(value_.ptr);    break;
            case TT_BYTE_ARRAY: delete static_cast<ByteArrayT*>(value_.ptr); break;
            case TT_INT_ARRAY:  delete static_cast<IntArrayT*>(value_.ptr);  break;
            case TT_LONG_ARRAY: delete static_cast<LongArrayT*>(value_.ptr); break;
            case TT_LIST:       delete static_cast<ListT*>(value_.ptr);      break;
            case TT_COMPOUND:   delete static_cast<CompoundT*>(value_.ptr);  break;
            default: break;
        }
    }

    void copyValueFrom(const BasicTag& other)
    {
        switch (other.type_)
        {
            case TT_BYTE:   value_.i8  = other.value_.i8;  break;
            case TT_SHORT:  value_.i16 = other.value_.i16; break;
            case TT_INT:    value_.i32 = other.value_.i32; break;
            case TT_LONG:   value_.i64 = other.value_.i64; break;
            case TT_FLOAT:  value_.f32 = other.value_.f32; break;
            case TT_DOUBLE: value_.f64 = other.value_.f64; break;
            case TT_STRING:
                value_.ptr = other.value_.ptr
                    ? new StringT(*static_cast<const StringT*>(other.value_.ptr)) : nullptr;
                break;
            case TT_BYTE_ARRAY:
                value_.ptr = other.value_.ptr
                    ? new ByteArrayT(*static_cast<const ByteArrayT*>(other.value_.ptr)) : nullptr;
                break;
            case TT_INT_ARRAY:
                value_.ptr = other.value_.ptr
                    ? new IntArrayT(*static_cast<const IntArrayT*>(other.value_.ptr)) : nullptr;
                break;
            case TT_LONG_ARRAY:
                value_.ptr = other.value_.ptr
                    ? new LongArrayT(*static_cast<const LongArrayT*>(other.value_.ptr)) : nullptr;
                break;
            case TT_LIST:
                value_.ptr = other.value_.ptr
                    ? new ListT(*static_cast<const ListT*>(other.value_.ptr)) : nullptr;
                break;
            case TT_COMPOUND:
                value_.ptr = other.value_.ptr
                    ? new CompoundT(*static_cast<const CompoundT*>(other.value_.ptr)) : nullptr;
                break;
            default:
                value_.i64 = 0;
                break;
        }
    }

    // ===============
    // > IO utilities
    // ===============

    static bool isBigEndian() noexcept
    {
        static const int probe = 1;
        static const bool result = (reinterpret_cast<const char*>(&probe)[0] == 0);
        return result;
    }

    template<typename T>
    static T readBinary(std::istream& is, bool bigEndian)
    {
        char buf[sizeof(T)];
        is.read(buf, sizeof(T));
        if (bigEndian != isBigEndian())
            std::reverse(buf, buf + sizeof(T));
        T val;
        memcpy(&val, buf, sizeof(T));
        return val;
    }

    template<typename T>
    static void writeBinary(std::ostream& os, T val, bool bigEndian)
    {
        char buf[sizeof(T)];
        memcpy(buf, &val, sizeof(T));
        if (bigEndian != isBigEndian())
            std::reverse(buf, buf + sizeof(T));
        os.write(buf, sizeof(T));
    }

    static StringT readNbtString(std::istream& is, bool bigEndian)
    {
        int16_t len = readBinary<int16_t>(is, bigEndian);
        if (len <= 0)
            return StringT{};
        StringT s(static_cast<size_t>(len), typename StringT::value_type{});
        is.read(&s[0], len);
        return s;
    }

    static void writeNbtString(std::ostream& os, const StringT& s, bool bigEndian)
    {
        if (s.size() > static_cast<size_t>(std::numeric_limits<int16_t>::max()))
            throw std::length_error("nbt::BasicTag: string length exceeds the NBT 32767-byte limit");
        writeBinary<int16_t>(os, static_cast<int16_t>(s.size()), bigEndian);
        os.write(s.data(), static_cast<std::streamsize>(s.size()));
    }

    static StringT makeIndent(int width, int level)
    {
        if (width <= 0) return "";
        return StringT(static_cast<size_t>(width * level), ' ');
    }

    // =========================
    // > Binary parsing helpers
    // =========================

    static std::pair<StringT, BasicTagType>
    parseNamed(std::istream& is, bool bigEndian)
    {
        const int rawByte = is.get();
        if (rawByte == std::char_traits<char>::eof())
            throw std::runtime_error("nbt::BasicTag: illegal nbt tag data");
        const auto typeByte = static_cast<TagType>(static_cast<uint8_t>(rawByte));
        if (typeByte == TT_END)
            return {StringT{}, BasicTagType{}};

        StringT name = readNbtString(is, bigEndian);
        BasicTagType tag;
        tag.type_ = typeByte;
        parsePayload(is, bigEndian, tag);
        return {std::move(name), std::move(tag)};
    }

    static void parsePayload(std::istream& is, bool bigEndian, BasicTagType& tag)
    {
        switch (tag.type_)
        {
            case TT_BYTE:
                tag.value_.i8 = readBinary<int8_t>(is, bigEndian);
                break;
            case TT_SHORT:
                tag.value_.i16 = readBinary<int16_t>(is, bigEndian);
                break;
            case TT_INT:
                tag.value_.i32 = readBinary<int32_t>(is, bigEndian);
                break;
            case TT_LONG:
                tag.value_.i64 = readBinary<int64_t>(is, bigEndian);
                break;
            case TT_FLOAT:
                tag.value_.f32 = readBinary<float>(is, bigEndian);
                break;
            case TT_DOUBLE:
                tag.value_.f64 = readBinary<double>(is, bigEndian);
                break;
            case TT_STRING:
                tag.value_.ptr = new StringT(readNbtString(is, bigEndian));
                break;
            case TT_BYTE_ARRAY:
            {
                int32_t n = readBinary<int32_t>(is, bigEndian);
                auto* arr = new ByteArrayT();
                arr->reserve(static_cast<size_t>(n));
                for (int32_t i = 0; i < n; ++i)
                    arr->push_back(readBinary<int8_t>(is, bigEndian));
                tag.value_.ptr = arr;
                break;
            }
            case TT_INT_ARRAY:
            {
                int32_t n = readBinary<int32_t>(is, bigEndian);
                auto* arr = new IntArrayT();
                arr->reserve(static_cast<size_t>(n));
                for (int32_t i = 0; i < n; ++i)
                    arr->push_back(readBinary<int32_t>(is, bigEndian));
                tag.value_.ptr = arr;
                break;
            }
            case TT_LONG_ARRAY:
            {
                int32_t n = readBinary<int32_t>(is, bigEndian);
                auto* arr = new LongArrayT();
                arr->reserve(static_cast<size_t>(n));
                for (int32_t i = 0; i < n; ++i)
                    arr->push_back(readBinary<int64_t>(is, bigEndian));
                tag.value_.ptr = arr;
                break;
            }
            case TT_LIST:
            {
                tag.listItemType_ = static_cast<TagType>(static_cast<uint8_t>(is.get()));
                int32_t n = readBinary<int32_t>(is, bigEndian);
                auto* lst = new ListT();
                lst->reserve(static_cast<size_t>(n));
                for (int32_t i = 0; i < n; ++i)
                {
                    BasicTagType item;
                    item.type_ = tag.listItemType_;
                    parsePayload(is, bigEndian, item);
                    lst->push_back(std::move(item));
                }
                tag.value_.ptr = lst;
                break;
            }
            case TT_COMPOUND:
            {
                auto* cmp = new CompoundT();
                while (true)
                {
                    int peek = is.peek();
                    if (peek == static_cast<int>(TT_END))
                    {
                        is.get(); // consume the End Tag terminator
                        break;
                    }
                    else if (peek == EOF)
                    {
                        throw std::runtime_error("nbt::BasicTag: unclosed compound tag during parse");
                    }
                    auto named = parseNamed(is, bigEndian);
                    cmp->emplace(std::move(named.first), std::move(named.second));
                }
                tag.value_.ptr = cmp;
                break;
            }
            default:
                throw std::runtime_error("nbt::BasicTag: unknown tag type during parse");
        }
    }

    // =========================
    // > Binary writing helpers
    // =========================

    void writeNamed(std::ostream& os, bool bigEndian, const StringT& name) const
    {
        os.put(static_cast<char>(static_cast<uint8_t>(type_)));
        writeNbtString(os, name, bigEndian);
        writePayload(os, bigEndian);
    }

    void writePayload(std::ostream& os, bool bigEndian) const
    {
        switch (type_)
        {
            case TT_END:
                os.put('\0');
                break;
            case TT_BYTE:
                writeBinary<int8_t>(os, value_.i8, bigEndian);
                break;
            case TT_SHORT:
                writeBinary<int16_t>(os, value_.i16, bigEndian);
                break;
            case TT_INT:
                writeBinary<int32_t>(os, value_.i32, bigEndian);
                break;
            case TT_LONG:
                writeBinary<int64_t>(os, value_.i64, bigEndian);
                break;
            case TT_FLOAT:
                writeBinary<float>(os, value_.f32, bigEndian);
                break;
            case TT_DOUBLE:
                writeBinary<double>(os, value_.f64, bigEndian);
                break;
            case TT_STRING:
            {
                const StringT empty{};
                const auto& s = value_.ptr ? *static_cast<const StringT*>(value_.ptr) : empty;
                writeNbtString(os, s, bigEndian);
                break;
            }
            case TT_BYTE_ARRAY:
            {
                const ByteArrayT empty{};
                const auto& a = value_.ptr ? *static_cast<const ByteArrayT*>(value_.ptr) : empty;
                writeBinary<int32_t>(os, static_cast<int32_t>(a.size()), bigEndian);
                for (auto v : a) writeBinary<int8_t>(os, v, bigEndian);
                break;
            }
            case TT_INT_ARRAY:
            {
                const IntArrayT empty{};
                const auto& a = value_.ptr ? *static_cast<const IntArrayT*>(value_.ptr) : empty;
                writeBinary<int32_t>(os, static_cast<int32_t>(a.size()), bigEndian);
                for (auto v : a) writeBinary<int32_t>(os, v, bigEndian);
                break;
            }
            case TT_LONG_ARRAY:
            {
                const LongArrayT empty{};
                const auto& a = value_.ptr ? *static_cast<const LongArrayT*>(value_.ptr) : empty;
                writeBinary<int32_t>(os, static_cast<int32_t>(a.size()), bigEndian);
                for (auto v : a) writeBinary<int64_t>(os, v, bigEndian);
                break;
            }
            case TT_LIST:
            {
                // Per-element type and name headers are omitted;
                // only the element type, count, and payloads are written.
                TagType itemType = listItemType_;
                const ListT* lst = value_.ptr ? static_cast<const ListT*>(value_.ptr) : nullptr;
                int32_t n = lst ? static_cast<int32_t>(lst->size()) : 0;
                if (n == 0) itemType = TT_END;
                os.put(static_cast<char>(static_cast<uint8_t>(itemType)));
                writeBinary<int32_t>(os, n, bigEndian);
                if (lst)
                {
                    for (const auto& item : *lst)
                        item.writePayload(os, bigEndian);
                }
                break;
            }
            case TT_COMPOUND:
            {
                // Each entry is serialized as a named tag, followed by a TAG_End terminator.
                if (value_.ptr)
                {
                    const auto& cmp = *static_cast<const CompoundT*>(value_.ptr);
                    for (const auto& kv : cmp)
                        kv.second.writeNamed(os, bigEndian, kv.first);
                }
                os.put(static_cast<char>(static_cast<uint8_t>(TT_END)));
                break;
            }
            default:
                throw std::runtime_error("nbt::BasicTag: unknown tag type during dump");
        }
    }

    // ===============
    // > SNBT helpers
    // ===============

    StringT toSnbtImpl(int indent, int level) const
    {
        const StringT ind  = makeIndent(indent, level);
        const StringT ind1 = makeIndent(indent, level + 1);
        const StringT nl   = indent >= 0 ? "\n" : "";
        const StringT sp   = indent >  0 ? " "  : "";

        switch (type_)
        {
            case TT_END:
                return "";
            case TT_BYTE:
                return std::to_string(static_cast<int>(value_.i8)) + "b";
            case TT_SHORT:
                return std::to_string(value_.i16) + "s";
            case TT_INT:
                return std::to_string(value_.i32);
            case TT_LONG:
                return std::to_string(value_.i64) + "l";
            case TT_FLOAT:
                return std::to_string(value_.f32) + "f";
            case TT_DOUBLE:
                return std::to_string(value_.f64) + "d";
            case TT_STRING:
            {
                const StringT& s = value_.ptr ? *static_cast<const StringT*>(value_.ptr) : StringT{};
                return StringT("\"") + s + "\"";
            }
            case TT_BYTE_ARRAY:
            {
                const ByteArrayT* a = static_cast<const ByteArrayT*>(value_.ptr);
                if (!a || a->empty()) return "[B;]";
                StringT s = "[B;";
                for (size_t i = 0; i < a->size(); ++i)
                {
                    s += (i == 0 ? sp : "," + sp);
                    s += std::to_string(static_cast<int>((*a)[i])) + "b";
                }
                return s + "]";
            }
            case TT_INT_ARRAY:
            {
                const IntArrayT* a = static_cast<const IntArrayT*>(value_.ptr);
                if (!a || a->empty()) return "[I;]";
                StringT s = "[I;";
                for (size_t i = 0; i < a->size(); ++i)
                {
                    s += (i == 0 ? sp : "," + sp);
                    s += std::to_string((*a)[i]);
                }
                return s + "]";
            }
            case TT_LONG_ARRAY:
            {
                const LongArrayT* a = static_cast<const LongArrayT*>(value_.ptr);
                if (!a || a->empty()) return "[L;]";
                StringT s = "[L;";
                for (size_t i = 0; i < a->size(); ++i)
                {
                    s += (i == 0 ? sp : "," + sp);
                    s += std::to_string((*a)[i]) + "l";
                }
                return s + "]";
            }
            case TT_LIST:
            {
                const ListT* lst = static_cast<const ListT*>(value_.ptr);
                if (!lst || lst->empty()) return "[]";
                StringT s = "[";
                for (size_t i = 0; i < lst->size(); ++i)
                {
                    s += nl + ind1;
                    s += (*lst)[i].toSnbtImpl(indent, level + 1);
                    if (i + 1 < lst->size()) s += ",";
                }
                s += nl + ind + "]";
                return s;
            }
            case TT_COMPOUND:
            {
                const CompoundT* cmp = static_cast<const CompoundT*>(value_.ptr);
                if (!cmp || cmp->empty()) return "{}";
                StringT s = "{";
                bool first = true;
                for (const auto& kv : *cmp)
                {
                    if (!first) s += ",";
                    s += nl + ind1 + kv.first + ":" + sp;
                    s += kv.second.toSnbtImpl(indent, level + 1);
                    first = false;
                }
                s += nl + ind + "}";
                return s;
            }
            default:
                return "";
        }
    }

    // =======================
    // > SNBT parsing helpers
    // =======================

    static void snbtSkipWs(const StringT& s, size_t& pos)
    {
        while (pos < s.size())
        {
            char c = s[pos];
            if (c != ' ' && c != '\t' && c != '\n' && c != '\r') break;
            ++pos;
        }
    }

    static StringT snbtReadQuotedString(const StringT& s, size_t& pos)
    {
        ++pos; // skip opening '"'
        StringT result;
        while (pos < s.size() && s[pos] != '"')
        {
            if (s[pos] == '\\' && pos + 1 < s.size())
            {
                ++pos;
                switch (s[pos])
                {
                    case '"':  result += '"';    break;
                    case '\\': result += '\\';   break;
                    case 'n':  result += '\n';   break;
                    case 'r':  result += '\r';   break;
                    case 't':  result += '\t';   break;
                    default:   result += s[pos]; break;
                }
            }
            else { result += s[pos]; }
            ++pos;
        }
        if (pos >= s.size())
            throw std::runtime_error("nbt::BasicTag::fromSnbt(): unterminated string literal");
        ++pos; // skip closing '"'
        return result;
    }

    static StringT snbtReadKey(const StringT& s, size_t& pos)
    {
        snbtSkipWs(s, pos);
        if (pos < s.size() && s[pos] == '"')
            return snbtReadQuotedString(s, pos);
        size_t start = pos;
        while (pos < s.size() && s[pos] != ':' && s[pos] != ' ' &&
            s[pos] != '\t' && s[pos] != '\n' && s[pos] != '\r')
            ++pos;
        return s.substr(start, pos - start);
    }

    // Reads an integer token with an optional b/s/l suffix; strips the suffix before numeric conversion.
    static int64_t snbtReadRawInteger(const StringT& s, size_t& pos)
    {
        snbtSkipWs(s, pos);
        size_t start = pos;
        if (pos < s.size() && (s[pos] == '-' || s[pos] == '+')) ++pos;
        while (pos < s.size() && isdigit(static_cast<unsigned char>(s[pos]))) ++pos;
        if (pos < s.size() && (s[pos] == 'b' || s[pos] == 'B' || s[pos] == 's' || s[pos] == 'S' ||
            s[pos] == 'l' || s[pos] == 'L'))
            ++pos;
        StringT tok(s.begin() + start, s.begin() + pos);
        if (!tok.empty() && isalpha(static_cast<unsigned char>(tok.back())))
            tok.pop_back();
        return std::stoll(tok);
    }

    static BasicTagType snbtParsePrimitive(const StringT& s, size_t& pos)
    {
        size_t start = pos;
        while (pos < s.size() && s[pos] != ',' && s[pos] != ']' && s[pos] != '}')
            ++pos;
        size_t end = pos;
        while (end > start && (s[end - 1] == ' ' || s[end - 1] == '\t' ||
            s[end - 1] == '\n' || s[end - 1] == '\r'))
            --end;
        if (end <= start)
            throw std::runtime_error("nbt::BasicTag::fromSnbt(): empty primitive value");

        StringT token(s.begin() + start, s.begin() + end);
        char last = static_cast<char>(tolower(static_cast<unsigned char>(token.back())));
        StringT body = token.substr(0, token.size() - 1);

        auto isIntStr = [](const StringT& t) -> bool
        {
            if (t.empty()) return false;
            size_t i = (t[0] == '-' || t[0] == '+') ? 1 : 0;
            if (i == t.size()) return false;
            for (; i < t.size(); ++i)
                if (!isdigit(static_cast<unsigned char>(t[i]))) return false;
            return true;
        };
        auto isNumStr = [](const StringT& t) -> bool
        {
            if (t.empty()) return false;
            try { std::stod(t); return true; } catch (...) { return false; }
        };

        if (last == 'b' && isIntStr(body)) return BasicTagType(static_cast<int8_t>(std::stoi(body)));
        if (last == 's' && isIntStr(body)) return BasicTagType(static_cast<int16_t>(std::stoi(body)));
        if (last == 'l' && isIntStr(body)) return BasicTagType(static_cast<int64_t>(std::stoll(body)));
        if (last == 'f' && isNumStr(body)) return BasicTagType(std::stof(body));
        if (last == 'd' && isNumStr(body)) return BasicTagType(std::stod(body));
        if (isIntStr(token))               return BasicTagType(static_cast<int32_t>(std::stoi(token)));
        if (isNumStr(token))               return BasicTagType(std::stod(token));

        return BasicTagType(StringT(token.begin(), token.end()));
    }

    static BasicTagType snbtParseListOrArray(const StringT& s, size_t& pos)
    {
        ++pos; // skip '['
        snbtSkipWs(s, pos);

        // Typed array prefix: "B;", "I;", "L;"
        if (pos + 1 < s.size() && s[pos + 1] == ';')
        {
            char typeChar = s[pos];
            pos += 2;

            if (typeChar == 'B' || typeChar == 'b')
            {
                BasicTagType tag(TT_BYTE_ARRAY);
                auto& arr = tag.getByteArray();
                while (true)
                {
                    snbtSkipWs(s, pos);
                    if (pos >= s.size())
                        throw std::runtime_error("nbt::BasicTag::fromSnbt(): unterminated byte array");
                    if (s[pos] == ']') { ++pos; break; }
                    if (s[pos] == ',') { ++pos; continue; }
                    arr.push_back(static_cast<int8_t>(snbtReadRawInteger(s, pos)));
                }
                return tag;
            }

            if (typeChar == 'I' || typeChar == 'i')
            {
                BasicTagType tag(TT_INT_ARRAY);
                auto& arr = tag.getIntArray();
                while (true)
                {
                    snbtSkipWs(s, pos);
                    if (pos >= s.size())
                        throw std::runtime_error("nbt::BasicTag::fromSnbt(): unterminated int array");
                    if (s[pos] == ']') { ++pos; break; }
                    if (s[pos] == ',') { ++pos; continue; }
                    arr.push_back(static_cast<int32_t>(snbtReadRawInteger(s, pos)));
                }
                return tag;
            }

            if (typeChar == 'L' || typeChar == 'l')
            {
                BasicTagType tag(TT_LONG_ARRAY);
                auto& arr = tag.getLongArray();
                while (true)
                {
                    snbtSkipWs(s, pos);
                    if (pos >= s.size())
                        throw std::runtime_error("nbt::BasicTag::fromSnbt(): unterminated long array");
                    if (s[pos] == ']') { ++pos; break; }
                    if (s[pos] == ',') { ++pos; continue; }
                    arr.push_back(snbtReadRawInteger(s, pos));
                }
                return tag;
            }

            throw std::runtime_error("nbt::BasicTag::fromSnbt(): unknown array type prefix");
        }

        // Regular list
        BasicTagType tag(TT_LIST);
        while (true)
        {
            snbtSkipWs(s, pos);
            if (pos >= s.size())
                throw std::runtime_error("nbt::BasicTag::fromSnbt(): unterminated list");
            if (s[pos] == ']') { ++pos; break; }
            if (s[pos] == ',') { ++pos; continue; }
            tag.pushBack(snbtParseValue(s, pos));
        }
        return tag;
    }

    static BasicTagType snbtParseCompound(const StringT& s, size_t& pos)
    {
        ++pos; // skip '{'
        BasicTagType tag(TT_COMPOUND);
        bool first = true;
        while (true)
        {
            snbtSkipWs(s, pos);
            if (pos >= s.size())
                throw std::runtime_error("nbt::BasicTag::fromSnbt(): unterminated compound tag");
            if (s[pos] == '}') { ++pos; break; }
            if (!first)
            {
                if (s[pos] != ',')
                    throw std::runtime_error("nbt::BasicTag::fromSnbt(): expected ',' between compound entries");
                ++pos;
                snbtSkipWs(s, pos);
            }
            first = false;
            StringT key = snbtReadKey(s, pos);
            snbtSkipWs(s, pos);
            if (pos >= s.size() || s[pos] != ':')
                throw std::runtime_error("nbt::BasicTag::fromSnbt(): expected ':' after compound key");
            ++pos;
            tag.insert(key, snbtParseValue(s, pos));
        }
        return tag;
    }

    static BasicTagType snbtParseValue(const StringT& s, size_t& pos)
    {
        snbtSkipWs(s, pos);
        if (pos >= s.size())
            throw std::runtime_error("nbt::BasicTag::fromSnbt(): unexpected end of input");
        char c = s[pos];
        if (c == '{') return snbtParseCompound(s, pos);
        if (c == '[') return snbtParseListOrArray(s, pos);
        if (c == '"') return BasicTagType(snbtReadQuotedString(s, pos));
        return snbtParsePrimitive(s, pos);
    }
};

// ============
// > TagGetter
// ============

namespace detail
{

template<typename T, typename BasicTagType>
struct TagGetter
{
    static_assert(sizeof(T) == 0, "nbt::BasicTag::get<T>(): unsupported type T");
};

#define NBT_DEFINE_GETTER(T, METHOD)                            \
template<typename BasicTagType>                                 \
struct TagGetter<T, BasicTagType>                               \
{                                                               \
    static T  get(const BasicTagType& t) { return t.METHOD(); } \
    static T& getRef(BasicTagType& t)    { return t.METHOD(); } \
};

NBT_DEFINE_GETTER(int8_t,  getByte)
NBT_DEFINE_GETTER(int16_t, getShort)
NBT_DEFINE_GETTER(int32_t, getInt)
NBT_DEFINE_GETTER(int64_t, getLong)
NBT_DEFINE_GETTER(float,   getFloat)
NBT_DEFINE_GETTER(double,  getDouble)

#undef NBT_DEFINE_GETTER

template<typename BasicTagType>
struct TagGetter<typename BasicTagType::StringT, BasicTagType>
{
    static typename BasicTagType::StringT  get(const BasicTagType& t) { return t.getString(); }
    static typename BasicTagType::StringT& getRef(BasicTagType& t)    { return t.getString(); }
};

template<typename BasicTagType>
struct TagGetter<typename BasicTagType::ByteArrayT, BasicTagType>
{
    static typename BasicTagType::ByteArrayT  get(const BasicTagType& t) { return t.getByteArray(); }
    static typename BasicTagType::ByteArrayT& getRef(BasicTagType& t)    { return t.getByteArray(); }
};

template<typename BasicTagType>
struct TagGetter<typename BasicTagType::IntArrayT, BasicTagType>
{
    static typename BasicTagType::IntArrayT  get(const BasicTagType& t) { return t.getIntArray(); }
    static typename BasicTagType::IntArrayT& getRef(BasicTagType& t)    { return t.getIntArray(); }
};

template<typename BasicTagType>
struct TagGetter<typename BasicTagType::LongArrayT, BasicTagType>
{
    static typename BasicTagType::LongArrayT  get(const BasicTagType& t) { return t.getLongArray(); }
    static typename BasicTagType::LongArrayT& getRef(BasicTagType& t)    { return t.getLongArray(); }
};

template<typename BasicTagType>
struct TagGetter<typename BasicTagType::ListT, BasicTagType>
{
    static typename BasicTagType::ListT  get(const BasicTagType& t) { return t.getList(); }
    static typename BasicTagType::ListT& getRef(BasicTagType& t)    { return t.getList(); }
};

template<typename BasicTagType>
struct TagGetter<typename BasicTagType::CompoundT, BasicTagType>
{
    static typename BasicTagType::CompoundT  get(const BasicTagType& t) { return t.getCompound(); }
    static typename BasicTagType::CompoundT& getRef(BasicTagType& t)    { return t.getCompound(); }
};

} // namespace detail

// ===========================
// > Convenience type aliases
// ===========================

/** @brief Default tag: std::map-based compound, std::vector-based list. */
using Tag = BasicTag<>;

/** @brief Insertion-order-preserving variant. */
using OrderedTag = BasicTag<std::vector, OrderedMap>;

} // namespace nbt

namespace std
{

template<
    template<typename, typename...> class L,
    template<typename, typename, typename...> class C,
    template<typename> class A
>
void swap(nbt::BasicTag<L, C, A>& lhs, nbt::BasicTag<L, C, A>& rhs) noexcept
{
    lhs.swap(rhs);
}

} // namespace std

#endif // !MCNBT_MCNBT_HPP
