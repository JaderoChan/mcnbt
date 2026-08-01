# MCNBT

**简体中文 | [English](./README_EN.md)**

易于使用的 Minecraft NBT 格式 Header-Only C++ 读写库。支持 Java 版（大端序）与基岩版（小端序），支持 SNBT，支持 gzip 解压缩，提供基岩版实体/方块实体/方块状态数据结构。

## 特点

- 仅头文件库，易于集成
- C++11 标准，除 gzip 外无额外依赖
- 支持基岩版（小端序）与 Java 版（大端序）NBT 二进制读写
- 支持 gzip 压缩/解压（需 zlib，默认随项目编译）
- 支持 SNBT 格式读写
- 提供保持插入顺序的 `OrderedTag`
- 提供基岩版 `be/` 模块（block entity、block state、entity、item 数据结构及 `.mcstructure` 生成工具）

## 兼容文件格式

`.dat` `.nbt` `.mcstructure` `.schematic` 等所有 NBT 二进制格式

## 获取与构建

```sh
git clone https://github.com/JaderoChan/mcnbt.git
cd mcnbt
cmake -B build -DCMAKE_BUILD_TYPE=Release
```

## 安装

```sh
cmake --install build [--prefix <安装路径>]
```

## CMake 选项

| 选项 | 默认值 | 说明 |
| --- | --- | --- |
| `MCNBT_BUILD_ZLIB` | `ON` | 编译 zlib 以支持 gzip 压缩/解压；关闭后不定义 `MCNBT_HAS_ZLIB`，压缩相关接口不可用 |
| `MCNBT_USE_BUILTIN_ZLIB` | `ON` | 使用内置 `3rdparty/zlib` 子模块；关闭后改用 `find_package(ZLIB REQUIRED)` 查找系统 zlib |

> `MCNBT_BUILD_ZLIB=ON` 时，生成的 `config.hpp` 中会定义 `MCNBT_HAS_ZLIB`，可在代码中用 `#ifdef MCNBT_HAS_ZLIB` 条件编译。

## CMake 集成

1. 已安装

    ```cmake
    find_package(mcnbt REQUIRED)
    target_link_libraries(${YOUR_TARGET} PRIVATE mcnbt::mcnbt)
    ```

2. 作为子项目

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

### Tag 类型

| 枚举值 | C++ 类型 | 描述 |
| --- | --- | --- |
| `TT_END` | — | 空标签（默认构造） |
| `TT_BYTE` | `int8_t` | 字节；也用于存储布尔值 |
| `TT_SHORT` | `int16_t` | 短整型 |
| `TT_INT` | `int32_t` | 整型 |
| `TT_LONG` | `int64_t` | 长整型 |
| `TT_FLOAT` | `float` | 单精度浮点 |
| `TT_DOUBLE` | `double` | 双精度浮点 |
| `TT_STRING` | `std::string` | 字符串 |
| `TT_BYTE_ARRAY` | `std::vector<int8_t>` | 字节数组 |
| `TT_INT_ARRAY` | `std::vector<int32_t>` | 整型数组 |
| `TT_LONG_ARRAY` | `std::vector<int64_t>` | 长整型数组 |
| `TT_LIST` | `std::vector<Tag>` | 同类型标签列表 |
| `TT_COMPOUND` | `std::map<std::string, Tag>` | 键值对集合（按键排序） |

`Tag` 是 `BasicTag<>` 的默认别名，Compound 内部使用 `std::map`（按键字典序排序）。

若需要保持插入顺序，使用 `OrderedTag`：

```cpp
OrderedTag tag = OrderedTag::compound();
```

### 构造 Tag

```cpp
Tag byteVal(int8_t(42));           // TT_BYTE
Tag boolVal(true);                 // TT_BYTE（true 为 1，false 为 0）
Tag shortVal(int16_t(100));        // TT_SHORT
Tag intVal(int32_t(1000));         // TT_INT
Tag longVal(int64_t(1LL));         // TT_LONG
Tag floatVal(3.14f);               // TT_FLOAT
Tag doubleVal(3.14);               // TT_DOUBLE
Tag strVal("hello");               // TT_STRING
Tag strVal2(std::string("world")); // TT_STRING

Tag compound = Tag::compound();    // TT_COMPOUND（空）
Tag list     = Tag::list();        // TT_LIST（空）
```

### Compound 操作

```cpp
Tag root = Tag::compound();

// 赋值插入（key 不存在则创建，存在则覆盖）
root["x"]     = int32_t(128);
root["name"]  = std::string("Steve");
root["alive"] = true;

Tag pos = Tag::list();
pos << int32_t(0) << int32_t(1) << int32_t(2);
root["pos"] = pos;

// 指定插入（已存在时不覆盖）
root.insert("score", int32_t(0));

// 检查并访问
if (root.contains("name"))
    std::string n = root["name"].getString();

// 删除
root.erase("score");

// 大小
size_t n = root.size();
```

### List 操作

```cpp
Tag list = Tag::list();

// 追加
list << int32_t(1) << int32_t(2) << int32_t(3);
list.pushBack(int32_t(4));

// 按索引访问
Tag& first = list[0];

// 在指定位置插入
list.insert(0, int32_t(-1));

// 删除
list.erase(0);

size_t n = list.size();
```

### 读取值

```cpp
// 类型化 getter
int32_t     hp   = root["health"].getInt();
float       spd  = root["speed"].getFloat();
std::string name = root["name"].getString();
root["health"].getInt() += 5;

// 泛型 getter
auto hp2 = root["health"].get<int32_t>();

// 显式转换
int32_t hp3 = static_cast<int32_t>(root["health"]);
```

### 遍历

```cpp
// 遍历 List
for (Tag& item : list) { ... }

// 遍历 Compound（仅值）
for (Tag& val : root) { ... }

// 遍历 Compound 键值对
for (const auto& kv : root.items())
    std::cout << kv.first << " = " << kv.second.toSnbt() << "\n";
```

### 二进制 I/O

```cpp
// 从文件解析（false = 小端序/基岩版，true = 大端序/Java 版）
auto result   = Tag::parse("level.dat", false);
std::string rootName = result.first;
Tag root      = result.second;

// C++17 结构化绑定
auto [name, root2] = Tag::parse("level.dat", false);

// 带 headerSkip（跳过某些基岩版地图文件的 8 字节头）
auto [name3, root3] = Tag::parse("level.dat", false, 8);

// 写入文件
root.dump("output.nbt", false, "RootName");

// 从流/向流读写
std::ifstream ifs("level.dat", std::ios::binary);
auto [name4, root4] = Tag::parse(ifs, false);

std::ofstream ofs("output.nbt", std::ios::binary);
root4.dump(ofs, false, "RootName");
```

**gzip 压缩 I/O**（需 `MCNBT_BUILD_ZLIB=ON`）：

```cpp
// parse() 在启用 zlib 时自动检测并解压 gzip 数据
auto [name, root] = Tag::parse("compressed.nbt", false);

// 压缩写入
root.dumpCompressed("compressed.nbt", false, "RootName");
```

### SNBT

```cpp
// 序列化为 SNBT
std::string compact  = root.toSnbt();    // 紧凑格式
std::string indented = root.toSnbt(4);   // 每级缩进 4 空格

// 从 SNBT 解析
Tag t = Tag::fromSnbt("{health:20,name:\"Steve\"}");
```

### 类型检查

```cpp
tag.isEnd();       tag.isByte();      tag.isShort();     tag.isInt();
tag.isLong();      tag.isFloat();     tag.isDouble();    tag.isString();
tag.isByteArray(); tag.isIntArray();  tag.isLongArray();
tag.isList();      tag.isCompound();
TagType tt = tag.type();   // 返回 TagType 枚举
```

---

## 基岩版模块（`mcnbt/be/`）

提供基岩版常用数据结构，所有结构体均为模板（`template <typename BasicTagType = Tag>`），可配合 `OrderedTag` 使用。

### 头文件

```cpp
#include <mcnbt/be/block_entity.hpp>   // 所有 Block Entity 数据结构
#include <mcnbt/be/block_state.hpp>    // 所有 Block State 数据结构
#include <mcnbt/be/entity.hpp>         // Entity 通用数据
#include <mcnbt/be/item.hpp>           // Item 通用数据
#include <mcnbt/be/mcstructure.hpp>    // MCStructure 与 createSingleBlockStructure
```

### 构建 Block Entity Data（BED）

```cpp
#include <mcnbt/be/block_entity.hpp>

// 命令方块
nbt::be::CommandBlockBED bed;
bed.command       = "say Hello, World!";
bed.isPowered     = true;
bed.isConditional = false;
Tag bedTag = bed.getTag();  // 返回完整的 BED compound tag

// 结构方块
nbt::be::StructureBlockBED sbed;
sbed.structureName = "mystructure:test";
sbed.xStructureSize = 5;
sbed.yStructureSize = 5;
sbed.zStructureSize = 5;
Tag sbedTag = sbed.getTag();
```

### 构建 Block State Data（BSD）

```cpp
#include <mcnbt/be/block_state.hpp>

// 命令方块：非条件，朝向上方
nbt::be::CommandBlockBSD bsd(false, nbt::be::CommandBlockBSD::FD_UP);
Tag bsdTag = bsd.getTag();

// 结构方块：保存模式
nbt::be::StructureBlockBSD sbsd(nbt::be::StructureBlockBSD::MODE_SAVE);
Tag sbsdTag = sbsd.getTag();
```

### 生成单方块 .mcstructure

```cpp
#include <mcnbt/be/mcstructure.hpp>

nbt::be::CommandBlockBED bed;
bed.command = "tp @a 0 64 0";

nbt::be::CommandBlockBSD bsd(false, nbt::be::CommandBlockBSD::FD_NORTH);

// 创建 1×1×1 结构文件
Tag structure = nbt::be::createSingleBlockStructure(
    "minecraft:command_block", bed, bsd
);
structure.dump("command_block.mcstructure", false, "");
```

---

## 许可证

MIT License — 详见 [LICENSE](LICENSE)。
