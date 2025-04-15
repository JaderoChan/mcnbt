#include <climits>
#include <iostream>

#include <mcnbt/mcnbt.hpp>

using namespace nbt;

// Fast way for construct tag: nbt::g.*(value, name) and nbt::gCompound(name).
// Fast way for add tag (#addTag) to list or compound: operator<<(tag).
Tag fastWayExample()
{
    auto emptyList = gList(TT_END, "EmptyList");
    auto emptyCompound = gCompound("EmptyCompound");

    // Create a compound with all the different types of values.
    auto pack = gCompound("Pack");
    pack << gByte(static_cast<char>(false), "ByteValue_Bool_False");
    pack << gByte(static_cast<char>(true), "ByteValue_Bool_True");
    pack << gShort(SHRT_MAX, "ShortValue_Max");
    pack << gShort(SHRT_MIN, "ShortValue_Min");
    pack << gInt(INT32_MAX, "IntValue_Max");
    pack << gInt(INT32_MIN, "IntValue_Min");
    pack << gLong(LONG_LONG_MAX, "LongValue_Max");
    pack << gLong(LONG_LONG_MIN, "LongValue_Min");
    pack << gFloat(3.1415926, "FloatValue_Pi");
    pack << gDouble(2.718281828459045, "DoubleValue_E");
    pack << gString("Hello, World!", "StringValue");
    pack << gByteArray({ 1, 2, 3, 4, 5 }, "ByteArrayValue");
    pack << gIntArray({ 1, 2, 3, 4, 5 }, "IntArrayValue");
    pack << gLongArray({ 1, 2, 3, 4, 5 }, "LongArrayValue");
    // Add a nested list.
    pack << (gList(TT_LIST, "ListValue")
             << emptyList.copy().setName("EmptyList1")
             << emptyList.copy().setName("EmptyList2"));   // Invalid #setName, #addTag process will delete them name.
    // Nested self.
    pack << pack.copy();

    // Nested list example.
    auto box = gList(TT_LIST, "Box");
    box << (gList(TT_COMPOUND) << pack.copy()) << (gList(TT_COMPOUND) << pack.copy() << pack.copy());

    // Create the root compound and add tags.
    auto root = gCompound("Root");
    root << emptyList << emptyCompound << pack << box;

    return root;
}

int main()
{
    try
    {
        auto root = fastWayExample();
        // Fast way for get tag (#getTag) by name or index: operator[](name or index).
        std::cout << "EmptyList Size: " << root["EmptyList"].size() << std::endl;
        std::cout << "EmptyList Element Tag Type: "
                  << getTagTypeString(root["EmptyList"].listItemType()) << std::endl;
        std::cout << "Root Size: " << root.size() << std::endl;
        std::cout << "Root[Box][0][0] Type: "
                  << getTagTypeString(root["Box"][0][0].type()) << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    catch (...)
    {
        std::cerr << "Unknown error." << std::endl;
        return 1;
    }

    return 0;
}
