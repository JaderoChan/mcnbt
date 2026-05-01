#ifndef MCNBT_GZIP_HPP
#define MCNBT_GZIP_HPP

#include <cstddef>      // size_t
#include <string>       // string
#include <limits>       // numeric_limits
#include <stdexcept>    // runtime_error

#ifndef ZLIB_CONST
    #define ZLIB_CONST
#endif // !ZLIB_CONST
#include <zlib.h>

namespace nbt
{

namespace gzip
{

/// @brief Checks if the given data is compressed using Gzip or Zlib.
inline bool isCompressed(const std::string& data)
{
    if (data.size() < 2)
        return false;

    unsigned char byte1 = data[0];
    unsigned char byte2 = data[1];

    bool isZlib = (byte1 == 0x78 && (byte2 == 0x9C || byte2 == 0x01 || byte2 == 0xDA || byte2 == 0x5E));
    bool isGzip = (byte1 == 0x1F && byte2 == 0x8B);

    return isZlib || isGzip;
}

/// @overload
inline bool isCompressed(const char* data, size_t size)
{
    return isCompressed(std::string(data, size));
}

/// @brief Compresses the given data using Gzip.
inline std::string compress(const std::string& data)
{
    if (data.size() > std::numeric_limits<uInt>::max())
        throw std::runtime_error("The input data is too large to be compressed.");

    z_stream stream;
    stream.zalloc = Z_NULL;
    stream.zfree = Z_NULL;
    stream.opaque = Z_NULL;
    stream.avail_in = 0;
    stream.next_in = Z_NULL;

    constexpr int level = Z_DEFAULT_COMPRESSION;
    constexpr int method = Z_DEFLATED;
    constexpr int windowsBits = 15 + 16;
    constexpr int memLevel = 8;
    constexpr int strategy = Z_DEFAULT_STRATEGY;

    int ret = deflateInit2(&stream, level, method, windowsBits, memLevel, strategy);
    if (ret != Z_OK)
        throw std::runtime_error("Failed to initialize zlib deflate.");

    std::string compressed;

    stream.next_in = reinterpret_cast<z_const Bytef*>(data.data());
    stream.avail_in = static_cast<uInt>(data.size());

    uInt compressedSize = 0;
    uInt increaseSize = static_cast<uInt>(data.size()) / 2 + 1024;
    do
    {
        if (compressed.size() < (compressedSize + increaseSize))
            compressed.resize(compressedSize + increaseSize);

        stream.avail_out = increaseSize;
        stream.next_out = reinterpret_cast<Bytef*>(&compressed[0] + compressedSize);

        ret = deflate(&stream, Z_FINISH);
        if (ret != Z_STREAM_END && ret != Z_OK)
        {
            std::string errmsg = stream.msg;
            deflateEnd(&stream);
            throw std::runtime_error("Failed to inflate data: " + errmsg);
        }

        compressedSize += increaseSize - stream.avail_out;
    } while (stream.avail_out == 0);

    deflateEnd(&stream);

    compressed.resize(compressedSize);

    return compressed;
}

/// @overload
inline std::string compress(const char* data, size_t size)
{
    return compress(std::string(data, size));
}

/// @brief Decompresses the given data using Gzip.
inline std::string decompress(const std::string& data)
{
    if (data.size() * 2 > std::numeric_limits<uInt>::max())
        throw std::runtime_error("The input data is too large to be compressed.");

    z_stream stream;

    stream.zalloc = Z_NULL;
    stream.zfree = Z_NULL;
    stream.opaque = Z_NULL;
    stream.avail_in = 0;
    stream.next_in = Z_NULL;

    constexpr int windowsBits = 15 + 32;

    int ret = inflateInit2(&stream, windowsBits);
    if (ret != Z_OK)
        throw std::runtime_error("Failed to initialize zlib inflate.");

    std::string decompressed;

    stream.next_in = reinterpret_cast<z_const Bytef*>(data.data());
    stream.avail_in = static_cast<uInt>(data.size());

    uInt decompressedSize = 0;
    do
    {
        decompressed.resize(data.size() * 2 + decompressedSize);

        stream.avail_out = static_cast<uInt>(decompressed.size() - decompressedSize);
        stream.next_out = reinterpret_cast<Bytef*>(&decompressed[0] + decompressedSize);

        ret = inflate(&stream, Z_FINISH);
        if (ret != Z_STREAM_END && ret != Z_OK)
        {
            std::string errmsg = stream.msg;
            inflateEnd(&stream);
            throw std::runtime_error("Failed to inflate data: " + errmsg);
        }

        decompressedSize = static_cast<uInt>(decompressed.size()) - stream.avail_out;
    } while (stream.avail_out == 0);

    inflateEnd(&stream);

    decompressed.resize(decompressedSize);

    return decompressed;
}

/// @overload
inline std::string decompress(const char* data, size_t size)
{
    return decompress(std::string(data, size));
}

} // namespace gzip

} // namespace nbt

#endif // !MCNBT_GZIP_HPP
