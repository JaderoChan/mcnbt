# MCNBT

**[简体中文](./README_ZH.md) | English**

A header-only C++ library for reading and writing Minecraft NBT format. Supports Java Edition (big-endian) and Bedrock Edition (little-endian), SNBT, gzip decompression, and provides Bedrock Edition entity/block entity/block state data structures.

## Features

- Header-only, easy to integrate
- C++11 standard, no extra dependencies except for gzip support
- Supports Bedrock Edition (little-endian) and Java Edition (big-endian) NBT binary I/O
- Supports gzip compression/decompression (requires zlib, compiled with the project by default)
- Supports SNBT format read/write
- Provides `OrderedTag` that preserves insertion order
- Provides a Bedrock Edition `be/` module (block entity, block state, entity, item data structures and `.mcstructure` generation utilities)

## Compatible File Formats

`.dat` `.nbt` `.mcstructure` `.schematic` and all other NBT binary formats

## Clone & Build

```sh
git clone https://github.com/JaderoChan/mcnbt.git
cd mcnbt
cmake -B build -DCMAKE_BUILD_TYPE=Release
```

## Install

```sh
cmake --install build [--prefix <install-path>]
```

## CMake Options

| Option | Default | Description |
| --- | --- | --- |
| `MCNBT_BUILD_ZLIB` | `ON` | Compile zlib to enable gzip compression/decompression; when disabled, `MCNBT_HAS_ZLIB` is not defined and compression-related APIs are unavailable |
| `MCNBT_USE_BUILTIN_ZLIB` | `ON` | Use the bundled `3rdparty/zlib` submodule; when disabled, uses `find_package(ZLIB REQUIRED)` to find the system zlib |

> When `MCNBT_BUILD_ZLIB=ON`, the generated `config.hpp` defines `MCNBT_HAS_ZLIB`, which can be used for conditional compilation with `#ifdef MCNBT_HAS_ZLIB`.

## CMake Integration

1. Installed

    ```cmake
    find_package(mcnbt REQUIRED)
    target_link_libraries(${YOUR_TARGET} PRIVATE mcnbt::mcnbt)
    ```

2. As a subdirectory

    ```cmake
    include(FetchContent)
    FetchContent_Declare(
        mcnbt
        SOURCE_DIR ${YOUR_MCNBT_PATH}
    )
    FetchContent_MakeAvailable(mcnbt)
    target_link_libraries(${YOUR_TARGET} PRIVATE mcnbt::mcnbt)
    ```

---

### Tag Types

| Enum Value | C++ Type | Description |
| --- | --- | --- |
| `TT_END` | — | Empty tag (default-constructed) |
| `TT_BYTE` | `int8_t` | Byte; also used for booleans |
| `TT_SHORT` | `int16_t` | Short integer |
| `TT_INT` | `int32_t` | Integer |
| `TT_LONG` | `int64_t` | Long integer |
| `TT_FLOAT` | `float` | Single-precision float |
| `TT_DOUBLE` | `double` | Double-precision float |
| `TT_STRING` | `std::string` | String |
| `TT_BYTE_ARRAY` | `std::vector<int8_t>` | Byte array |
| `TT_INT_ARRAY` | `std::vector<int32_t>` | Integer array |
| `TT_LONG_ARRAY` | `std::vector<int64_t>` | Long integer array |
| `TT_LIST` | `std::vector<Tag>` | List of same-type tags |
| `TT_COMPOUND` | `std::map<std::string, Tag>` | Key-value collection (sorted by key) |

`Tag` is the default alias for `BasicTag<>`, where Compound internals use `std::map` (sorted lexicographically by key).

To preserve insertion order, use `OrderedTag`:

```cpp
OrderedTag tag = OrderedTag::compound();
```

### Constructing Tags

```cpp
Tag byteVal(int8_t(42));           // TT_BYTE
Tag boolVal(true);                 // TT_BYTE (true = 1, false = 0)
Tag shortVal(int16_t(100));        // TT_SHORT
Tag intVal(int32_t(1000));         // TT_INT
Tag longVal(int64_t(1LL));         // TT_LONG
Tag floatVal(3.14f);               // TT_FLOAT
Tag doubleVal(3.14);               // TT_DOUBLE
Tag strVal("hello");               // TT_STRING
Tag strVal2(std::string("world")); // TT_STRING

Tag compound = Tag::compound();    // TT_COMPOUND (empty)
Tag list     = Tag::list();        // TT_LIST (empty)
```

### Compound Operations

```cpp
Tag root = Tag::compound();

// Assign/insert (creates the key if absent, overwrites if present)
root["x"]     = int32_t(128);
root["name"]  = std::string("Steve");
root["alive"] = true;

Tag pos = Tag::list();
pos << int32_t(0) << int32_t(1) << int32_t(2);
root["pos"] = pos;

// Insert without overwrite (no-op if key exists)
root.insert("score", int32_t(0));

// Check and access
if (root.contains("name"))
    std::string n = root["name"].getString();

// Remove
root.erase("score");

// Size
size_t n = root.size();
```

### List Operations

```cpp
Tag list = Tag::list();

// Append
list << int32_t(1) << int32_t(2) << int32_t(3);
list.pushBack(int32_t(4));

// Access by index
Tag& first = list[0];

// Insert at position
list.insert(0, int32_t(-1));

// Remove
list.erase(0);

size_t n = list.size();
```

### Reading Values

```cpp
// Typed getters
int32_t     hp   = root["health"].getInt();
float       spd  = root["speed"].getFloat();
std::string name = root["name"].getString();
root["health"].getInt() += 5;

// Generic getter
auto hp2 = root["health"].get<int32_t>();

// Explicit cast
int32_t hp3 = static_cast<int32_t>(root["health"]);
```

### Iteration

```cpp
// Iterate over List
for (Tag& item : list) { ... }

// Iterate over Compound (values only)
for (Tag& val : root) { ... }

// Iterate over Compound key-value pairs
for (const auto& kv : root.items())
    std::cout << kv.first << " = " << kv.second.toSnbt() << "\n";
```

### Binary I/O

```cpp
// Parse from file (false = little-endian/Bedrock, true = big-endian/Java)
auto result          = Tag::parse("level.dat", false);
std::string rootName = result.first;
Tag root             = result.second;

// C++17 structured bindings
auto [name, root2] = Tag::parse("level.dat", false);

// With headerSkip (skip the 8-byte header found in some Bedrock world files)
auto [name3, root3] = Tag::parse("level.dat", false, 8);

// Write to file
root.dump("output.nbt", false, "RootName");

// Read from / write to streams
std::ifstream ifs("level.dat", std::ios::binary);
auto [name4, root4] = Tag::parse(ifs, false);

std::ofstream ofs("output.nbt", std::ios::binary);
root4.dump(ofs, false, "RootName");
```

**Gzip compressed I/O** (requires `MCNBT_BUILD_ZLIB=ON`):

```cpp
// parse() automatically detects and decompresses gzip data when zlib is enabled
auto [name, root] = Tag::parse("compressed.nbt", false);

// Write with compression
root.dumpCompressed("compressed.nbt", false, "RootName");
```

### SNBT

```cpp
// Serialize to SNBT
std::string compact  = root.toSnbt();    // compact format
std::string indented = root.toSnbt(4);   // 4-space indent per level

// Parse from SNBT
Tag t = Tag::fromSnbt("{health:20,name:\"Steve\"}");
```

### Type Checks

```cpp
tag.isEnd();       tag.isByte();      tag.isShort();     tag.isInt();
tag.isLong();      tag.isFloat();     tag.isDouble();    tag.isString();
tag.isByteArray(); tag.isIntArray();  tag.isLongArray();
tag.isList();      tag.isCompound();
TagType tt = tag.type();   // returns a TagType enum value
```

---

## Bedrock Edition Module (`mcnbt/be/`)

Provides common Bedrock Edition data structures. All structs are templates (`template <typename BasicTagType = Tag>`) and can be used with `OrderedTag`.

### Headers

```cpp
#include <mcnbt/be/block_entity.hpp>   // All Block Entity data structures
#include <mcnbt/be/block_state.hpp>    // All Block State data structures
#include <mcnbt/be/entity.hpp>         // Entity common data
#include <mcnbt/be/item.hpp>           // Item common data
#include <mcnbt/be/mcstructure.hpp>    // MCStructure and createSingleBlockStructure
```

### Building Block Entity Data (BED)

```cpp
#include <mcnbt/be/block_entity.hpp>

// Command block
nbt::be::CommandBlockBED bed;
bed.command       = "say Hello, World!";
bed.isPowered     = true;
bed.isConditional = false;
Tag bedTag = bed.getTag();  // returns the complete BED compound tag

// Structure block
nbt::be::StructureBlockBED sbed;
sbed.structureName  = "mystructure:test";
sbed.xStructureSize = 5;
sbed.yStructureSize = 5;
sbed.zStructureSize = 5;
Tag sbedTag = sbed.getTag();
```

### Building Block State Data (BSD)

```cpp
#include <mcnbt/be/block_state.hpp>

// Command block: unconditional, facing up
nbt::be::CommandBlockBSD bsd(false, nbt::be::CommandBlockBSD::FD_UP);
Tag bsdTag = bsd.getTag();

// Structure block: save mode
nbt::be::StructureBlockBSD sbsd(nbt::be::StructureBlockBSD::MODE_SAVE);
Tag sbsdTag = sbsd.getTag();
```

### Generating a Single-Block .mcstructure

```cpp
#include <mcnbt/be/mcstructure.hpp>

nbt::be::CommandBlockBED bed;
bed.command = "tp @a 0 64 0";

nbt::be::CommandBlockBSD bsd(false, nbt::be::CommandBlockBSD::FD_NORTH);

// Create a 1×1×1 structure file
Tag structure = nbt::be::createSingleBlockStructure(
    "minecraft:command_block", bed, bsd
);
structure.dump("command_block.mcstructure", false, "");
```

---

## License

MIT License — see [LICENSE](LICENSE) for details.
