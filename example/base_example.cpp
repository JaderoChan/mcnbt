#include <iostream>

#include <mcnbt/mcnbt.hpp>

using namespace nbt;

void numExample()
{
    Tag byteNum(TT_BYTE);
    Tag shortNum(TT_SHORT);
    Tag intNum(TT_INT);
    Tag longNum(TT_LONG);
    Tag floatNum(TT_FLOAT);
    Tag doubleNum(TT_DOUBLE);

    // Test set value.
    std::cout << "--Test set value--" << std::endl;
    std::cout << "#byteNum value before set: " << (int) byteNum.getByte() << std::endl;
    byteNum.setByte(127);
    std::cout << "#byteNum value after set 127: " << (int) byteNum.getByte() << std::endl;
    std::cout << '\n';

    // Test get value.
    std::cout << "--Test get value--" << std::endl;
    std::cout << "getShort() with default value: " << shortNum.getShort() << std::endl;
    std::cout << '\n';

    // Test continuous set value.
    std::cout << "--Test continuous set value--" << std::endl;
    std::cout << "#intNum value before continuous set: " << intNum.getInt() << std::endl;
    intNum.setInt(1).setInt(2);
    std::cout << "#intNum value after continuous set 1, 2: " << intNum.getInt() << std::endl;
    std::cout << '\n';

    // Test type check.
    std::cout << "--Test check type--" << std::endl;
    std::cout << "#floatNum value: " << floatNum.getFloat() << std::endl;
    bool isFloatPointNum = floatNum.isFloatPoint();
    std::cout << "#floatNum.isFloatPoint(): " << (isFloatPointNum ? "true" : "false") << std::endl;
    std::cout << '\n';

    // Test get and set name.
    std::cout << "--Test get and set name--" << std::endl;
    std::cout << "#doubleNum name before set: " << doubleNum.name() << std::endl;
    doubleNum.setName("double num");
    std::cout << "#doubleNum name after set: " << doubleNum.name() << std::endl;
    std::cout << '\n';
}

void stringExample()
{
    Tag str(TT_STRING);

    // Test set and get value.
    std::cout << "--Test set and get value--" << std::endl;
    std::cout << "#str value before set: " << str.getString() << std::endl;
    str.setString("Hello, World!");
    std::cout << "#str value after set: " << str.getString() << std::endl;
    std::cout << '\n';

    // Test set and get name.
    std::cout << "--Test set and get name--" << std::endl;
    std::cout << "#str name before set: " << str.name() << std::endl;
    str.setName("string");
    std::cout << "#str name after set: " << str.name() << std::endl;
    std::cout << '\n';

    // Test get the name length, string length.
    std::cout << "--Test get name length, string length and size--" << std::endl;
    std::cout << "#str name length: " << str.nameLength() << std::endl;
    std::cout << "#str size: " << str.size() << std::endl;
    std::cout << '\n';

    // Test clear value.
    std::cout << "--Test clear value--" << std::endl;
    std::cout << "#str value before clear: " << str.getString() << std::endl;
    str.removeAll();
    std::cout << "#str value after clear: " << str.getString() << std::endl;
    std::cout << '\n';
}

void arrayExample()
{
    Tag byteArr(TT_BYTE_ARRAY);
    Tag intArr(TT_INT_ARRAY);
    Tag longArr(TT_LONG_ARRAY);

    // Test error handling when remove front element from empty array.
    std::cout << "--Test error handling--" << std::endl;
    try {
        byteArr.removeFront();
    } catch (std::exception& e) {
        std::cout << "Error, remove front from #byteArr that is empty: " << e.what() << std::endl;
    }
    std::cout << '\n';

    // Test set and get value.
    std::cout << "--Test set and get value--" << std::endl;
    std::cout << "#byteArr value before set: ";
    std::cout << byteArr.toSnbt() << std::endl;
    std::cout << "#byteArr value after set {1, 2, 3, 4, 5}: ";
    byteArr.setByteArray({ 1, 2, 3, 4, 5 });
    std::cout << byteArr.toSnbt() << std::endl;
    std::cout << '\n';

    // Test get size.
    std::cout << "--Test get size--" << std::endl;
    intArr.setIntArray({ -1, -2, -3, -4, -5 });
    std::cout << "#intArr value: ";
    std::cout << intArr.toSnbt() << std::endl;
    std::cout << "#intArr size: " << intArr.size() << std::endl;
    std::cout << '\n';

    // Test get element by index.
    std::cout << "--Test get element by index--" << std::endl;
    std::cout << "#intArr value: ";
    std::cout << intArr.toSnbt() << std::endl;
    std::cout << "#intArr element at index 2: " << intArr.getInt(2) << std::endl;
    std::cout << '\n';

    // Test add element.
    std::cout << "--Test add element--" << std::endl;
    std::cout << "#intArr value before add 100: ";
    std::cout << intArr.toSnbt() << std::endl;
    intArr.addInt(100);
    std::cout << "#intArr value after add 100: ";
    std::cout << intArr.toSnbt() << std::endl;
    std::cout << '\n';

    // Test remove element.
    std::cout << "--Test remove all elements--" << std::endl;
    std::cout << "#intArr value before remove all: ";
    std::cout << intArr.toSnbt() << std::endl;
    intArr.removeAll();
    std::cout << "#intArr value after remove all: ";
    std::cout << intArr.toSnbt() << std::endl;
    std::cout << '\n';

    // Test remove element by index.
    std::cout << "--Test remove element by index--" << std::endl;
    longArr.setLongArray({ 100000000, 20000000, 30000000, 40000000, 50000000 });
    std::cout << "#longArr value before remove: ";
    std::cout << longArr.toSnbt() << std::endl;
    std::cout << "#longArr value after remove 3rd element: ";
    longArr.remove(2);
    std::cout << longArr.toSnbt() << std::endl;
    std::cout << '\n';

    // Test error handling when remove element out of range.
    std::cout << "--Test error handling--" << std::endl;
    std::cout << "#longArr value: ";
    std::cout << longArr.toSnbt() << std::endl;
    try {
        longArr.remove(10);
    } catch (std::exception& e) {
        std::cout << "Error, remove element out of range (#longArr.removeLong(10)): " << e.what() << std::endl;
    }
    std::cout << '\n';

    // Test get front and back element.
    std::cout << "--Test get front and back element--" << std::endl;
    std::cout << "#longArr front element: " << longArr.getFrontLong() << std::endl;
    std::cout << "#longArr back element: " << longArr.getBackLong() << std::endl;
    std::cout << '\n';
}

void listExample()
{
    Tag lst(TT_LIST);

    // Test init list element type.
    std::cout << "--Test init list element type--" << std::endl;
    std::cout << "#lst element type before init: " << getTagTypeString(lst.listElementType()) << std::endl;
    lst.initListElementType(TT_STRING);
    std::cout << "#lst element type after init: " << getTagTypeString(lst.listElementType()) << std::endl;
    std::cout << '\n';

    // Test error handling when init list element type twice.
    std::cout << "--Test error handling--" << std::endl;
    try {
        lst.initListElementType(TT_INT);
    } catch (std::exception& e) {
        std::cout << "Error, init list element type twice (#lst.initListElementType(TT_INT)): "
                  << e.what() << std::endl;
    }
    std::cout << '\n';

    // Test add and get element.
    std::cout << "--Test add and get element--" << std::endl;
    std::cout << "#lst value before add strings: ";
    std::cout << lst.toSnbt() << std::endl;
    std::cout << "#lst value after add strings ('Hello', 'World', '!!!'): ";
    Tag str1 = Tag(TT_STRING).setString("Hello");
    Tag str2 = Tag(TT_STRING).setString("World");
    Tag str3 = Tag(TT_STRING).setString("!!!");
    lst.addTag(str1).addTag(str2).addTag(str3);
    std::cout << lst.toSnbt() << std::endl;
    std::cout << '\n';

    // Test error handling when set name to list element.
    std::cout << "--Test error handling--" << std::endl;
    try {
        lst[0].setName("Hello");
    } catch (std::exception& e) {
        std::cout << "Error, set name for list element: " << e.what() << std::endl;
    }
    std::cout << '\n';

    // Test get size.
    std::cout << "--Test get size--" << std::endl;
    std::cout << "#lst size: " << lst.size() << std::endl;
    std::cout << '\n';

    // Test add element with << operator.
    std::cout << "--Test add element with << operator--" << std::endl;
    std::cout << "#lst value before add strings: ";
    std::cout << lst.toSnbt() << std::endl;
    std::cout << "#lst value after add strings ('  ', 'Bye', '...') with << operator: ";
    Tag str4 = Tag(TT_STRING).setString("  ");
    Tag str5 = Tag(TT_STRING).setString("Bye");
    Tag str6 = Tag(TT_STRING).setString("...");
    lst << str4 << str5 << str6;
    std::cout << lst.toSnbt() << std::endl;
    std::cout << '\n';

    // Test get front and back element.
    std::cout << "--Test get front and back element--" << std::endl;
    std::cout << "#lst front element: " << lst.getFrontTag().getString() << std::endl;
    std::cout << "#lst back element: " << lst.getBackTag().getString() << std::endl;
    std::cout << '\n';

    // Test get element by index.
    std::cout << "--Test get element by index--" << std::endl;
    std::cout << "#lst element at index 2: " << lst.getTag(2).getString() << std::endl;
    std::cout << '\n';

    // Test remove element by index.
    std::cout << "--Test remove element by index--" << std::endl;
    std::cout << "#lst value before remove element at index 2: ";
    std::cout << lst.toSnbt() << std::endl;
    std::cout << "#lst value after remove element at index 2: ";
    lst.remove(2);
    std::cout << lst.toSnbt() << std::endl;
    std::cout << '\n';

    // Test copy list.
    std::cout << "--Test copy list--" << std::endl;
    Tag lst2 = lst;
    std::cout << "#lst value: ";
    std::cout << lst.toSnbt() << std::endl;
    std::cout << "#lst2 value: ";
    std::cout << lst.toSnbt() << std::endl;
    std::cout << '\n';

    // Test remove all elements.
    std::cout << "--Test remove all elements--" << std::endl;
    std::cout << "#lst value before remove all: ";
    std::cout << lst.toSnbt() << std::endl;
    std::cout << "#lst value after remove all: ";
    lst.removeAll();
    std::cout << lst.toSnbt() << std::endl;
    std::cout << '\n';

    // Test reset list element type.
    std::cout << "--Test reset list element type--" << std::endl;
    std::cout << "#lst element type before reset: " << getTagTypeString(lst.listElementType()) << std::endl;
    lst.resetList();
    std::cout << "#lst element type after reset: " << getTagTypeString(lst.listElementType()) << std::endl;
    std::cout << '\n';

    // Test init list element type again.
    std::cout << "--Test init list element type again--" << std::endl;
    std::cout << "#lst element type before init again: " << getTagTypeString(lst.listElementType()) << std::endl;
    lst.initListElementType(TT_LIST);
    std::cout << "#lst element type after init again: " << getTagTypeString(lst.listElementType()) << std::endl;
    std::cout << '\n';

    // Test nested add list.
    std::cout << "--Test nested add list--" << std::endl;
    std::cout << "#lst value before add list: ";
    std::cout << lst.toSnbt() << std::endl;
    std::cout << "#lst1 value: ";
    Tag lst1 = Tag(TT_LIST).initListElementType(TT_INT);
    lst1 << Tag(TT_INT).setInt(1) << Tag(TT_INT).setInt(2) << Tag(TT_INT).setInt(3);
    std::cout << lst1.toSnbt() << std::endl;
    lst.addTag(lst1);           // default move constructor. (lst1 is invalid after this operation)
    lst.addTag(lst2.copy());    // copy constructor. (lst2 is still valid after this operation)
    std::cout << "#lst value after add list (#lst1, #lst2): ";
    std::cout << lst.toSnbt() << std::endl;
    std::cout << "#lst1 value: ";
    std::cout << lst1.toSnbt() << std::endl;
    std::cout << "#lst2 value: ";
    std::cout << lst2.toSnbt() << std::endl;
    std::cout << '\n';
}

void compoundExample()
{
    Tag root(TT_COMPOUND);

    // Add some tags to root.
    root.addTag(Tag(TT_BYTE).setByte(127).setName("max byte"));
    root.addTag(Tag(TT_SHORT).setShort(32767).setName("max short"));
    root.addTag(Tag(TT_INT).setInt(2147483647).setName("max int"));
    root.addTag(Tag(TT_LONG).setLong(9223372036854775807).setName("max long"));
    root.addTag(Tag(TT_FLOAT).setFloat(3.14159f).setName("pi"));
    root.addTag(Tag(TT_DOUBLE).setDouble(2.718281828459045).setName("e"));
    root.addTag(Tag(TT_STRING).setString("Hello, World!").setName("greeting"));
    root.addTag(Tag(TT_BYTE_ARRAY).setByteArray({ 1, 2, 3, 4, 5 }).setName("byte array"));

    // Add some list to list for test nested list.
    Tag lst = Tag(TT_LIST).initListElementType(TT_LIST);
    lst << (Tag(TT_LIST).initListElementType(TT_INT)
            << Tag(TT_INT).setInt(1)
            << Tag(TT_INT).setInt(2)
            << Tag(TT_INT).setInt(3));
    lst << (Tag(TT_LIST).initListElementType(TT_STRING)
            << Tag(TT_STRING).setString("NiHao")
            << Tag(TT_STRING).setString("ShiJie!"));
    Tag tmpLst = Tag(TT_LIST).initListElementType(TT_DOUBLE);
    tmpLst << Tag(TT_DOUBLE).setDouble(1.1) << Tag(TT_DOUBLE).setDouble(2.2) << Tag(TT_DOUBLE).setDouble(3.3);
    lst << (Tag(TT_LIST).initListElementType(TT_LIST) << tmpLst.copy() << tmpLst);

    root << lst;

    std::cout << "--Test get tag by name--" << std::endl;
    std::cout << "#root value: ";
    std::cout << root.toSnbt() << std::endl;
    std::cout << "#max byte value: " << root.getFrontTag().getByte() << std::endl;
    std::cout << "#max short value: " << root.getTag("max short").getShort() << std::endl;
    std::cout << "#max int value: " << root["max int"].getInt() << std::endl;
    std::cout << "#max long value: " << root[3].getLong() << std::endl;
    std::cout << "#pi value: " << root.getTag("pi").getFloat() << std::endl;
    std::cout << "#e value: " << root.getTag("e").getDouble() << std::endl;
    std::cout << "#greeting value: " << root.getTag("greeting").getString() << std::endl;
    std::cout << "#byte array value: ";
    std::cout << root["byte array"].toSnbt() << std::endl;
    std::cout << "#nested list value: ";
    std::cout << root.getBackTag().toSnbt() << std::endl;
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
