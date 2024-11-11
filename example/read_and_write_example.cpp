#include <iostream>
#include <utility>  // std::pair

#include <mcnbt/mcnbt.hpp>

using namespace nbt;

std::pair<std::string, bool> inputHint()
{
    std::string filename;
    bool isBigEndian = false;

    std::cout << "Enter filename: ";
    std::getline(std::cin, filename);

    while (true) {
        std::cout << "Is the file big-endian? (y/n): ";
        std::string input;
        std::getline(std::cin, input);
        if (input == "y" || input == "Y") {
            isBigEndian = true;
            break;
        } else if (input == "n" || input == "N") {
            isBigEndian = false;
            break;
        } else {
            std::cout << "Invalid input. Please enter 'y' or 'n'." << std::endl;
            continue;
        }
    }

    return { filename, isBigEndian };
}

void wrieExample(const std::string& filename, bool isBigEndian)
{
    auto root = gCompound("Person");
    root << gString("Alice", "name");
    root << gInt(25, "age");
    root << gByte(0, "gender (0=male, 1=female)");

    auto birthday = gCompound("birthday");
    birthday << gInt(1990, "year") << gInt(1, "month") << gInt(1, "day");
    root << birthday;

    auto friends = gList(COMPOUND, "friends");
    friends << (gCompound("Person") <<
                gString("Bob", "name") <<
                gInt(26, "age") <<
                gByte(1, "gender (0=male, 1=female)"));
    friends << (gCompound("Person") <<
                gString("Charlie", "name") <<
                gInt(30, "age") <<
                gByte(1, "gender (0=male, 1=female)"));
    root << friends;

    root.write(filename, isBigEndian);
}

void readExample(const std::string& filename, bool isBigEndian)
{
    auto root = Tag::fromFile(filename, isBigEndian);
    std::cout << root.toSnbt() << std::endl;
}

int main()
{
    auto rtn = inputHint();

    std::cout << "Write example:" << std::endl;
    wrieExample(rtn.first, rtn.second);
    std::cout << "Successfully write to " << rtn.first << std::endl;

    std::cout << std::string(60, '-') << std::endl;

    std::cout << "Read example:" << std::endl;
    readExample(rtn.first, rtn.second);

    return 0;
}
