# MCNBT

An easy-to-use NBT read and write Header-Only library in C++.

## :earth_africa: Dependency

**cpp-gzip:** Used to extract NBT tags that use GZIP compressed. If you do not need to use it, you can disable it by adding the following macros to the *nbt.hpp* file.

Goto: https://github.com/mapbox/gzip-hpp

```cpp
#define NBT_NOGZIP
```

## :rocket: Feature

- Simple and easy to use
- Fast speed and high-performance
- Support for different byte orders (large endian order is usually used for numeric Tag for Java, while small endian order is used for bedrock)
- Support convert NBT to SNBT
- Can generate a single block entity structure (bedrock only) for importing special entities (currently only structure block and command block are supported)

## :robot: Files supported

.dat
.nbt
.mcstructure
.schematic

## :triangular_flag_on_post: Usage

For standard NBT operations, you only need to include the *nbt.hpp* header file in your project (if the GZIP library is not disabled with the NBT_NOGZIP macro, the project will also need to configure the *cpp-gzip* library)

### 1. Read the NBT from the file

```cpp
#include "nbt.hpp"

int main(){
    //...
    Nbt::Tag nbt = Nbt::Tag(Nbt::Compound, is, false);
    //...
    
    return 0;
}
```

The *Nbt::Tag()* function in the code is a constructor of the Nbt::Tag class, which takes an Nbt::TagTypes, input stream, and Boolean value as arguments.

**Argument1 (Nbt::TagType enum)**: Used to indicate the type of the root tag (which can be both List and Compound), Nbt::Compound is usually chosen as the first parameter.

**Argument2 (std::istream)**: If reading from a file (the file must be read in binary form), the ifstream object of the file should be passed to represent reading the NBT from the file input stream.

**Argument3 (bool)**: Used to indicate the byte order of the current input stream, the default is small endian order, that is, the default support for reading bedrock NBT files.

### 2、Construct NBT

```cpp
#include <nbt.hpp>

int main(){
    // Method 1
    Nbt::Tag nbt = Nbt::Tag(Nbt::Compound, "root");
    // Method 2
    Nbt::Tag list = Nbt::gList("version");
    
    return 0;
}
```

**Method 1** Constructs a NBT of Compound named *root*.

**Method 2** Is a "convenience function" for quickly creating a particular object, which in this case constructs a List named *version*.

All convenience functions:

- Nbt::gByte()
- Nbt::gShort()
- Nbt::gInt()
- Nbt::gLong()
- Nbt::gFloat()
- Nbt::gDouble()
- Nbt::gByteArray()
- Nbt::gIntArray()
- Nbt::gLongArray()
- Nbt::gList()
- Nbt::gCompound()

And the PureData version of the above convenience function:

- Nbt::gpByte()
- Nbt::gpShort()
- Nbt::gpInt()
- Nbt::gpLong()
- Nbt::gpFloat()
- Nbt::gpDouble()
- Nbt::gpByteArray()
- Nbt::gpIntArray()
- Nbt::gpLongArray()
- Nbt::gpList()
- Nbt::gpCompound()

The PureData version differs from the normal version in that the PureData version constructs tags that apply to elements that are lists. (The Tag obtained by the normal version can also be used as a List element, but when added to the List, the Tag name is deleted and converted to PureData.)

The above function can also be implemented through the constructor of Nbt::Tag, such as Nbt::Tag(Nbt::Int, false) is equivalent to Nbt::gInt(), and Nbt::Tag(Nbt::Int, true) is equivalent to Nbt::gpInt().

### 3. Read and modify Tag values

For the basic data type Tag, call the corresponding get or set function directly.

For example, for a Tag of type Nbt::Int

```cpp
//...
Nbt::Tag num = Nbt::gInt("Num", 1);
num.getInt();	// return 1.
num.setInt(10);
num.getInt();	// return 10.
//...
```

For the array type Tag (ByteArray, IntArray, LongArray), you can use the corresponding get function to get the entire data object (wrapped in std::vector) and manipulate it directly, and for simple add and delete elements you can use the corresponding add and remove functions.

For example, for an Nbt::IntArray Tag

```cpp
//...
Nbt::Tag intArray = Nbt::gIntArray("Nums");
intArray.addInt(1);			// add element.
intArray.addInt(2);
intArray.addInt(3);
intArray.remove(0);			// remove element by index.
std::vector<int32> *data = intArray.getIntArray();
int first = (*data)[0];		// first = 2.
//...
```

For Compound data types (List, Compound), addMember can be used for add and delete operations with removeMember and << operator overloads, and getMember or [] operator overloads can be used for read operations.

For Compound tags, the [] operator overloading function can take the Tag's location index in Compound and the Tag's name as arguments (if there is a Tag with the same name, return the Tag with the smallest index value, If not found, an error will be reported, so you should use a function such as HasMember() to determine before obtaining the element).

The [] operator overload function of the List tag can only take an index as an argument.

```cpp
//...
Nbt::Tag root = Nbt::gCompound("compound");
root.addMember(gInt("Num1", 1));		 // add elementn by addMember().
root << gInt("Num2", 2);				// add elementn by << operation overload.
root.removeMember("Num1");				// remove element by name.
root.removeMember(0);			    	// remove element by index.
root.addMember(gString("str", "text"));
root["str"].setString("text2");			// get element by name.
std::string str = root[0].getString();	 // get element by index. str = "text2".

Nbt::Tag list = Nbt::gList("list", Nbt::Int);	// In addition to the name, you also need to specify the data type of the elements in the List to construct List Tag.
list.addMember(gpInt(1));			// add elementn by addMember(), It is recommended to use the gp family of functions to construct elements within a List.
list << gpInt(2);				   // add elementn by << operation overload.
int num = list[0].getInt();		    // num = 1
//...
```



