# MCNBT

**简体中文 | [English](doc/README_EN.md)**

 易于使用的MC NBT格式Header-Only读写库。高性能，支持解压缩，支持Java版与基岩版，基岩版实体数据结构。

## :earth_africa: 依赖

**zlib:** 用于解压缩使用了gzip的NBT标签。若需要使用，可通过在包含*mcnbt.hpp*头文件之前加入以下宏进行启用。

```cpp
#define MCNBT_USE_GZIP
```

例如：

```cpp
#define MCNBT_USE_GZIP
#include <mcnbt/mcnbt>

// 其他代码
```

## :rocket: 特点

- 简单易用
- 仅头文件库，易于添加至项目中
- 速度快，性能好
- 除解压缩需要zlib库外，无其他依赖，C++11标准及以上即可使用
- 支持基岩版与Java版的NBT读写（使用不同字节序，基岩版为小端序，Java版为大端序）
- 支持使用zlib库进行gzip解压缩
- 支持SNBT
- 可生成单方块实体方块结构（仅基岩版），用于对实体方块的导入（目前仅支持结构方块与命令方块）

## :robot: 兼容文件

.dat
.nbt
.mcstructure
.schematic
等等

## :triangular_flag_on_post: 使用方法

只需要在项目中包含*mcnbt.hpp*头文件（对于使用了MCNBT_USE_GZIP宏进行启用gzip解压缩的情况下，项目还需要配置好*zlib*库）

### 1、从文件中读取NBT

```cpp
//...
std::string filename = "C:/nbt.nbt";    // NBT文件路径
bool isBigEndian = true;                // 指定待读取的NBT文件的字节序（基岩版NBT使用小端序，Java版使用大端序）
nbt::Tag nbt = nbt::fromFile(filename, isBigEndian);
//...
```

### 2、构造一个NBT

```cpp
// ...
// 方法一（指定Tag的Tag type进行构造）
nbt::Tag tag1 = nbt::Tag(nbt::TT_INT);
tag1.setInt(1);
tag1.setName("Tag1");

nbt::Tag tag2 = nbt::Tag(nbt::TT_INT).setInt(1).setName("Tag2");
// 方法二（使用Fast way方法进行带值，名称（可选）构造）
nbt::Tag tag3 = nbt::gInt(1, "Tag3");
// ...
```

**方法二** 是一个“便利函数”用于快速创建特定对象，在本例中，其构造了一个名为*Tag3*的Int类型Tag。

所有的便利函数：

- nbt::gByte(nbt::byte value, const std::string& name)
- nbt::gShort(nbt::int16 value, const std::string& name)
- nbt::gInt(nbt::int32 value, const std::string& name)
- nbt::gLong(nbt::int64 value, const std::string& name)
- nbt::gFloat(nbt::fp32 value, const std::string& name)
- nbt::gDouble(nbt::fp64 value, const std::string& name)
- nbt::gByteArray(const std::vector\<nbt::byte\>& value, const std::string& name)
- nbt::gIntArray(const std::vector\<nbt::int32\>& value, const std::string& name)
- nbt::gLongArray(const std::vector\<nbt::int64\>& value, const std::string& name)
- nbt::gList(const std::vector\<nbt::Tag\>& value, const std::string& name)
- nbt::gCompound(const std::string& name)

### 3、读取与修改Tag值

对于数字类型的Tag，直接调用对应的get或set函数即可。
如对于一个nbt::INT类型的Tag

```cpp
//...
Nbt::Tag num = Nbt::gInt(1, "Num");
num.getInt();   // return 1.
num.setInt(10);
num.getInt();   // return 10.
//...
```

对于数组类型Tag（ByteArray，IntArray，LongArray），可以使用对应的get函数获取整个数据对象(std::vector\<T\>类型)指针（可能为nullptr，在操作前可通过empty函数判断其是否为空或者不存在）并直接对其操作，对于简单的增删元素可以使用相应的add和remove函数。
如对于一个nbt::IntArray类型的Tag

```cpp
//...
// 通过一个空的std::vector<nbt::int32>初始化int类型数组。
nbt::Tag intArray = nbt::gIntArray({}, "Nums");
intArray.addInt(1);     // 添加值。
intArray.addInt(2);
intArray.addInt(3);
intArray.remove(0);     // 通过索引移除值。
int first = (*intArray.getIntArray())[0];       // first == 2.
//...
```

对于复合数据类型（List，Compound），可以使用addTag与removeTag和<<运算符重载函数进行增删操作，使用getTag或[]运算符重载函数进行读取操作。
对于Compound标签，[]运算符重载函数可以使用Tag在Compound中的位置索引与Tag的名称作为参数，若未找到会引发报错，所以在获取元属前应该使用hasTag()函数进行判断。
而List标签的[]运算符重载函数只能使用索引作为参数。

```cpp
//...
nbt::Tag root = nbt::gCompound("Compound");
root.addTag(gInt(1, "Num1"));       // 使用addTag()函数添加元素。
root << gInt(2, "Num2");            // 使用<<运算符添加元素。
root.removeTag("Num1");             // 使用元素名称移除元素。
root.removeTag(0);                  // 使用元素索引移除元素。
root.addTag(gString("text", "str"));
root["str"].setString("text2");     // 使用元素名称获取元素。
std::string str1 = root[0].getString();         // 使用元素索引获取元素。str1 == "text2"
std::string str2 = root.front().getString();    // 使用front或back函数获取元素。str2 == "tex2"

nbt::Tag list = nbt::gList(Nbt::TT_INT, "List");    // gList函数的第一个参数用于指定List内部Tag的类型
list.addTagr(gInt(1));              // 使用addTag()函数添加元素。
list << gInt(2);                    // 使用<<运算符添加元素。
int num = list[0].getInt();         // num == 1
//...
```
