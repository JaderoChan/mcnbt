# MCNBT

**[简体中文](README_CN.md) | English**

An easy-to-use Header-Only read and write library in MC NBT format. High performance, supports decompression, supports Java edition and Bedrock edition, BE entity data structure.

## :earth_africa: Dependency

**zlib:** Used to decompress or compress NBT tags that use gzip. If needed, it can be enabled by adding the following macros before including the *mcnbt.hpp* header file.

```cpp
#define MCNBT_USE_GZIP
```

e.g.

```cpp
#define MCNBT_USE_GZIP
#include <mcnbt/mcnbt>

// Others.
```

## :rocket: Features

- Easy to use
- Header-only library, easy to add to project
- Fast speed, high performance
- In addition to the *zlib* library required for decompress and compress, there are no other dependencies, and C++11 standard and above can be used
- Support bedrock edition and java edition (bedrock edition nbt byte order is little endian, java is big endian)
- Support gzip decompress and compress by *zlib* library
- Support SNBT
- Can be generate single block entity block structure for import entity block (Currently only structure blocks and command blocks are supported) (Only bedrock edition)

## :robot: File supported

.dat
.nbt
.mcstructure
.schematic
and so on

## :triangular_flag_on_post: Usage

You only need to include the *mcnbt.hpp* header file in your project（In cases where gzip decompression and compression is enabled using the MCNBT_USE_GZIP macro, the project will also need to configure the *zlib* library）

### 1. Load a NBT from file

```cpp
//...
std::string filename = "C:/nbt.nbt";    // NBT filename.
bool isBigEndian = true;                // Specifies the byte order of the NBT file to be read (small endian order is used for bedrock edition NBT, large endian order is used for java edition)
nbt::Tag nbt = nbt::fromFile(filename, isBigEndian);
//...
```

### 2. Construct a NBT

```cpp
// ...
// Method 1 (Construct by specifies Tag type of tag)
nbt::Tag tag1 = nbt::Tag(nbt::TT_INT);
tag1.setInt(1);
tag1.setName("Tag1");

nbt::Tag tag2 = nbt::Tag(nbt::TT_INT).setInt(1).setName("Tag2");
// Method 2 (Construct by fast way functions)
nbt::Tag tag3 = nbt::gInt(1, "Tag3");
// ...
```

**Method 2** use the fast way functions for speed construct specify object. In this case it construct a NBT that tag type is INT.

All fast way functions:

- nbt::gByte(nbt::Byte value, const std::string& name)
- nbt::gShort(nbt::Int16 value, const std::string& name)
- nbt::gInt(nbt::Int32 value, const std::string& name)
- nbt::gLong(nbt::Int64 value, const std::string& name)
- nbt::gFloat(nbt::Fp32 value, const std::string& name)
- nbt::gDouble(nbt::fp64 value, const std::string& name)
- nbt::gByteArray(const std::vector\<nbt::Byte\>& value, const std::string& name)
- nbt::gIntArray(const std::vector\<nbt::Int32\>& value, const std::string& name)
- nbt::gLongArray(const std::vector\<nbt::Int64\>& value, const std::string& name)
- nbt::gList(const std::vector\<nbt::Tag\>& value, const std::string& name)
- nbt::gCompound(const std::string& name)

### 3. Read and modify tag value

For nums Tag, direct call the corresponding getter/setter.
e.g.

```cpp
//...
Nbt::Tag num = Nbt::gInt(1, "Num");
num.getInt();   // return 1.
num.setInt(10);
num.getInt();   // return 10.
//...
```

For arrays Tag (ByteArray, IntArray, LongArray), use getter to obtain a std::vector\<T\> point (maybe it is nullptr, you should call the empty to check whether it is empty or not exists) to operate element, for simple add or erase, you can use corresponding add and remove functions.
e.g.

```cpp
//...
// Initialize a int array by empty std::vector<nbt::Int32> value.
nbt::Tag intArray = nbt::gIntArray({}, "Nums");
intArray.addInt(1);         // Add a value.
intArray.addInt(2);
intArray.addInt(3);
intArray.remove(0);         // Remove a value by index.
int first = (*intArray.getIntArray())[0];   // first == 2.
//...
```

For complex Tag (List，Compound), use addTag and removeTag functions and << operator to add or ease, use getTag function or [] operator to read element.

For compound Tag，[] operator can use element's name or index as parameter, return the Tag with the smallest index value, if not find specify element throw exception, therefore, the hasTag() function should be called before obtaining the element.

For list Tag, only get element by index.

```cpp
//...
nbt::Tag root = nbt::gCompound("Compound");
root.addTag(gInt(1, "Num1"));           // add element by addTag function.
root << gInt(2, "Num2");                // add element by << operator.
root.removeTag("Num1");                 // earse element by element's name.
root.removeTag(0);                      // earse element by element's index.
root.addTag(gString("text", "str"));
root["str"].setString("text2");         // get element by it's name.
std::string str1 = root[0].getString(); // get element by it's index str1 == "text2"
std::string str2 = root.front().getString();        // get element by front and back function. str2 == "tex2"

nbt::Tag list = nbt::gList(Nbt::TT_INT, "List");    // gList function's first parameter used for specifies the tag type of element.
list.addTagr(gInt(1));
list << gInt(2);
int num = list[0].getInt();             // num == 1
//...
```
