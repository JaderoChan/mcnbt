#include <iostream>

#include <mcnbt/mcnbt.hpp>

using namespace nbt;

int main()
{
    // Create the nbt root compound.
    auto root = gCompound("Root");

    // Booleans example.
    auto booleans = gCompound("Booleans");
    booleans << gByte(0, "False") << gByte(1, "True");

    // Numbers example.
    auto numbers = gCompound("Numbers");
    numbers << gShort(12345, "Short") << gInt(123456789, "Int") << gLong(1234567890123, "Long");
    numbers << gFloat(3.1415926, "Float") << gDouble(2.718281828459045, "Double");

    // Strings example.
    auto strings = gCompound("Strings");
    strings << gString("Hello, world!", "String");

    // Arrays example.
    auto arrays = gCompound("Arrays");
    arrays << gByteArray({1, 2, 3, 4, 5}, "ByteArray")
           << gIntArray({1, 2, 3, 4, 5}, "IntArray")
           << gLongArray({1, 2, 3, 4, 5}, "LongArray")
           << gByteArray({}, "EmptyByteArray")
           << gIntArray({}, "EmptyIntArray")
           << gLongArray({}, "EmptyLongArray");

    // Lists example.
    auto lists = gCompound("Lists");
    auto list1 = gList(INT, "IntList");
    list1 << gInt(1) << gInt(2) << gInt(3);
    auto list2 = gList(LIST, "NestedList");
    list2 << list1.copy() << list1.copy();
    auto list3 = gList(END, "EmptyEndList");
    auto list4 = gList(BYTE, "EmptyByteList");
    lists << list1 << list2 << list3 << list4;

    // Compounds example.
    auto compounds = gCompound("Compounds");
    auto sub1 = gCompound("EmptySubCompound");
    auto sub2 = gCompound("SubCompound");
    sub2 << gString("This is a string in a subcompound", "StringInSubCompound");
    compounds << sub1 << sub2;

    // Add all the exampels to the root compound.
    root << booleans << numbers << strings << arrays << lists << compounds;

    /*
    * Write the snbt without and with indent.
    */

    // No indent.
    std::ofstream out1("snbt_example_no_indent.txt");
    if (out1.is_open()) {
        out1 << root.toSnbt(false);
        out1.close();
    }

    // With indent.
    std::ofstream out2("snbt_example_with_indent.txt");
    if (out2.is_open()) {
        out2 << root.toSnbt(true);
        out2.close();
    }
}
