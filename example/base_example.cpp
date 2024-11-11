#include <iostream>

#include <mcnbt/mcnbt.hpp>

using namespace nbt;

// Forwards declarations of print functions.
void printArray(const Tag& array);
void printList(const Tag& list);
void printCompound(const Tag& compound);

// You can ignore details of the following three print functions. They are used to easy print the example data.

// Simple print function for array.
void printArray(const Tag& arr)
{
    // If array is empty, print "[]" and return.
    if (arr.empty()) {
        std::cout << "[]" << std::endl;
        return;
    }

    std::cout << "[";

    // Print all elements of the corresponding array.
    if (arr.isByteArray()) {
        for (const auto& var : *arr.getByteArray())
            std::cout << (int) var << ", ";
    } else if (arr.isIntArray()) {
        for (const auto& var : *arr.getIntArray())
            std::cout << var << ", ";
    } else if (arr.isLongArray()) {
        for (const auto& var : *arr.getLongArray())
            std::cout << var << ", ";
    }

    std::cout << "]" << std::endl;
}

// Simple print function for list.
void printList(const Tag& lst)
{
    // If list is empty, print "[]" and return.
    if (lst.empty()) {
        std::cout << "[]" << std::endl;
        return;
    }

    std::cout << "[";

    // Print all elements.
    for (const auto& var : *lst.getTags()) {
        if (var.isByte())
            std::cout << (int) var.getByte() << ", ";
        else if (var.isShort())
            std::cout << var.getShort() << ", ";
        else if (var.isInt())
            std::cout << var.getInt() << ", ";
        else if (var.isLong())
            std::cout << var.getLong() << ", ";
        else if (var.isFloat())
            std::cout << var.getFloat() << ", ";
        else if (var.isDouble())
            std::cout << var.getDouble() << ", ";
        else if (var.isString())
            std::cout << var.getString() << ", ";
        else if (var.isArray()) {
            printArray(var);
            std::cout << ", ";
        } else if (var.isList()) {
            printList(var);
            std::cout << ", ";
        } else if (var.isCompound()) {
            printCompound(var);
            std::cout << ", ";
        }
    }

    std::cout << "]" << std::endl;
}

// Simple print function for compound.
void printCompound(const Tag& cmp)
{
    // If compound is empty, print "{}" and return.
    if (cmp.empty()) {
        std::cout << "{}" << std::endl;
        return;
    }

    std::cout << "{";

    // Print all elements.
    for (const auto& var : *cmp.getTags()) {
        std::cout << var.name() << ": ";
        if (var.isByte())
            std::cout << (int) var.getByte() << ", ";
        else if (var.isShort())
            std::cout << var.getShort() << ", ";
        else if (var.isInt())
            std::cout << var.getInt() << ", ";
        else if (var.isLong())
            std::cout << var.getLong() << ", ";
        else if (var.isFloat())
            std::cout << var.getFloat() << ", ";
        else if (var.isDouble())
            std::cout << var.getDouble() << ", ";
        else if (var.isString())
            std::cout << var.getString() << ", ";
        else if (var.isArray()) {
            printArray(var);
            std::cout << ", ";
        } else if (var.isList()) {
            printList(var);
            std::cout << ", ";
        } else if (var.isCompound()) {
            printCompound(var);
            std::cout << ", ";
        }
    }

    std::cout << "}" << std::endl;
}

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

    // Test error handling when set value to wrong type.
    std::cout << "--Test error handling--" << std::endl;
    try {
        longNum.setFloat(1.0f);
    } catch (std::exception& e) {
        std::cout << "Error, set float to long tag (#longNum.setFloat(1.0f)): " << e.what() << std::endl;
    }
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

    // Test get the name length, string length and size (equal to string length).
    std::cout << "--Test get name length, string length and size--" << std::endl;
    std::cout << "#str name length: " << str.nameLen() << std::endl;
    std::cout << "#str string length: " << str.stringLen() << std::endl;
    std::cout << "#str size: " << str.size() << std::endl;
    std::cout << '\n';

    // Test clear value.
    std::cout << "--Test clear value--" << std::endl;
    std::cout << "#str value before clear: " << str.getString() << std::endl;
    str.clear();
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
    printArray(byteArr);
    std::cout << "#byteArr value after set {1, 2, 3, 4, 5}: ";
    byteArr.setByteArray({ 1, 2, 3, 4, 5 });
    printArray(byteArr);
    std::cout << '\n';

    // Test get size.
    std::cout << "--Test get size--" << std::endl;
    intArr.setIntArray({ -1, -2, -3, -4, -5 });
    std::cout << "#intArr value: ";
    printArray(intArr);
    std::cout << "#intArr size: " << intArr.size() << std::endl;
    std::cout << '\n';

    // Test get element by index.
    std::cout << "--Test get element by index--" << std::endl;
    std::cout << "#intArr value: ";
    printArray(intArr);
    std::cout << "#intArr element at index 2: " << intArr.getInt(2) << std::endl;
    std::cout << '\n';

    // Test add element.
    std::cout << "--Test add element--" << std::endl;
    std::cout << "#intArr value before add 100: ";
    printArray(intArr);
    intArr.addInt(100);
    std::cout << "#intArr value after add 100: ";
    printArray(intArr);
    std::cout << '\n';

    // Test remove element.
    std::cout << "--Test remove all elements--" << std::endl;
    std::cout << "#intArr value before remove all: ";
    printArray(intArr);
    intArr.clear();
    std::cout << "#intArr value after remove all: ";
    printArray(intArr);
    std::cout << '\n';

    // Test remove element by index.
    std::cout << "--Test remove element by index--" << std::endl;
    longArr.setLongArray({ 100000000, 20000000, 30000000, 40000000, 50000000 });
    std::cout << "#longArr value before remove: ";
    printArray(longArr);
    std::cout << "#longArr value after remove 3rd element: ";
    longArr.remove(2);
    printArray(longArr);
    std::cout << '\n';

    // Test error handling when remove element out of range.
    std::cout << "--Test error handling--" << std::endl;
    std::cout << "#longArr value: ";
    printArray(longArr);
    try {
        longArr.remove(10);
    } catch (std::exception& e) {
        std::cout << "Error, remove element out of range (#longArr.removeLong(10)): " << e.what() << std::endl;
    }
    std::cout << '\n';

    // Test get front and back element.
    std::cout << "--Test get front and back element--" << std::endl;
    std::cout << "#longArr front element: " << longArr.frontLong() << std::endl;
    std::cout << "#longArr back element: " << longArr.backLong() << std::endl;
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
        std::cout << "Error, init list element type twice (#lst.initListElementType(TT_INT)): " <<
            e.what() << std::endl;
    }
    std::cout << '\n';

    // Test add and get element.
    std::cout << "--Test add and get element--" << std::endl;
    std::cout << "#lst value before add strings: ";
    printList(lst);
    std::cout << "#lst value after add strings ('Hello', 'World', '!!!'): ";
    Tag str1 = Tag(TT_STRING).setString("Hello");
    Tag str2 = Tag(TT_STRING).setString("World");
    Tag str3 = Tag(TT_STRING).setString("!!!");
    lst.addTag(str1).addTag(str2).addTag(str3);
    printList(lst);
    std::cout << '\n';

    // Test get size.
    std::cout << "--Test get size--" << std::endl;
    std::cout << "#lst size: " << lst.size() << std::endl;
    std::cout << '\n';

    // Test add element with << operator.
    std::cout << "--Test add element with << operator--" << std::endl;
    std::cout << "#lst value before add strings: ";
    printList(lst);
    std::cout << "#lst value after add strings ('  ', 'Bye', '...') with << operator: ";
    Tag str4 = Tag(TT_STRING).setString("  ");
    Tag str5 = Tag(TT_STRING).setString("Bye");
    Tag str6 = Tag(TT_STRING).setString("...");
    lst << str4 << str5 << str6;
    printList(lst);
    std::cout << '\n';

    // Test get front and back element.
    std::cout << "--Test get front and back element--" << std::endl;
    std::cout << "#lst front element: " << lst.frontTag().getString() << std::endl;
    std::cout << "#lst back element: " << lst.backTag().getString() << std::endl;
    std::cout << '\n';

    // Test get element by index.
    std::cout << "--Test get element by index--" << std::endl;
    std::cout << "#lst element at index 2: " << lst.getTag(2).getString() << std::endl;
    std::cout << '\n';

    // Test remove element by index.
    std::cout << "--Test remove element by index--" << std::endl;
    std::cout << "#lst value before remove element at index 2: ";
    printList(lst);
    std::cout << "#lst value after remove element at index 2: ";
    lst.remove(2);
    printList(lst);
    std::cout << '\n';

    // Test copy list.
    std::cout << "--Test copy list--" << std::endl;
    Tag lst2 = lst;
    std::cout << "#lst value: ";
    printList(lst);
    std::cout << "#lst2 value: ";
    printList(lst2);
    std::cout << '\n';

    // Test remove all elements.
    std::cout << "--Test remove all elements--" << std::endl;
    std::cout << "#lst value before remove all: ";
    printList(lst);
    std::cout << "#lst value after remove all: ";
    lst.clear();
    printList(lst);
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
    printList(lst);
    std::cout << "#lst1 value: ";
    Tag lst1 = Tag(TT_LIST).initListElementType(TT_INT);
    lst1 << Tag(TT_INT).setInt(1) << Tag(TT_INT).setInt(2) << Tag(TT_INT).setInt(3);
    printList(lst1);
    lst.addTag(lst1);           // default move constructor. (lst1 is invalid after this operation)
    lst.addTag(lst2.copy());    // copy constructor. (lst2 is still valid after this operation)
    std::cout << "#lst value after add list (#lst1, #lst2): ";
    printList(lst);
    std::cout << "#lst1 value: ";
    printList(lst1);
    std::cout << "#lst2 value: ";
    printList(lst2);
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
    lst << (Tag(TT_LIST).initListElementType(TT_INT) <<
            Tag(TT_INT).setInt(1) << Tag(TT_INT).setInt(2) << Tag(TT_INT).setInt(3));
    lst << (Tag(TT_LIST).initListElementType(TT_STRING) <<
            Tag(TT_STRING).setString("NiHao") << Tag(TT_STRING).setString("ShiJie!"));
    Tag tmpLst = Tag(TT_LIST).initListElementType(TT_DOUBLE);
    tmpLst << Tag(TT_DOUBLE).setDouble(1.1) << Tag(TT_DOUBLE).setDouble(2.2) << Tag(TT_DOUBLE).setDouble(3.3);
    lst << (Tag(TT_LIST).initListElementType(TT_LIST) << tmpLst.copy() << tmpLst);

    root << lst;

    std::cout << "--Test get tag by name--" << std::endl;
    std::cout << "#root value: ";
    printCompound(root);
    std::cout << "#max byte value: " << root.frontTag().getByte() << std::endl;
    std::cout << "#max short value: " << root.getTag("max short").getShort() << std::endl;
    std::cout << "#max int value: " << root["max int"].getInt() << std::endl;
    std::cout << "#max long value: " << root[3].getLong() << std::endl;
    std::cout << "#pi value: " << root.getTag("pi").getFloat() << std::endl;
    std::cout << "#e value: " << root.getTag("e").getDouble() << std::endl;
    std::cout << "#greeting value: " << root.getTag("greeting").getString() << std::endl;
    std::cout << "#byte array value: ";
    printArray(root["byte array"]);
    std::cout << "#nested list value: ";
    printList(root.backTag());
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
