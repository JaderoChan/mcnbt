#include <iostream>

#include <mcnbt/mcnbt.hpp>

using namespace nbt;

void out(const std::string& msg) { std::cout << msg << std::endl; }

void out(const Tag& tag) { std::cout << tag.toSnbt() << std::endl; }

void test1()
{
    auto root = gCompound();

    // Add a same name tag.
    out("<Add same name tag>");

    root << gString("Hello world!", "Text");
    root << gInt(1, "num1");
    root << gInt(2, "num2");
    root << gInt(3, "num3");
    out("-Before Value: ");
    out(root);

    root << gString("Hello NBT!", "Text");
    out("-After Value (add same name tag \"Text\"): ");
    out(root);

    // Nested compound tag set and add same name tag
    out("<Nested compound tag set and add same name tag>");

    auto nested = gCompound();
    nested << root;

    out("-Before Value: ");
    out(nested);

    nested[0]["num1"].setName("num4").setInt(0);
    nested[0]["num2"].setName("num4").setInt(4);
    nested[0]["num3"].setName("num5").setInt(5);
    nested[0] << gString("Hello NBT!", "Text");

    out("-After Value: ");
    out(nested);

    nested[0].hasTag("num1") ? out("num1 exists.") : out("num1 not exists.");
    nested[0].hasTag("num2") ? out("num2 exists.") : out("num2 not exists.");
    nested[0].hasTag("num3") ? out("num3 exists.") : out("num3 not exists.");
    nested[0].hasTag("num4") ? out("num4 exists.") : out("num4 not exists.");
    nested[0].hasTag("num5") ? out("num5 exists.") : out("num5 not exists.");
    nested[0].hasTag("num6") ? out("num6 exists.") : out("num6 not exists.");

    // Invaild assignment.
    out("<Invaild assignment> (Assign parent to self)");

    out("-Before Value: ");
    out(nested);

    try {
        nested[0] = nested;
    } catch (std::exception& e) {
        out(e.what());
    }

    out("-After Value: ");
    out(nested);

    // Valid assignment.
    out("<Valid assignment> (Copy parent to self)");

    out("-Before Value: ");
    out(nested);

    try {
        nested[0] = nested.copy();
    } catch (std::exception& e) {
        out(e.what());
    }

    out("-After Value: ");
    out(nested);

    // Assign to self.
    out("<Assign to self>");

    out("-Before Value: ");
    out(nested);

    try {
        nested = nested;
    } catch (std::exception& e) {
        out(e.what());
    }

    out("-After Value: ");
    out(nested);

    // Set list element name.
    out("<Set list element name>");

    out("-Before Value: ");
    auto list = gList(TT_INT, "list");
    list << gInt(1) << gInt(2) << gInt(3);
    out(list);

    try {
        list[0].setName("num1");
        list[1].setName("num2");
        list[2].setName("num3");
    } catch (std::exception& e) {
        out(e.what());
    }

    out("-After Value: ");
    out(list);
}

void test2()
{
    auto root = gCompound();
    auto list = gList(TT_INT, "list");

    list << gInt(1) << gInt(2) << gInt(3);
    root << list;
    root << gInt(4, "num1");

    out("-Before Value: ");
    out(root);

    root[0][0].setInt(4);
    root[0][1].setInt(5);
    root[0][2].setInt(6);

    out("-After Value: ");
    out(root);

    out("-List Value: ");
    out(root[0]);

    // Add the list element to the root.
    out("---Start---");

    out("-Before Value: ");
    out(root);

    try {
        root << root[0][0] << root[0][1] << root[0][2];
    } catch (std::exception& e) {
        out(e.what());
    }

    out("-After Value: ");
    out(root);
    out("List value");
    out(list);

    out("---End---");

    // Assign to list element.
    out("---Start---");

    out("-Before Value: ");
    out(root);

    try {
        root[0][0] = gInt(10, "num0");
        root[0] << gInt(11, "num2");
        root[0] << root[1];
    } catch (std::exception& e) {
        out(e.what());
    }

    out("-After Value: ");
    out(root);

    out("---End---");
}

void test3()
{
    Tag lst(TT_LIST);
    lst.initListElementType(TT_STRING);

    Tag str1 = Tag(TT_STRING).setString("1");
    Tag str2 = Tag(TT_STRING).setString("2");
    Tag str3 = Tag(TT_STRING).setString("3");
    Tag str4 = Tag(TT_STRING).setString("4");
    Tag str5 = Tag(TT_STRING).setString("5");
    Tag str6 = Tag(TT_STRING).setString("6");
    Tag str7 = Tag(TT_STRING).setString("7");
    Tag str8 = Tag(TT_STRING).setString("8");
    Tag str9 = Tag(TT_STRING).setString("9");
    Tag str10 = Tag(TT_STRING).setString("10");
    lst.addTag(str1);
    lst.addTag(str2);
    lst.addTag(str3);
    lst.addTag(str4);
    lst.addTag(str5);
    lst.addTag(str6);
    lst.addTag(str7);
    lst.addTag(str8);
    lst.addTag(str9);
    lst.addTag(str10);

    out(lst);
    std::cout << lst[0].parent() << std::endl;
    std::cout << lst[1].parent() << std::endl;
    std::cout << lst[2].parent() << std::endl;
    std::cout << lst[3].parent() << std::endl;
    std::cout << lst[4].parent() << std::endl;
    std::cout << lst[5].parent() << std::endl;
    std::cout << lst[6].parent() << std::endl;
    std::cout << lst[7].parent() << std::endl;
    std::cout << lst[8].parent() << std::endl;
    std::cout << lst[9].parent() << std::endl;
}

void test4()
{
    auto root = gCompound("Root");

    root << gInt(1, "1");
    root << gInt(2, "2");
    root << gInt(3, "3");
    root << gInt(4, "4");
    root << gInt(5, "5");

    out(root);

    root[0].setName("2");
    out(root);

    root[1].setName("3");

    out(root);
}

void test5()
{
    auto root = gCompound();
    root << gInt(1, "num1");
    root << gInt(2, "num2");
    root << gInt(3, "num3");

    auto nested = gCompound();
    nested << root;

    out(nested);
    out("------------------------------------");
    nested[0]["num1"].setName("num5").setInt(5);    // num5 = 5; num2 = 2; num3 = 3;
    out(nested);
    out("------------------------------------");
    nested[0]["num2"].setName("num5").setInt(3);    // num5 = 3; num3 = 3;
    out(nested);
    out("------------------------------------");
    nested[0]["num3"].setName("num6").setInt(6);    // num5 = 3; num6 = 6;
    out(nested);
    out("------------------------------------");
}

void test6()
{
    auto root = gCompound();

    root << gInt(1, "num1");
    root << gInt(2, "num2");
    root << gInt(3, "num3");
    root << gInt(4, "num4");
    root << gInt(5, "num5");

    out(root);

    root.remove("num2");
    root.remove("num4");

    out(root);

    out(root[0]);
    out(root[1]);
    out(root[2]);

    out(root.hasTag("num1") ? "num1 exists." : "num1 not exists.");
    out(root.hasTag("num2") ? "num2 exists." : "num2 not exists.");
    out(root.hasTag("num3") ? "num3 exists." : "num3 not exists.");
    out(root.hasTag("num4") ? "num4 exists." : "num4 not exists.");
    out(root.hasTag("num5") ? "num5 exists." : "num5 not exists.");
}

int main()
{
    test1();
    test2();
    test3();
    test4();
    test5();
    test6();

    return 0;
}
