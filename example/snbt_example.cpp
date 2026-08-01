#include <iostream>

#include <mcnbt/mcnbt.hpp>

using namespace nbt;

int main()
{
    auto root = Tag::compound();

    // Booleans example.
    auto booleans = Tag::compound();
    booleans["False"] = false;
    booleans["True"]  = true;

    // Numbers example.
    auto numbers = Tag::compound();
    numbers["Short"]  = int16_t(12345);
    numbers["Int"]    = int32_t(123456789);
    numbers["Long"]   = int64_t(1234567890123LL);
    numbers["Float"]  = 3.1415926f;
    numbers["Double"] = 2.718281828459045;

    // Strings example.
    auto strings = Tag::compound();
    strings["String"] = "Hello, world!";

    // Arrays example.
    auto arrays = Tag::compound();
    arrays["ByteArray"]      = Tag::ByteArrayT{int8_t(1), int8_t(2), int8_t(3), int8_t(4), int8_t(5)};
    arrays["IntArray"]       = Tag::IntArrayT{1, 2, 3, 4, 5};
    arrays["LongArray"]      = Tag::LongArrayT{1, 2, 3, 4, 5};
    arrays["EmptyByteArray"] = Tag(Tag::TT_BYTE_ARRAY);
    arrays["EmptyIntArray"]  = Tag(Tag::TT_INT_ARRAY);
    arrays["EmptyLongArray"] = Tag(Tag::TT_LONG_ARRAY);

    // Lists example.
    auto list1 = Tag::list();
    list1 << Tag(int32_t(1)) << Tag(int32_t(2)) << Tag(int32_t(3));
    auto list2 = Tag::list();
    list2 << list1 << list1;  // lvalue: copies list1 twice

    auto lists = Tag::compound();
    lists["IntList"]       = std::move(list1);
    lists["NestedList"]    = std::move(list2);
    lists["EmptyEndList"]  = Tag::list();
    lists["EmptyByteList"] = Tag::list();

    // Compounds example.
    auto sub2 = Tag::compound();
    sub2["StringInSubCompound"] = "This is a string in a subcompound";
    auto compounds = Tag::compound();
    compounds["EmptySubCompound"] = Tag::compound();
    compounds["SubCompound"]      = std::move(sub2);

    root["Booleans"]  = std::move(booleans);
    root["Numbers"]   = std::move(numbers);
    root["Strings"]   = std::move(strings);
    root["Arrays"]    = std::move(arrays);
    root["Lists"]     = std::move(lists);
    root["Compounds"] = std::move(compounds);

    auto snbt = root.toSnbt();
    // No indent.
    std::ofstream out1("./snbt_example_no_indent.txt");
    if (out1.is_open())
    {
        out1 << root.toSnbt();
        out1.close();
    }

    // With indent.
    std::ofstream out2("./snbt_example_with_indent.txt");
    if (out2.is_open())
    {
        out2 << root.toSnbt(4);
        out2.close();
    }

    return 0;
}
