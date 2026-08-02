#include <iostream>

#include <mcnbt/mcnbt.hpp>

int main()
{
    std::string filepath;
    std::cout << "Enter the NBT file path:" << std::endl;
    std::cin >> filepath;

    std::ifstream ifs(filepath, std::ios::binary);
    if (!ifs.is_open())
        throw std::runtime_error("Failed to open the file: " + filepath);

    std::string data((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
    ifs.close();

#ifdef MCNBT_HAS_ZLIB
    if (nbt::gzip::isCompressed(data))
    {
        std::string dec = nbt::gzip::decompress(data);
        if (dec.empty())
        {
            std::cout << "Failed to decompress the NBT file." << std::endl;
            return 1;
        }
        data = std::move(dec);
    }
#endif

    using ResultType = decltype(nbt::Tag::parse("", false));
    ResultType result;
    bool isBigEndian = false;

    // Try both endiannesses; the one that consumes all data is correct.
    bool leOk = false, beOk = false;
    bool leEof = false, beEof = false;
    ResultType leResult, beResult;
    {
        std::istringstream ss(data);
        try { leResult = nbt::Tag::parse(ss, false); leOk = true; leEof = (ss.peek() == EOF); }
        catch (...) {}
    }
    {
        std::istringstream ss(data);
        try { beResult = nbt::Tag::parse(ss, true); beOk = true; beEof = (ss.peek() == EOF); }
        catch (...) {}
    }

    if (!leOk && !beOk)
    {
        std::cout << "Failed to parse the NBT file." << std::endl;
        return 1;
    }

    // Prefer whichever reached EOF; if tied, prefer Big Endian (Java Edition default).
    if (beOk && (beEof || !leOk || (!leEof && !beEof)))
    {
        result = std::move(beResult);
        isBigEndian = true;
    }
    else
    {
        result = std::move(leResult);
    }

    auto rootName = result.first;
    auto root = std::move(result.second);
    std::cout << (isBigEndian ? "[Big Endian]" : "[Little Endian]") << std::endl;
    std::cout << root.toSnbt(2) << std::endl;

    return 0;
}
