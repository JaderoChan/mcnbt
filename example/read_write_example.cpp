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

    while (true)
    {
        std::cout << "Is the file big-endian? (y/n): ";
        std::string input;
        std::getline(std::cin, input);
        if (input == "y" || input == "Y")
        {
            isBigEndian = true;
            break;
        }
        else if (input == "n" || input == "N")
        {
            isBigEndian = false;
            break;
        }
        else
        {
            std::cout << "Invalid input. Please enter 'y' or 'n'." << std::endl;
            continue;
        }
    }

    return {filename, isBigEndian};
}

void writeExample(const std::string& filename, bool isBigEndian)
{
    auto root = Tag::compound();
    root["name"]   = "Alice";
    root["age"]    = 25;
    root["gender (0=male, 1=female)"] = int8_t(0);

    auto birthday = Tag::compound();
    birthday["year"]  = 1990;
    birthday["month"] = 1;
    birthday["day"]   = 1;
    root["birthday"] = std::move(birthday);

    auto friends = Tag::list();

    auto bob = Tag::compound();
    bob["name"]   = "Bob";
    bob["age"]    = 26;
    bob["gender (0=male, 1=female)"] = int8_t(1);
    friends << std::move(bob);

    auto charlie = Tag::compound();
    charlie["name"]   = "Charlie";
    charlie["age"]    = 30;
    charlie["gender (0=male, 1=female)"] = int8_t(1);
    friends << std::move(charlie);

    root["friends"] = std::move(friends);

    root.dump(filename, isBigEndian, "Person");
}

void readExample(const std::string& filename, bool isBigEndian)
{
    auto result = Tag::parse(filename, isBigEndian);
    std::cout << result.second.toSnbt(2) << std::endl;
}

int main()
{
    auto rtn = inputHint();

    std::cout << "Write example:" << std::endl;
    writeExample(rtn.first, rtn.second);
    std::cout << "Successfully wrote to " << rtn.first << std::endl;

    std::cout << std::string(60, '-') << std::endl;

    std::cout << "Read example:" << std::endl;
    readExample(rtn.first, rtn.second);

    return 0;
}
