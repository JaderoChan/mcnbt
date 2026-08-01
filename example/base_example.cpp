#include <iostream>

#include <mcnbt/mcnbt.hpp>

using namespace nbt;

static const char* typeStr(Tag::TagType t)
{
    switch (t)
    {
        case Tag::TT_END:        return "TAG_End";
        case Tag::TT_BYTE:       return "TAG_Byte";
        case Tag::TT_SHORT:      return "TAG_Short";
        case Tag::TT_INT:        return "TAG_Int";
        case Tag::TT_LONG:       return "TAG_Long";
        case Tag::TT_FLOAT:      return "TAG_Float";
        case Tag::TT_DOUBLE:     return "TAG_Double";
        case Tag::TT_STRING:     return "TAG_String";
        case Tag::TT_BYTE_ARRAY: return "TAG_Byte_Array";
        case Tag::TT_LIST:       return "TAG_List";
        case Tag::TT_COMPOUND:   return "TAG_Compound";
        case Tag::TT_INT_ARRAY:  return "TAG_Int_Array";
        case Tag::TT_LONG_ARRAY: return "TAG_Long_Array";
        default:                 return "TAG_Unknown";
    }
}

void numExample()
{
    Tag byteNum(Tag::TT_BYTE);
    Tag shortNum(Tag::TT_SHORT);
    Tag intNum(Tag::TT_INT);
    Tag longNum(Tag::TT_LONG);
    Tag floatNum(Tag::TT_FLOAT);
    Tag doubleNum(Tag::TT_DOUBLE);

    // Test set value via mutable reference.
    std::cout << "--Test set value--" << std::endl;
    std::cout << "byteNum value before set: " << static_cast<int>(byteNum.getByte()) << std::endl;
    byteNum.getByte() = 127;
    std::cout << "byteNum value after set 127: " << static_cast<int>(byteNum.getByte()) << std::endl;
    std::cout << '\n';

    // Test get value.
    std::cout << "--Test get value--" << std::endl;
    std::cout << "getShort() with default value: " << shortNum.getShort() << std::endl;
    std::cout << '\n';

    // Test set value by reconstruction and mutable reference.
    std::cout << "--Test set value by reconstruction--" << std::endl;
    std::cout << "intNum value before set: " << intNum.getInt() << std::endl;
    intNum = Tag(int32_t(1));
    intNum.getInt() = 2;
    std::cout << "intNum value after set 1, then 2: " << intNum.getInt() << std::endl;
    std::cout << '\n';

    // Test type check.
    std::cout << "--Test check type--" << std::endl;
    std::cout << "floatNum type: " << typeStr(floatNum.type()) << std::endl;
    std::cout << "floatNum.isFloatPoint(): " << (floatNum.isFloatPoint() ? "true" : "false") << std::endl;
    std::cout << '\n';

    // Test explicit cast.
    std::cout << "--Test explicit cast--" << std::endl;
    doubleNum.getDouble() = 2.718281828459045;
    double d = static_cast<double>(doubleNum);
    std::cout << "static_cast<double>(doubleNum): " << d << std::endl;
    std::cout << '\n';
}

void stringExample()
{
    Tag str(Tag::TT_STRING);

    // Test set and get value via mutable reference.
    std::cout << "--Test set and get value--" << std::endl;
    std::cout << "str value before set: " << str.getString() << std::endl;
    str.getString() = "Hello, World!";
    std::cout << "str value after set: " << str.getString() << std::endl;
    std::cout << '\n';

    // Test get size.
    std::cout << "--Test get size--" << std::endl;
    std::cout << "str size: " << str.size() << std::endl;
    std::cout << '\n';

    // Test clear value.
    std::cout << "--Test clear value--" << std::endl;
    std::cout << "str value before clear: " << str.getString() << std::endl;
    str.clear();
    std::cout << "str value after clear: " << str.getString() << std::endl;
    std::cout << '\n';
}

void arrayExample()
{
    Tag byteArr(Tag::TT_BYTE_ARRAY);
    Tag intArr(Tag::TT_INT_ARRAY);
    Tag longArr(Tag::TT_LONG_ARRAY);

    // Test error handling when erasing from empty array.
    std::cout << "--Test error handling--" << std::endl;
    try
    {
        byteArr.erase(0);
    }
    catch (std::exception& e)
    {
        std::cout << "Error, erase(0) on empty byteArr: " << e.what() << std::endl;
    }
    std::cout << '\n';

    // Test set and get value via mutable reference.
    std::cout << "--Test set and get value--" << std::endl;
    std::cout << "byteArr value before set: ";
    std::cout << byteArr.toSnbt() << std::endl;
    byteArr.getByteArray() = {int8_t(1), int8_t(2), int8_t(3), int8_t(4), int8_t(5)};
    std::cout << "byteArr value after set {1, 2, 3, 4, 5}: ";
    std::cout << byteArr.toSnbt() << std::endl;
    std::cout << '\n';

    // Test get size.
    std::cout << "--Test get size--" << std::endl;
    intArr.getIntArray() = {-1, -2, -3, -4, -5};
    std::cout << "intArr value: ";
    std::cout << intArr.toSnbt() << std::endl;
    std::cout << "intArr size: " << intArr.size() << std::endl;
    std::cout << '\n';

    // Test get item by index.
    std::cout << "--Test get item by index--" << std::endl;
    std::cout << "intArr value: ";
    std::cout << intArr.toSnbt() << std::endl;
    std::cout << "intArr item at index 2: " << intArr.getIntArray()[2] << std::endl;
    std::cout << '\n';

    // Test add item.
    std::cout << "--Test add item--" << std::endl;
    std::cout << "intArr value before add 100: ";
    std::cout << intArr.toSnbt() << std::endl;
    intArr.getIntArray().push_back(100);
    std::cout << "intArr value after add 100: ";
    std::cout << intArr.toSnbt() << std::endl;
    std::cout << '\n';

    // Test clear all items.
    std::cout << "--Test clear all items--" << std::endl;
    std::cout << "intArr value before clear: ";
    std::cout << intArr.toSnbt() << std::endl;
    intArr.clear();
    std::cout << "intArr value after clear: ";
    std::cout << intArr.toSnbt() << std::endl;
    std::cout << '\n';

    // Test erase item by index.
    std::cout << "--Test erase item by index--" << std::endl;
    longArr.getLongArray() = {100000000LL, 20000000LL, 30000000LL, 40000000LL, 50000000LL};
    std::cout << "longArr value before erase: ";
    std::cout << longArr.toSnbt() << std::endl;
    longArr.erase(2);
    std::cout << "longArr value after erase index 2: ";
    std::cout << longArr.toSnbt() << std::endl;
    std::cout << '\n';

    // Test error handling when erasing out of range.
    std::cout << "--Test error handling--" << std::endl;
    std::cout << "longArr value: ";
    std::cout << longArr.toSnbt() << std::endl;
    try
    {
        longArr.erase(10);
    }
    catch (std::exception& e)
    {
        std::cout << "Error, erase out of range (longArr.erase(10)): " << e.what() << std::endl;
    }
    std::cout << '\n';

    // Test get front and back item.
    std::cout << "--Test get front and back item--" << std::endl;
    std::cout << "longArr front item: " << longArr.getLongArray().front() << std::endl;
    std::cout << "longArr back item: "  << longArr.getLongArray().back()  << std::endl;
    std::cout << '\n';
}

void listExample()
{
    Tag lst(Tag::TT_LIST);

    // List item type is inferred from the first element pushed.
    std::cout << "--Test list item type inference--" << std::endl;
    std::cout << "lst item type before push: " << typeStr(lst.listItemType()) << std::endl;
    lst << Tag("Hello");
    std::cout << "lst item type after pushing a string: " << typeStr(lst.listItemType()) << std::endl;
    std::cout << '\n';

    // Test add and get item.
    std::cout << "--Test add and get item--" << std::endl;
    std::cout << "lst value before add more strings: ";
    std::cout << lst.toSnbt() << std::endl;
    lst << Tag("World") << Tag("!!!");
    std::cout << "lst value after add ('World', '!!!'): ";
    std::cout << lst.toSnbt() << std::endl;
    std::cout << '\n';

    // Test error handling on wrong type access.
    std::cout << "--Test error handling--" << std::endl;
    try
    {
        lst.front().getInt();
    }
    catch (std::exception& e)
    {
        std::cout << "Error, wrong type (lst.front().getInt() on string): " << e.what() << std::endl;
    }
    std::cout << '\n';

    // Test get size.
    std::cout << "--Test get size--" << std::endl;
    std::cout << "lst size: " << lst.size() << std::endl;
    std::cout << '\n';

    // Test add item with << operator.
    std::cout << "--Test add item with << operator--" << std::endl;
    std::cout << "lst value before add strings: ";
    std::cout << lst.toSnbt() << std::endl;
    lst << Tag("  ") << Tag("Bye") << Tag("...");
    std::cout << "lst value after add ('  ', 'Bye', '...') with << operator: ";
    std::cout << lst.toSnbt() << std::endl;
    std::cout << '\n';

    // Test get front and back item.
    std::cout << "--Test get front and back item--" << std::endl;
    std::cout << "lst front item: " << lst.front().getString() << std::endl;
    std::cout << "lst back item: "  << lst.back().getString()  << std::endl;
    std::cout << '\n';

    // Test get item by index.
    std::cout << "--Test get item by index--" << std::endl;
    std::cout << "lst item at index 2: " << lst[2].getString() << std::endl;
    std::cout << '\n';

    // Test erase item by index.
    std::cout << "--Test erase item by index--" << std::endl;
    std::cout << "lst value before erase item at index 2: ";
    std::cout << lst.toSnbt() << std::endl;
    lst.erase(2);
    std::cout << "lst value after erase item at index 2: ";
    std::cout << lst.toSnbt() << std::endl;
    std::cout << '\n';

    // Test copy list.
    std::cout << "--Test copy list--" << std::endl;
    Tag lst2 = lst;
    std::cout << "lst value: ";
    std::cout << lst.toSnbt() << std::endl;
    std::cout << "lst2 value: ";
    std::cout << lst2.toSnbt() << std::endl;
    std::cout << '\n';

    // Test clear all items.
    std::cout << "--Test clear all items--" << std::endl;
    std::cout << "lst value before clear: ";
    std::cout << lst.toSnbt() << std::endl;
    lst.clear();
    std::cout << "lst value after clear (listItemType reset to TT_END): ";
    std::cout << lst.toSnbt() << std::endl;
    std::cout << '\n';

    // Test nested list.
    std::cout << "--Test nested list--" << std::endl;
    std::cout << "lst value before add list: ";
    std::cout << lst.toSnbt() << std::endl;
    Tag lst1 = Tag::list();
    lst1 << Tag(int32_t(1)) << Tag(int32_t(2)) << Tag(int32_t(3));
    std::cout << "lst1 value: ";
    std::cout << lst1.toSnbt() << std::endl;
    lst << std::move(lst1);  // moves lst1 (lst1 becomes TT_END)
    lst << lst2;             // copies lst2 (lst2 remains valid)
    std::cout << "lst value after add (lst1 moved, lst2 copied): ";
    std::cout << lst.toSnbt() << std::endl;
    std::cout << "lst1 type after move: " << typeStr(lst1.type()) << std::endl;
    std::cout << "lst2 value: ";
    std::cout << lst2.toSnbt() << std::endl;
    std::cout << '\n';
}

void compoundExample()
{
    Tag root = Tag::compound();

    root["max byte"]   = int8_t(127);
    root["max short"]  = int16_t(32767);
    root["max int"]    = int32_t(2147483647);
    root["max long"]   = int64_t(9223372036854775807LL);
    root["pi"]         = 3.14159f;
    root["e"]          = 2.718281828459045;
    root["greeting"]   = "Hello, World!";
    root["byte array"] = Tag::ByteArrayT{int8_t(1), int8_t(2), int8_t(3), int8_t(4), int8_t(5)};

    // Nested list of lists.
    auto inner1 = Tag::list();
    inner1 << Tag(int32_t(1)) << Tag(int32_t(2)) << Tag(int32_t(3));

    auto inner2 = Tag::list();
    inner2 << Tag("NiHao") << Tag("ShiJie!");

    auto tmpLst = Tag::list();
    tmpLst << Tag(1.1) << Tag(2.2) << Tag(3.3);

    auto inner3 = Tag::list();
    inner3 << tmpLst << std::move(tmpLst);  // first copies, second moves

    auto lst = Tag::list();
    lst << std::move(inner1) << std::move(inner2) << std::move(inner3);

    root["nested list"] = std::move(lst);

    std::cout << "--Test get tag by key--" << std::endl;
    std::cout << "root value: ";
    std::cout << root.toSnbt(4) << std::endl;
    std::cout << "max byte value: "  << static_cast<int>(root["max byte"].getByte())   << std::endl;
    std::cout << "max short value: " << root["max short"].getShort()                    << std::endl;
    std::cout << "max int value: "   << root["max int"].getInt()                        << std::endl;
    std::cout << "max long value: "  << root["max long"].getLong()                      << std::endl;
    std::cout << "pi value: "        << root["pi"].getFloat()                           << std::endl;
    std::cout << "e value: "         << root["e"].getDouble()                           << std::endl;
    std::cout << "greeting value: "  << root["greeting"].getString()                    << std::endl;
    std::cout << "byte array value: ";
    std::cout << root["byte array"].toSnbt() << std::endl;
    std::cout << "nested list value: ";
    std::cout << root["nested list"].toSnbt(4) << std::endl;
    std::cout << '\n';
}

int main()
{
    const std::string separator(60, '-');

    std::cout << "Num example" << std::endl;
    std::cout << separator << std::endl;
    numExample();

    std::cout << "String example" << std::endl;
    std::cout << separator << std::endl;
    stringExample();

    std::cout << "Array example" << std::endl;
    std::cout << separator << std::endl;
    arrayExample();

    std::cout << "List example" << std::endl;
    std::cout << separator << std::endl;
    listExample();

    std::cout << "Compound example" << std::endl;
    std::cout << separator << std::endl;
    compoundExample();

    return 0;
}
